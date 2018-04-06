#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>
#include <glad/glad.h>
#include "Shader.hpp"
#include "Utils.hpp"
#include "Simplex.hpp"

#define U64(x) ((uint64_t)(x))

#define PATCH_SIZE 32.0f
#define PATCH_MULTIPLIER 1.f
#define PLANET_RADIUS 6371000.0f
#define FREQUENCY 0.00001f
#define OCTAVE 8.f
#define SEED 8848.f

#if defined(PATCH_SIZE) && defined(PATCH_MULTIPLIER) && defined(PLANET_RADIUS)
  #define PI_RADIUS 2.0f*PI*PLANET_RADIUS
  #define PATCH_SIZE_VERT PATCH_SIZE * PATCH_MULTIPLIER
  #define PATCH_SIZE_QUAD PATCH_SIZE_VERT-1.f
  #define MAX_LOD Log2(PI_RADIUS/PATCH_SIZE_QUAD)-2.f
  #define MAX_SKIRT PI_RADIUS/(4*PATCH_SIZE_QUAD)*FREQUENCY*OCTAVE*SEED
#endif // defined

#define TESS_PROGRAM 1

struct quadId
{
  uint64_t value;
};

#define GET_BITS(value, first, count) (((value)>>U64(first)) & ((U64(1)<<U64(count))-U64(1)))
#define QUAD_ROOT 64u-4u
#define QUAD_DEPTH 64u-9u
#define QUAD_INDEX QUAD_DEPTH
inline uint64_t getQuadRoot(quadId id)  { return GET_BITS(id.value, QUAD_ROOT, 3u); }
inline uint64_t getQuadDepth(quadId id) { return GET_BITS(id.value, QUAD_DEPTH, 5u); }
inline uint64_t getQuadIndex(quadId id) { return GET_BITS(id.value, 0u, QUAD_DEPTH); }
#undef QUAD_INDEX
#undef QUAD_DEPTH
#undef QUAD_ROOT
#undef GET_BITS

inline uint64_t getChildIndex(quadId id)
{
  uint64_t depth = getQuadDepth(id);
  return (id.value >> U64(2*(depth - 1))) & U64(3);
}

inline quadId getParentID(quadId id)
{
  uint64_t depth = getQuadDepth(id);
  assert(depth != 0);
  uint64_t depthBit = U64(1) << U64(64 - 9);
  uint64_t mask = ~(U64(3) << U64(2*(depth - 1)));
  uint64_t value = (id.value - depthBit) & mask;
  return {value};
}

inline quadId makeChildId(uint64_t childIndex, quadId id)
{
  uint64_t depth = getQuadDepth(id);
  assert(childIndex < U64(4));
  assert(depth + U64(1) < U64(32));
  uint64_t depthBit = U64(1) << U64(64 - 9);
  uint64_t value = (id.value + depthBit) | (childIndex << U64(2*depth));
  return {value};
}

inline quadId makeRootId(uint64_t root)
{
  assert(root < U64(6)); // assegura que os ids tem o maior bit, logo id zero é inválido
  uint64_t highest_bit = U64(1) << U64(64 - 1);
  uint64_t value = highest_bit | (root << U64(64 - 4));
  return {value};
}

struct Quad
{
  glm::vec3 p[4];
  quadId id;
};


#define CACHE_MAX 1024
#define MAP_MAX 1499 // prime

struct HeightMapCache
{
  int count;
  quadId quadsIds[MAP_MAX];
  GLuint height_maps[MAP_MAX];
  uint32_t lastTick[MAP_MAX];
};

int findInHeightMap(quadId find, quadId key, const HeightMapCache &cache)
{
  assert(key.value != U64(0));

  /// TODO: Better hash

  uint32_t hash = uint32_t(key.value) ^ uint32_t(key.value >> 32);
  for (int i = 0; i < MAP_MAX; i++)
  {
    int index = (hash + i) % MAP_MAX;
    if (cache.quadsIds[index].value == find.value)
    {
      return index;
    }
  }

  return -1;
}

#undef U64

struct HeightMapGenerator
{
  float (*getHeight)(int, int, const glm::vec3 &);
  void (*generateHeightMap)(float *, int, const Quad &, int);
};

