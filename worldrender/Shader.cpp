
#include "Shader.hpp"
#include <string>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Utils.hpp"
#include <cstring>
#include <cassert>
#include <glad/glad.h>

using namespace std;

#include <stdlib.h>
#include <string.h>

string LoadShaderFromFile(string path)
{
// Read the Vertex Shader code from the file
  std::string ShaderCode;
  std::ifstream ShaderStream(path, std::ios::in);
  if(ShaderStream.is_open())
  {
    std::string Line = "";
    while(getline(ShaderStream, Line))
    ShaderCode += "\n" + Line;
    ShaderStream.close();
  }
  else
  {
    printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", path.c_str());
    getchar();
    return 0;
  }
  return ShaderCode;
}


inline GLuint CreateBuffer(GLenum target, unsigned int size, const void *data)
{
  glBindVertexArray(0);

  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, size, data, GL_STATIC_DRAW);
  return buffer;
}

GLuint CreateVertexBuffer(unsigned int size, const void *data)
{
  return CreateBuffer(GL_ARRAY_BUFFER, size, data);
}

GLuint CreateIndexBuffer(unsigned int size, const void *data)
{
  return CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, size, data);
}

inline int GetAttributeTypeSize(GLenum type)
{
  switch (type)
  {
    case GL_FLOAT: return sizeof(float);
    default: assert(0 && "Invalid attribute type!");
  }
  return 0;
}

void AddVertexAttribute(VertexFormat &format, int index, int size,
GLenum type, bool normalize)
{
  assert(0 <= index && index < MAX_VERTEX_ATTRIBUTES);
  assert(1 <= size && size <= 4);
  assert(format.attributes[index].size == 0);
  format.attributes[index].size = size;
  format.attributes[index].type = type;
  format.attributes[index].normalize = normalize;
  format.attributes[index].pointer = ((char*)0 + format.vertex_size);
  format.vertex_size += size * GetAttributeTypeSize(type);
}

GLuint CreateVertexArray(GLuint vertex_buffer, VertexFormat &format,
GLuint index_buffer)
{
  assert(vertex_buffer != 0);
  assert(format.vertex_size > 0);

  GLuint vertex_array;
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

  for (int index = 0; index < MAX_VERTEX_ATTRIBUTES; index++)
  {
    auto &a = format.attributes[index];
    if (a.size == 0) continue;

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, a.size, a.type,
    a.normalize ? GL_TRUE : GL_FALSE,
    format.vertex_size, a.pointer);
  }

  return vertex_array;
}

// Shaders

