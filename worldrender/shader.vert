out vec3 Normal;
out vec3 vPos;
out vec3 camPos;
out vec3 eyePos;
out vec3 tePos;
out mat4 tessP;
out mat4 tessV;
out float p;
ATTRIBUTE(vec3, UV, 0);

SAMPLER(sampler2D, HeightMap, 0);

uniform mat4 Projection;
uniform mat4 View;
uniform vec3 P[4];
uniform vec3 N[4];
uniform float RimSize;

struct Vertex { vec3 p; vec3 n; };

vec3 extractCameraPos(const mat4 a_modelView)
{
  mat3 rotMat = mat3(a_modelView);
  vec3 d = vec3(a_modelView[3]);

  vec3 retVec = -d * rotMat;
  return retVec;
}
Vertex interpolateLinear(Vertex v0, Vertex v1, float t)
{
  vec3 n = normalize(mix(v0.n, v1.n, t));
  vec3 p = mix(v0.p, v1.p, t);

  Vertex result;
  result.p = p;
  result.n = n;
  return result;
}

Vertex interpolate(Vertex v0, Vertex v1, float t)
{
  if (1.0 - dot(v0.n, v1.n) < 0.001)
    return interpolateLinear(v0, v1, t);

  // slerp normal
  float theta2 = acos(dot(v0.n, v1.n));
  float k = 1.0 - t;
  vec3 n = normalize(sin(k*theta2)*v0.n + sin(t*theta2)*v1.n);

  // compute position
  float theta = theta2 * 0.5;
  float gamma = theta - theta2*t;
  float tan_theta = tan(theta);
  float x = 1.0 - tan(gamma)/tan_theta;
  float y = 1.0/sin(theta) - 1.0/(cos(gamma)*tan_theta);
  vec3 v = (v1.p - v0.p) * 0.5;
  vec3 p = v0.p + x*v + y*n*length(v);

  Vertex result;
  result.p = p;
  result.n = n;
  return result;
}

float sampleHeight(vec2 uv)
{
  return texture(HeightMap, uv).r;
}

vec3 computeNormal(vec2 uv, float xyscale)
{
  vec3 offs = vec3(HeightMap_pixel_size.x, 0.0,
           HeightMap_pixel_size.y);
  float x0 = sampleHeight(uv - offs.xy);
  float x1 = sampleHeight(uv + offs.xy);
  float y0 = sampleHeight(uv - offs.yz);
  float y1 = sampleHeight(uv + offs.yz);
  return normalize(vec3(x0 - x1, 2.0*xyscale, y0 - y1));
}

void main()
{
  Vertex a; a.p = P[0]; a.n = N[0];
  Vertex b; b.p = P[1]; b.n = N[1];
  Vertex c; c.p = P[2]; c.n = N[2];
  Vertex d; d.p = P[3]; d.n = N[3];

  Vertex p = interpolate(a, b, UV.x);
  Vertex q = interpolate(c, d, UV.x);
  Vertex v = interpolate(p, q, UV.y);

  vec2 uv = mix(HeightMap_corners[0], HeightMap_corners[1], UV.xy);

  float height = sampleHeight(uv) - RimSize*UV.z;
  vec3 normal = computeNormal(uv, length(q.p - p.p) / 29.0);
  vec3 n = v.n;
  vec3 t = normalize(cross(n, q.p - p.p));
  vec3 bi = normalize(cross(t, n));
  Normal = normalize(mat3(t, n, bi) * normal);

  vPos = v.p + v.n*height;
  tessP = Projection;
  tessV = View;
//  gl_Position = Projection * View * vec4(v.p + v.n*height, 1.0);
  camPos = extractCameraPos(Projection * View);

  n.x = dot(vPos, t);
  n.y = dot(vPos, bi);
  n.z = dot(vPos, n);
  eyePos = normalize(n);
}