template<typename F>
HeightMapGenerator createHeightMap()
{
  struct Gen
  {
    static float getHeight(int depth, int max_depth, const glm::vec3 &p)
    {
      return F()(p, depth, max_depth);
    }

    static void generateHeightMap(float *data, int dim, const Quad &q,
                    int max_depth)
    {
      TIMED_FUNCTION();

      assert(dim > 3);

      int depth = getQuadDepth(q.id);

      glm::vec3 v0 = q.p[1] - q.p[0];
      glm::vec3 v1 = q.p[3] - q.p[2];

      double div = 1.0/(dim - 3);
      for (int y = 0; y < dim; y++)
      {
        for (int x = 0; x < dim; x++)
        {
          double u = (x - 1)*div;
          double v = (y - 1)*div;

          glm::vec3 p0 = q.p[0] + v0 * float(u);
          glm::vec3 p1 = q.p[2] + v1 * float(u);
          glm::vec3 v2 = p1 - p0;
          glm::vec3 p = p0 + v2 * float(v);

          data[y*dim + x] = F()(p, depth, max_depth);
        }
      }
    }
  };

  HeightMapGenerator result;
  result.getHeight = Gen::getHeight;
  result.generateHeightMap = Gen::generateHeightMap;
  return result;
};

struct Planet
{
  double radius;
  int maxLOD;
  int renderTick;
  float maxRim;
  DrawItem patch;
  Texture hmap;
  struct
  {
    Uniform proj;
    Uniform view;
    Uniform p;
    Uniform n;
    Uniform rim_size;
    TexUniforms hmap;
  } uniforms;
  List<Quad> quads;
  HeightMapCache cache;
  HeightMapGenerator hmap_gen;
};

struct TextureRect
{
  GLuint texture;
  glm::vec2 corners[2];
  glm::vec2 pixel_size;
};

struct cameraInfo
{
  glm::vec3 position;
  glm::mat3 rotation;
  float projection;
  float aspectRatio;
  float nearPlane;
  float farPlane;

  const float sqrLen()
  {
    return glm::length(position);
  }
  const float qLength()
  {
    return Q_rsqrt(position.x)+Q_rsqrt(position.y)+Q_rsqrt(position.z);
  }
  const float length()
  {
    return Square(position.x)+Square(position.y)+Square(position.z);
  }
};

TextureRect getHeightMapForQuad(Planet &planet, const Quad &q,
                int &generations_per_frame_left)
{
  const int dim = 32;

  TextureRect result = {};
  result.corners[0] = glm::vec2(1.5f / dim);
  result.corners[1] = glm::vec2((dim - 1.5f) / dim);
  result.pixel_size = glm::vec2(1.0f / dim);

  HeightMapCache &cache = planet.cache;

  int index = findInHeightMap(q.id, q.id, cache);

  if (index < 0)
  {
    int depth = getQuadDepth(q.id);
    if (generations_per_frame_left <= 0 && depth > 0)
    {
      quadId parentId = getParentID(q.id);

      index = findInHeightMap(parentId, parentId, cache);
      if (index >= 0)
      {
        int childIndex = getChildIndex(q.id);

        float x0 = 1.5f;
        float y0 = x0;
        float x1 = dim / 2.0f - 0.5f;
        float y1 = x1;

        if (childIndex == 1 || childIndex == 3)
        {
          x0 = dim / 2.0f + 0.5f;
          x1 = dim - 1.5f;
        }

        if (childIndex == 2 || childIndex == 3)
        {
          y0 = dim / 2.0f + 0.5f;
          y1 = dim - 1.5f;
        }

        result.corners[0] = glm::vec2(x0 / dim, y0 / dim);
        result.corners[1] = glm::vec2(x1 / dim, y1 / dim);
        result.pixel_size = glm::vec2(((dim / 2.0f - 1.0f) / (dim - 3.f)) / dim);
      }
    }

    if (generations_per_frame_left > 0 || index < 0)
    {
      generations_per_frame_left--;

      float data[dim*dim];
      planet.hmap_gen.generateHeightMap(data, dim, q, planet.maxLOD);
      GLuint height_map = CreateTexture2D(dim, dim, GL_RED, GL_FLOAT, data);

      if (cache.count == CACHE_MAX)
      {
        int lru = 0;
        int delta_ticks = -1;

        for (int i = 0; i < MAP_MAX; i++)
        {
          uint32_t t = cache.lastTick[i];
          int delta = planet.renderTick - t;
          if (cache.quadsIds[i].value != 0 && delta > delta_ticks)
          {
            lru = i;
            delta_ticks = delta;
          }
        }

        DeleteTexture(cache.height_maps[lru]);
        cache.quadsIds[lru] = quadId{0};
        cache.count--;
      }

      index = findInHeightMap(quadId{0}, q.id, cache);
      cache.quadsIds[index] = q.id;
      cache.height_maps[index] = height_map;
      cache.count++;
    }
  }

  cache.lastTick[index] = planet.renderTick;
  result.texture = cache.height_maps[index];
  return result;
}


