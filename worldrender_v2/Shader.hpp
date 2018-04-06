#ifndef SHADER_HPP_INCLUDED
#define SHADER_HPP_INCLUDED

#include "Utils.hpp"
#include "string"

#include <glad/glad.h>

#define GLSL(vs, fs) \
"#if defined(VERTEX_SHADER)\n" #vs "\n#else\n" #fs "\n#endif\n"


#define MAX_VERTEX_ATTRIBUTES 16

struct VertexFormat
{
    struct Attribute
    {
        GLuint size;
        GLenum type;
        bool normalize;
        void *pointer;
    };

    GLuint vertex_size;
    Attribute attributes[MAX_VERTEX_ATTRIBUTES];
};

struct Texture
{
    GLuint texture;
    GLuint bind_index;
};

struct Uniform
{
    enum Type
    {
        FLOAT = 1,
        VEC2  = 2,
        VEC3  = 3,
        MAT4  = 16
    };

    GLint loc;
    Type type;
    GLuint count;
    union
    {
        float f[16];
        glm::vec2  v2[8];
        glm::vec3  v3[5];
        //Vec4  v4[4];
        glm::mat4  m4;
    } value;
};

struct TexUniforms
{
    Uniform corners;
    Uniform pixel_size;
};

struct DrawItem
{
    GLuint shader;
    GLuint vertex_array;
    Texture *textures;
    Uniform *uniforms;
    GLuint texture_count;
    GLuint uniform_count;
    GLenum primitive_mode;
    GLenum index_type;
    GLuint first;
    GLuint count;
    bool draw_arrays;
};

inline int GetUniformCountFromSize(unsigned int size)
{
    return size / sizeof(Uniform);
}

// Geometry

GLuint CreateVertexBuffer(unsigned int size, const void *data);
GLuint CreateIndexBuffer(unsigned int size, const void *data);
inline int GetAttributeTypeSize(GLenum type);
void AddVertexAttribute(VertexFormat &format, int index, int size,
                        GLenum type, bool normalize);
GLuint CreateVertexArray(GLuint vertex_buffer, VertexFormat &format,
                         GLuint index_buffer);
GLuint CompileShader(GLenum type, const char *source);
GLuint CreateShaderFromSource(std::string source);
void InitUniform(Uniform &u, GLuint shader, const char *name, Uniform::Type type, int count=1);
void InitTexUniforms(TexUniforms &us, GLuint shader, const char *sampler_name);
GLuint CreateTexture2D(unsigned int w, unsigned int h, GLenum fmt,
                       GLenum type, const void *data);
void DeleteTexture(GLuint texture);
void Draw(DrawItem &d);

std::string LoadShaderFromFile(std::string path);

#endif // SHADER_HPP_INCLUDED