GLuint CompileShader(GLenum type, const char *source)
{
  const char *source_list[] = {
    // 0 = Common header
    "#version 400 core\n"
    "#define SAMPLER(type, name, index) \\\n"
    "uniform type name; \\\n"
    "uniform vec2 name##_corners[2]; \\\n"
    "uniform vec2 name##_pixel_size\n",
    "",    // 1 = Shader specific header
    source // 2 = User code
  };

  if (type == GL_VERTEX_SHADER)
  {
    source_list[1] =
    "#define VERTEX_SHADER 1\n"
    "#define TESS_CONTROL_SHADER 0\n"
    "#define TESS_EVALUATION_SHADER 0\n"
    "#define FRAGMENT_SHADER 0\n"
    "#define ATTRIBUTE(type, name, index) in type name\n"
    "#define VARYING(type, name) out type name\n";
  }
  else if(type == GL_TESS_CONTROL_SHADER)
  {
    source_list[1] =
    "#define VERTEX_SHADER 0\n"
    "#define TESS_CONTROL_SHADER 1\n"
    "#define TESS_EVALUATION_SHADER 0\n"
    "#define FRAGMENT_SHADER 0\n";
  }
  else if(type == GL_TESS_EVALUATION_SHADER)
  {
    source_list[1] =
    "#define VERTEX_SHADER 0\n"
    "#define TESS_CONTROL_SHADER 0\n"
    "#define TESS_EVALUATION_SHADER 1\n"
    "#define FRAGMENT_SHADER 0\n";
  }
  else if(type == GL_FRAGMENT_SHADER)
  {
    source_list[1] =
    "#define VERTEX_SHADER 0\n"
    "#define TESS_CONTROL_SHADER 0\n"
    "#define TESS_EVALUATION_SHADER 0\n"
    "#define FRAGMENT_SHADER 1\n"
    "#define VARYING(type, name) in type name\n";
  }

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 3, source_list, 0);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_TRUE)
  return shader;

  char info[256];
  glGetShaderInfoLog(shader, sizeof(info), 0, info);

  LOG_ERROR("Couldn't compile %s shader:\n%s\n",
  (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", info);
  glDeleteShader(shader);
  return 0;
}

// Parse utils
inline bool IsAlpha(char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
inline bool IsNumber(char c) { return '0' <=c && c <= '9'; }
inline bool IsAlphaNum(char c) { return IsAlpha(c) || IsNumber(c); }
inline bool IsIdent(char c) { return IsAlphaNum(c) || c == '_'; }

struct Token
{
  enum Type
  {
    END, IDENT, NUMBER, COMMA, OPAREN, CPAREN
  };

  Type type;
  const char *str;
  union
  {
    int number;
    int len;
  };
};

struct Tokenizer
{
  const char *at;
};

Token GetNextToken(Tokenizer &tokenizer)
{
  Token result = {};

  do
  {
    switch (*tokenizer.at)
    {
      case '\0': {break;}

      case 'a'...'z':
      case 'A'...'Z':
      case '_':
      {
        result.type = Token::IDENT;
        result.str = tokenizer.at;
        while (IsIdent(*tokenizer.at))
        tokenizer.at++;
        result.len = tokenizer.at - result.str;
        break;
      }

      case '0'...'9':
      {
        result.type = Token::NUMBER;
        while (IsNumber(*tokenizer.at))
        {
          result.number *= 10;
          result.number += (*tokenizer.at - '0');
          tokenizer.at++;
        }
        break;
      }

      case ',':
      {
        result.type = Token::COMMA;
        tokenizer.at++;
        break;
      }

      case '(':
      {
        result.type = Token::OPAREN;
        tokenizer.at++;
        break;
      }

      case ')':
      {
        result.type = Token::CPAREN;
        tokenizer.at++;
        break;
      }

      default:
      {
        tokenizer.at++;
        continue;
      }
    }
  }
  while (0);

  return result;
}

inline bool ExpectToken(Tokenizer &tokenizer, Token::Type type, Token &token)
{
  token = GetNextToken(tokenizer);
  if (token.type != type)
  {
    LOG_WARNING("When parsing shader source: Unexpected token.");
    return false;
  }
  return true;
}

inline bool ExpectToken(Tokenizer &tokenizer, Token::Type type)
{
  Token token;
  return ExpectToken(tokenizer, type, token);
}

struct ParseItem
{
  bool is_attribute;
  const char *name;
  int name_len;
  int index;
};

int ParseAttributesAndSamplers(const char *source, int max, ParseItem *items)
{
  int count = 0;

  Tokenizer tokenizer = {source};

  while (1)
  {
    Token token = GetNextToken(tokenizer);

    if (token.type == Token::END)   { break; }
    if (token.type != Token::IDENT) { continue; }

    bool attribute = strncmp(token.str, "ATTRIBUTE", token.len) == 0;
    if (attribute || strncmp(token.str, "SAMPLER", token.len) == 0)
    {
      Token name, index;

      if (!ExpectToken(tokenizer, Token::OPAREN))        { continue; }
      if (!ExpectToken(tokenizer, Token::IDENT))         { continue; }
      if (!ExpectToken(tokenizer, Token::COMMA))         { continue; }
      if (!ExpectToken(tokenizer, Token::IDENT, name))   { continue; }
      if (!ExpectToken(tokenizer, Token::COMMA))         { continue; }
      if (!ExpectToken(tokenizer, Token::NUMBER, index)) { continue; }
      if (!ExpectToken(tokenizer, Token::CPAREN))        { continue; }

      if (count == max)
      {
        LOG_ERROR("Too many items.");
        continue;
      }

      ParseItem &item = items[count++];
      item.is_attribute = attribute;
      item.name = name.str;
      item.name_len = name.len;
      item.index = index.number;
    }
  }

  return count;
}

GLuint CreateShaderFromSource(std::string path)
{
  std::string vSauce = LoadShaderFromFile(path+".vert");
  std::string cSauce = LoadShaderFromFile(path+".cont");
    std::string eSauce = LoadShaderFromFile(path+".eval");
    std::string fSauce = LoadShaderFromFile(path+".frag");

  GLuint vs = CompileShader(GL_VERTEX_SHADER, vSauce.c_str());
  GLuint cs = CompileShader(GL_TESS_CONTROL_SHADER, cSauce.c_str());
    GLuint es = CompileShader(GL_TESS_EVALUATION_SHADER, eSauce.c_str());
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fSauce.c_str());

    DEFER(if (vs != 0) { glDeleteShader(vs); }, GLuint, vs);
    DEFER(if (cs != 0) { glDeleteShader(cs); }, GLuint, cs);
    DEFER(if (es != 0) { glDeleteShader(es); }, GLuint, es);
    DEFER(if (fs != 0) { glDeleteShader(fs); }, GLuint, fs);

  if (vs == 0 || cs == 0 || es == 0 || fs == 0)
        return 0;

  GLuint p = glCreateProgram();

  // Parse attributes and samplers
  const int max_items = MAX_VERTEX_ATTRIBUTES + 32;
  ParseItem items[max_items];
  int item_count = ParseAttributesAndSamplers(vSauce.c_str(), max_items, items);

  for (int i = 0; i < item_count; i++)
  {
    ParseItem &item = items[i];
    if (item.is_attribute)
    {
      char name[64] = {};
      if (item.name_len >= (int)sizeof(name))
      {
        LOG_ERROR("Vertex attribute name too long.");
        continue;
      }
      if (item.index >= MAX_VERTEX_ATTRIBUTES)
      {
        LOG_ERROR("Expected attribute index between 0-%d.",
        MAX_VERTEX_ATTRIBUTES - 1);
        continue;
      }

      memcpy(name, item.name, item.name_len);
      glBindAttribLocation(p, item.index, name);
    }
  }

  glAttachShader(p, vs);
  glAttachShader(p, cs);
  glAttachShader(p, es);
  glAttachShader(p, fs);

  glLinkProgram(p);
  glDetachShader(p, vs);
  glDetachShader(p, cs);
  glDetachShader(p, es);
  glDetachShader(p, fs);

  GLint status;
  glGetProgramiv(p, GL_LINK_STATUS, &status);
  if (status == GL_TRUE)
  {
    glUseProgram(p);

    for (int i = 0; i < item_count; i++)
    {
      ParseItem &item = items[i];
      if (!item.is_attribute)
      {
        char name[64] = {};
        if (item.name_len >= (int)sizeof(name))
        {
          LOG_ERROR("Sampler name too long.");
          continue;
        }
        //if (item.index >= MAX_TEXTURE_SAMPLERS)
        //{
        //    LOG_ERROR("Expected sampler index between 0-%d.",
        //              MAX_TEXTURE_SAMPLERS - 1);
        //    continue;
        //}

        memcpy(name, item.name, item.name_len);
        GLint loc = glGetUniformLocation(p, name);
        glUniform1i(loc, item.index);
      }
    }

    return p;
  }

  char info[256];
  glGetProgramInfoLog(p, 256, 0, info);
  LOG_ERROR("Couldn't link shader:\n%s\n", info);
  glDeleteProgram(p);
  return 0;
}

void InitUniform(Uniform &u, GLuint shader, const char *name,
Uniform::Type type, int count)
{
  assert(count > 0);
  assert((int)type * count <= 16);

  u.loc = glGetUniformLocation(shader, name);
  u.type = type;
  u.count = count;

  if (u.loc < 0)
  {
    LOG_WARNING("Uniform named '%s' not found.", name);
  }
}

void InitTexUniforms(TexUniforms &us, GLuint shader, const char *sampler_name)
{
  char buf[128];
  #define TEX_UNIFORM(Name, Type, Count) { \
  int len = snprintf(buf, sizeof(buf), "%s_%s", sampler_name, #Name); \
  if (len < (int)sizeof(buf)) { \
  us.Name.loc = glGetUniformLocation(shader, buf); \
  us.Name.type = Uniform::Type; \
  us.Name.count = Count; } \
  else LOG_WARNING("Uniform name too long: %s_%s.", sampler_name, #Name); }

  TEX_UNIFORM(corners, VEC2, 2);
  TEX_UNIFORM(pixel_size, VEC2, 1);

  us.corners.value.v2[0] = glm::vec2(0.0f);
  us.corners.value.v2[1] = glm::vec2(1.0f);

  #undef TEX_UNIFORM
}

// Textures

GLuint CreateTexture2D(unsigned int w, unsigned int h, GLenum fmt,
GLenum type, const void *data)
{
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  GLint internal_fmt = fmt;
  if (type == GL_FLOAT && fmt == GL_RED)
  internal_fmt = GL_R32F;

  glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, w, h, 0, fmt, type, data);
  // GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST
  // GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return texture;
}

void DeleteTexture(GLuint texture)
{
  glDeleteTextures(1, &texture);
}


// Draw

inline int getQuadIndexTypeSize(GLenum type)
{
  switch (type)
  {
  case GL_UNSIGNED_INT: return sizeof(uint32_t);
  default: assert(0 && "Invalid index type!");
  }
  return 0;
}

void Draw(DrawItem &d)
{
  #ifdef TESS_PROGRAM
    glPatchParameteri(GL_PATCH_VERTICES,3);
  #endif // TESS_PROGRAM
  glUseProgram(d.shader);
  glBindVertexArray(d.vertex_array);

  for (GLuint i = 0; i < d.texture_count; i++)
  {
    Texture &tex = d.textures[i];
    glActiveTexture(GL_TEXTURE0 + tex.bind_index);
    glBindTexture(GL_TEXTURE_2D, tex.texture);
  }

  for (GLuint i = 0; i < d.uniform_count; i++)
  {
    Uniform &u = d.uniforms[i];
    if (u.loc < 0) continue;
    switch (u.type)
    {
      case Uniform::FLOAT: glUniform1fv(u.loc, u.count, u.value.f); break;
      case Uniform::VEC2:  glUniform2fv(u.loc, u.count, u.value.f); break;
      case Uniform::VEC3:  glUniform3fv(u.loc, u.count, u.value.f); break;
      case Uniform::MAT4:
      glUniformMatrix4fv(u.loc, u.count, GL_FALSE, u.value.f); break;
      default: assert(0 && "Invalid uniform type!");
    }
  }

  if (d.draw_arrays)
  {
    glDrawArrays(d.primitive_mode, d.first, d.count);
  }
  else
  {
    void *offset = (char*)0 + d.first * getQuadIndexTypeSize(d.index_type);
    glDrawElements(d.primitive_mode, d.count, d.index_type, offset);
  }
}