bool InitPlanet(Planet &p, double radius, HeightMapGenerator hmap_gen)
{
  const int patchSizeVerts = PATCH_SIZE_VERT; // número de vertices em um patch
  const int patchSizeQuads = PATCH_SIZE_QUAD; //numero de quads em um patch
  const int rimPerVertex = 4*patchSizeVerts; //tamanho do corte em vertices
  const int totalVertexes = Square(patchSizeVerts) + rimPerVertex; //máximo de vértices pra serem passados para gpu
  const int indexPerReset = 2; //default is 2
  const int indexPerStrip =
    2 + patchSizeQuads*2 + indexPerReset;
  const int rimPerIndex = patchSizeQuads*4 + 2*indexPerStrip;
  const int totalIndex =
    patchSizeQuads*indexPerStrip - indexPerReset + rimPerIndex;

  GLuint shader = CreateShaderFromSource("shader");
  if (shader == 0)
  {
    return false;
  }

  glm::vec3 verts[totalVertexes];
  {
    int n = 0;

    double div = 1.0f / patchSizeQuads;

    for (int x = 0; x < patchSizeVerts; ++x)
      verts[n++] = glm::vec3(x*div, 0.0f, 1.0f); // rim

    for (int y = 0; y < patchSizeVerts; ++y)
    {
      verts[n++] = glm::vec3(0.0f, y*div, 1.0f); // rim

      for (int x = 0; x < patchSizeVerts; ++x)
        verts[n++] = glm::vec3(x*div, y*div, 0.0f);

      verts[n++] = glm::vec3(1.0f, y*div, 1.0f); // rim
    }

    for (int x = 0; x < patchSizeVerts; ++x)
      verts[n++] = glm::vec3(x*div, 1.0f, 1.0f); // rim

    assert(n == totalVertexes);
  }

  uint32_t indexes[totalIndex];
  {
    int n = 0;

    uint32_t v0 = 0;
    uint32_t v1 = patchSizeVerts + 1;

    for (int x = 0; x < patchSizeVerts; ++x)
    {
      indexes[n++] = v0++;
      indexes[n++] = v1++;
    }

    // reset
    indexes[n++] = v1-1;
    indexes[n++] = v0;
    v1++;

    for (int y = 0; y < patchSizeQuads; ++y)
    {
      for (int x = 0; x < patchSizeVerts + 2; ++x)
      {
        indexes[n++] = v0++;
        indexes[n++] = v1++;
      }
      if (y+1 < patchSizeQuads)
      {
        // reset
        indexes[n++] = v1-1;
        indexes[n++] = v0;
      }
    }

    v0++;
    // reset
    indexes[n++] = v1-1;
    indexes[n++] = v0;

    // rim
    for (int x = 0; x < patchSizeVerts; ++x)
    {
      indexes[n++] = v0++;
      indexes[n++] = v1++;
    }

    assert(n == totalIndex);
  }

  VertexFormat vf = {};
  AddVertexAttribute(vf, 0, 3, GL_FLOAT, false);

  GLuint vertex_buffer = CreateVertexBuffer(sizeof(verts), verts);
  GLuint index_buffer = CreateIndexBuffer(sizeof(indexes), indexes);
  GLuint vertex_array = CreateVertexArray(vertex_buffer, vf, index_buffer);

  p.hmap.texture = 0;
  p.hmap.bind_index = 0;

  InitTexUniforms(p.uniforms.hmap, shader, "HeightMap");

  InitUniform(p.uniforms.proj, shader, "Projection", Uniform::MAT4);
  InitUniform(p.uniforms.view, shader, "View", Uniform::MAT4);
  InitUniform(p.uniforms.p, shader, "P", Uniform::VEC3, 4);
  InitUniform(p.uniforms.n, shader, "N", Uniform::VEC3, 4);
  InitUniform(p.uniforms.rim_size, shader, "RimSize", Uniform::FLOAT);

  p.radius = radius;
  p.hmap_gen = hmap_gen;

  p.maxLOD = MAX_LOD;
  p.maxRim = MAX_SKIRT;

  DrawItem &d = p.patch;
  d.shader = shader;
  d.vertex_array = vertex_array;
  d.textures = &p.hmap;
  d.texture_count = 1;
  d.uniforms = &p.uniforms.proj;
  d.uniform_count = GetUniformCountFromSize(sizeof(p.uniforms));
  #ifndef TESS_PROGRAM
    d.primitive_mode = GL_TRIANGLE_STRIP;
  #else
    d.primitive_mode = GL_PATCHES;
  #endif
  d.index_type = GL_UNSIGNED_INT;
  d.first = 0;
  d.count = totalIndex;
  d.draw_arrays = false;

  return true;
}

void InitcameraInfo(cameraInfo &info, float fovy_rad, float aspectRatio,
          float nearPlane, float farPlane)
{
  info.projection = 1.0f/Tan(0.5*fovy_rad);
  info.aspectRatio = aspectRatio;
  info.nearPlane = nearPlane;
  info.farPlane = farPlane;
}

void ProcessQuad(Planet &planet, const Quad &q, cameraInfo &cam, int lod)
{
  if (lod == 0)
  {
    // No more splitting
    ListAdd(planet.quads, q);
    return;
  }

  glm::vec3 mid_n = glm::normalize(q.p[0] + q.p[1] + q.p[2] + q.p[3]);
  glm::vec3 mid = mid_n * float(planet.radius);

  glm::vec3 p[5];
  for (int i = 0; i < 4; i++)
  {
    float height = planet.hmap_gen.getHeight(0, 1, q.p[i]);
    p[i] = q.p[i] + glm::normalize(q.p[i]) * height;
  }
  float height = planet.hmap_gen.getHeight(0, 1, mid);
  p[4] = mid + mid_n * height;

  bool split = false;

  double d = (sqrLength(p[3] - p[0]) + sqrLength(p[2] - p[1]))/(1.0f + 2.5f*lod / planet.maxLOD);

  for (int i = 0; i < 5; i++)
  {
    if (sqrLength(p[i] - cam.position)*2.0 < d)
    {
      split = true;
      break;
    }
  }

  if (!split)
  {
    ListAdd(planet.quads, q);
    return;
  }

  #define VERT(i, j) glm::normalize(q.p[i] + q.p[j]) * (float)planet.radius
  #define QUAD(a, b, c, d, id) (Quad){verts[a], verts[b], verts[c], verts[d], id}

  glm::vec3 verts[] =
  {
    q.p[0], VERT(0, 1), q.p[1],
    VERT(0, 2), mid, VERT(1, 3),
    q.p[2], VERT(2, 3), q.p[3],
  };

  ProcessQuad(planet, QUAD(0, 1, 3, 4, makeChildId(0, q.id)), cam, lod - 1);
  ProcessQuad(planet, QUAD(1, 2, 4, 5, makeChildId(1, q.id)), cam, lod - 1);
  ProcessQuad(planet, QUAD(3, 4, 6, 7, makeChildId(2, q.id)), cam, lod - 1);
  ProcessQuad(planet, QUAD(4, 5, 7, 8, makeChildId(3, q.id)), cam, lod - 1);



#undef VERT
#undef QUAD
}

void RenderPlanet(Planet &planet, cameraInfo cam)
{
  ListResize(planet.quads, 0);

#define VERT(x, y, z) glm::normalize(glm::vec3(x, y, z)) * (float)planet.radius
#define QUAD(a, b, c, d, id) (Quad){verts[a], verts[b], verts[d], verts[c], id}

  // Cubo para ser transformado em esfera
  glm::vec3 verts[] =
  {
    VERT(-1, -1, -1), // 0
    VERT( 1, -1, -1), // 1
    VERT( 1,  1, -1), // 2
    VERT(-1,  1, -1), // 3
    VERT(-1, -1,  1), // 4
    VERT( 1, -1,  1), // 5
    VERT( 1,  1,  1), // 6
    VERT(-1,  1,  1), // 7
  };

  ProcessQuad(planet, QUAD(0, 1, 2, 3, makeRootId(0)), cam, planet.maxLOD); // front
  ProcessQuad(planet, QUAD(1, 5, 6, 2, makeRootId(1)), cam, planet.maxLOD); // right
  ProcessQuad(planet, QUAD(5, 4, 7, 6, makeRootId(2)), cam, planet.maxLOD); // back
  ProcessQuad(planet, QUAD(4, 0, 3, 7, makeRootId(3)), cam, planet.maxLOD); // left
  ProcessQuad(planet, QUAD(3, 2, 6, 7, makeRootId(4)), cam, planet.maxLOD); // top
  ProcessQuad(planet, QUAD(4, 5, 1, 0, makeRootId(5)), cam, planet.maxLOD); // bottom

#undef VERT
#undef QUAD


  glm::mat4 projection = {};
  {
    double f = cam.farPlane;
    double n = cam.nearPlane;

    projection[0][0] = cam.projection/cam.aspectRatio;
    projection[1][1] = cam.projection;
    projection[2][2] = (f + n) / (f - n);
    projection[2][3] = 1.0;
    projection[3][2] = -2.0*f*n / (f - n);
  }

  glm::mat4 view = {};
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
      view[i][j] = cam.rotation[j][i];
  }
  view[3][3] = 1.0f;

  planet.uniforms.proj.value.m4 = projection;
  planet.uniforms.view.value.m4 = view;

  // NOTE: a hack
  int generations_per_frame = 100;

  for (GLuint i = 0; i < planet.quads.num; ++i)
  {
    Quad &q = planet.quads.data[i];

    TextureRect texrect = getHeightMapForQuad(planet, q,
                          generations_per_frame);
    planet.hmap.texture = texrect.texture;
    planet.uniforms.hmap.corners.value.v2[0] = texrect.corners[0];
    planet.uniforms.hmap.corners.value.v2[1] = texrect.corners[1];
    planet.uniforms.hmap.pixel_size.value.v2[0] = texrect.pixel_size;

    for (GLuint j = 0; j < 4; ++j)
    {
      glm::vec3 p = q.p[j] - cam.position;
      glm::vec3 n = glm::normalize(q.p[j]);
      planet.uniforms.p.value.v3[j] = glm::vec3(p.x, p.y, p.z);
      planet.uniforms.n.value.v3[j] = glm::vec3(n.x, n.y, n.z);
    }

    float rim_size = planet.maxRim;
    int depth = getQuadDepth(q.id) - 1;
    if (depth > 0) rim_size /= (2 << depth);
    planet.uniforms.rim_size.value.f[0] = rim_size;

    Draw(planet.patch);
  }

}

int main(int argc, char **argv)
{
  // Inicialização do SDL
  //
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    LOG_ERROR("Couldn't initialize SDL: %s", SDL_GetError());
    return 1;
  }

  DEFER(SDL_Quit());

  // Criação da janela
  //
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  GLuint window_w = 1280u;
  GLuint window_h = 720u;

  SDL_Window *window = SDL_CreateWindow("Planet",
                      SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED,
                      window_w, window_h,
                      SDL_WINDOW_OPENGL|
                      SDL_WINDOW_RESIZABLE);

  if (window == nullptr)
  {
    LOG_ERROR("Couldn't create window: %s", SDL_GetError());
    return 1;
  }

  DEFER(SDL_DestroyWindow(window), SDL_Window*, window);


  // Criação de contextos
  //
  SDL_GLContext context = SDL_GL_CreateContext(window);

  if (context == nullptr)
  {
    LOG_ERROR("Couldn't create GL context: %s", SDL_GetError());
    return 1;
  }

  DEFER(SDL_GL_DeleteContext(context), SDL_GLContext, context);


  // Inicializar o GLEW
  //
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
  {
    LOG_ERROR("Failed to initialize GLAD");
    return -1;
  }

  if (GLVersion.major < 3)
  {
    LOG_ERROR("Your system doesn't support OpenGL >= 2!");
    return -1;
  }

  // Configurações básicas

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

  bool wire_frame = false;
  float temp_rim_size = 0.0f;

  double radius = PLANET_RADIUS;

  struct Perlin
  {
    inline float operator()(glm::vec3 p, int depth, int max_depth)
    {
      int octaves = 6 + 12 * depth / max_depth;
      p *= 0.00001;
      //float h = Simplex::fBm(p,octaves,2.0f,0.55f);
      //float h = Simplex::ridgedMF(p, 1.f, octaves,2.0f,0.55f);
      float h = Simplex::iqfBm(p, octaves, 2.0f, 0.55f);
      return h * 8848.0f;
    }
  };

  struct ConstantZero
  {
    inline float operator()(glm::vec3 p, int depth, int max_depth)
    {
      return 0.0f;
    }
  };

  HeightMapGenerator hmap_gen = createHeightMap<Perlin>();
  //HeightMapGenerator hmap_gen = createHeightMap<ConstantZero>();

  Planet planet = {};
  if (!InitPlanet(planet, radius, hmap_gen))
  {
    return 1;
  }

  struct Camera
  {
    glm::vec3 position;
    glm::vec3 angles;
  };

  struct SaveState
  {
    Camera active_camera;
    Camera saved_cameras[12];
  };

  SaveState save = {};
  save.active_camera.position.z = -radius - 10.0f;

  const char *save_file_name = "save";

  // Load state
  {
    FILE *f = fopen(save_file_name, "rb");
    if (f)
    {
      SaveState temp = {};
      size_t size = sizeof(SaveState);

      if (fread(&temp, 1, size, f) == size)
      {
        save = temp;
      }
      else
      {
        LOG_WARNING("Couldn't read save file.");
      }

      fclose(f);
    }
  }

  Camera cam = save.active_camera;

  double move_speed = 1000.0f; //metro por segundo
  float look_speed = 2.0f; //radiano por segundo

  // Start looping
  //
  uint32_t last_t = SDL_GetTicks();
  bool running = true;

  while (running)
  {
    START_FRAME();

    // Handle events
    BEGIN_TIMED_BLOCK(EventHandling);

    for (SDL_Event event; SDL_PollEvent(&event); )
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          running = false;
          break;
        }

        case SDL_WINDOWEVENT:
        {
          switch(event.window.event)
          {
            case SDL_WINDOWEVENT_RESIZED:
            {
              window_w = event.window.data1;
              window_h = event.window.data2;
              glViewport(0, 0, window_w, window_h);
              break;
            }

            default: break;
          }

          break;
        }

        case SDL_KEYDOWN:
        {
          bool shift = event.key.keysym.mod & KMOD_SHIFT;

          switch (event.key.keysym.scancode)
          {
            case SDL_SCANCODE_ESCAPE:
            {
              running = false;
              break;
            }
            #define SPEED_MOVE(index) \
            case SDL_SCANCODE_##index: move_speed = 1.0e##index; break;
            SPEED_MOVE(1);
            SPEED_MOVE(2);
            SPEED_MOVE(3);
            SPEED_MOVE(4);
            SPEED_MOVE(5);
            SPEED_MOVE(6);
            SPEED_MOVE(7);
            SPEED_MOVE(8);
            #undef SPEED_MOVE

            #define SAVE_POS(index) \
            case SDL_SCANCODE_F##index: \
            { \
              Camera &saved = save.saved_cameras[index-1]; \
              if (shift) saved = cam; \
              else if (sqrLength(saved.position) > 0.001) cam = saved; \
            } break
            SAVE_POS(1);
            SAVE_POS(2);
            SAVE_POS(3);
            SAVE_POS(4);
            SAVE_POS(5);
            SAVE_POS(6);
            SAVE_POS(7);
            SAVE_POS(8);
            SAVE_POS(9);
            SAVE_POS(10);
            SAVE_POS(11);
            SAVE_POS(12);
            #undef SAVE_POS

            // Modo wireframe
            case SDL_SCANCODE_P:
            {
              wire_frame = !wire_frame;
              glPolygonMode(GL_FRONT_AND_BACK, wire_frame ? GL_LINE : GL_FILL);
              break;
            }

            // Toggle rims
            case SDL_SCANCODE_K:
            {
              float temp = planet.maxRim;
              planet.maxRim = temp_rim_size;
              temp_rim_size = temp;
              break;
            }

            case SDL_SCANCODE_T:
            {
              TOGGLE_TIMING();
              break;
            }

            default: break;
          }

          break;
        }

        default: break;
      }
    }

    END_TIMED_BLOCK(EventHandling);

    // Update
    BEGIN_TIMED_BLOCK(UpdateSimulation);

    const Uint8 *keys = SDL_GetKeyboardState(nullptr);

    double dt;
    int dt_ms;
    {
      Uint32 ticks = SDL_GetTicks();
      Uint32 delta = ticks - last_t;
      last_t = ticks;
      dt = delta / 1000.0;
      dt_ms = delta;
    }

    {
      const int tri_count = planet.quads.num * 29*29*2;

      char title[100];
      snprintf(title, sizeof(title),
           "frametime: %d, fps: %d, tris: %d, quads: %d",
           dt_ms, (int)(1.0/dt), tri_count, planet.quads.num);
      SDL_SetWindowTitle(window, title);
    }

    glm::vec3 move = glm::vec3(keys[SDL_SCANCODE_D] - keys[SDL_SCANCODE_A], 0,
             keys[SDL_SCANCODE_W] - keys[SDL_SCANCODE_S]);
    glm::vec3 look = glm::vec3(keys[SDL_SCANCODE_DOWN] - keys[SDL_SCANCODE_UP],
             keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT], 0);

    cam.angles += look * look_speed * float(dt);

    glm::vec3 up = glm::vec3(glm::normalize(cam.position));
    glm::vec3 right;
    if (1.0f - glm::dot(up, glm::vec3(0.0f, 1.0f, 0.0f)) < 0.1f)
      right = glm::normalize(glm::cross(up, glm::vec3(0.0f, 0.0f, 1.0f)));
    else
      right = glm::normalize(glm::cross(up, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 forward = glm::normalize(glm::cross(right, up));

    glm::mat3 baseRotation = mat3FromBasisVectors(right, up, forward);

    glm::mat3 rotation = (mat3RotationY(cam.angles.y) *
                         mat3RotationX(cam.angles.x) *
                         mat3RotationZ(cam.angles.z));

    rotation = baseRotation * rotation;

    cam.position += (glm::vec3(rotation[0]) * move.x +
                    glm::vec3(rotation[1]) * move.y +
                    glm::vec3(rotation[2]) * move.z) * float(move_speed) * float(dt);

    END_TIMED_BLOCK(UpdateSimulation);

    // Render
    BEGIN_TIMED_BLOCK(Rendering);

    const float fovy = DegToRad(50.0f);
    const float aspect = (float)window_w/window_h;
    const float farPlane = 20000000.0f;

    cameraInfo cam_info;
    cam_info.position = cam.position;

    //Calcula a dimensão unitária da posição da camera para ajuste do near e do far (este parametro pode ser o mesmo a ser usado na transição de view)
    const float camLength = (cam_info.length())/1e11;
    const float planeScale = (camLength>420)?1:100;
    std::cout << cam_info.farPlane << std::endl;
    cam_info.rotation = rotation;
    cam_info.projection = 1.0f/Tan(0.5f*fovy);
    cam_info.aspectRatio = aspect;

    cam_info.nearPlane = camLength/planeScale;
    cam_info.farPlane = farPlane*camLength;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    RenderPlanet(planet, cam_info);

    END_TIMED_BLOCK(Rendering);

    // Draw screen
    BEGIN_TIMED_BLOCK(DrawScreen);

    SDL_GL_SwapWindow(window);
    SDL_Delay(10);

    END_TIMED_BLOCK(DrawScreen);

    // Check GL errors
    for (GLenum err; (err = glGetError()) != GL_NO_ERROR; )
    {
      switch (err)
      {
        #define CASE(e) case e: LOG_ERROR("GL error: " #e); break
        CASE(GL_INVALID_ENUM);
        CASE(GL_INVALID_VALUE);
        CASE(GL_INVALID_OPERATION);
        CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
        CASE(GL_OUT_OF_MEMORY);
        CASE(GL_STACK_UNDERFLOW);
        CASE(GL_STACK_OVERFLOW);
        default: LOG_ERROR("Unknown GL error: %d", err);
        #undef CASE
      }
    }
  }

  //Salva o estado da camera
  {
    save.active_camera = cam;

    FILE *f = fopen(save_file_name, "wb");
    if (f)
    {
      size_t size = sizeof(SaveState);

      if (fwrite(&save, 1, size, f) != size)
      {
        LOG_ERROR("Couldn't write save file.");
      }

      fclose(f);
    }
    else
    {
      LOG_ERROR("Couldn't open save file for writing.");
    }
  }

  return 0;
}
