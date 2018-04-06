#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib> // malloc, free
#include <cassert>
#include <glm/glm.hpp>
#include <glad/glad.h>

//LOGGING
#define LOG_WARNING(fmt, ...) fprintf(stdout, "[WARNING] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

//TIMING
inline uint64_t GetMicroTicks()
{
    static Uint64 freq = SDL_GetPerformanceFrequency();
    return SDL_GetPerformanceCounter()*1000000ull / freq;
}

static bool print_timings = false;
static uint64_t frame_start_time = 0;

#define PRINT_TIMING(timer_name, timer_var) if (print_timings) printf("%-20s %10u us\n", timer_name, uint32_t(GetMicroTicks() - timer_var))

struct ScopeTimer
{
    const char *name;
    uint64_t start_time;
    ScopeTimer(const char *n) : name(n), start_time(GetMicroTicks()) {}
    ~ScopeTimer() { PRINT_TIMING(name, start_time); }
};

#define START_TIMING() print_timings = true
#define STOP_TIMING() print_timings = false
#define TOGGLE_TIMING() print_timings = !print_timings
#define START_FRAME() PRINT_TIMING("FRAME TIME", frame_start_time); printf("\n"); frame_start_time = GetMicroTicks()
#define BEGIN_TIMED_BLOCK(id) uint64_t _timed_block_##id = GetMicroTicks()
#define END_TIMED_BLOCK(id) PRINT_TIMING(#id, _timed_block_##id)
#define TIMED_SCOPE(id) ScopeTimer _timed_scope_##id(__FUNCTION__ " " #id)
#define TIMED_FUNCTION() ScopeTimer JOIN(_timed_function_, __LINE__)(__FUNCTION__)

//PP
#define JOIN__(a, b) a##b
#define JOIN_(a, b) JOIN__(a, b)
#define JOIN(a, b) JOIN_(a, b)

#define NUM_VA_ARGS_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, N, ...) N
#define NUM_VA_ARGS(...) NUM_VA_ARGS_(__VA_ARGS__, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define DEFER1(code, line) struct _D##line { ~_D##line() { code; } } _d##line
#define DEFER3(code, line, T, a) struct _D##line { T a; _D##line(T a_):a(a_){} ~_D##line() { code; } } _d##line(a)
#define DEFER_(code, line, ...) JOIN(DEFER, NUM_VA_ARGS(1, ##__VA_ARGS__))(code, line, ##__VA_ARGS__)
//#define DEFER(code, ...) DEFER_(printf(#code"\n"); code, __LINE__, ##__VA_ARGS__)
#define DEFER(code, ...) DEFER_(code, __LINE__, ##__VA_ARGS__)

//MATH
#define PI 3.1415926535897932384626433832795

#define Abs(x) std::abs(x)
#define Min(x, y) std::min(x, y)
#define Max(x, y) std::max(x, y)
#define Sqrt(x) std::sqrt(x)
#define Sin(rad) std::sin(rad)
#define Cos(rad) std::cos(rad)
#define Tan(rad) std::tan(rad)
#define SinCos(rad, si, co) sincosf(rad, si, co)
#define ACos(x) std::acos(x)
#define DegToRad(deg) ((deg) * 0.01745329251994329576923690768489)
#define Log2(x) std::log2(x)
#define sqrLength(v) glm::dot(v,v)
template<class T> inline T Square(T x) { return x*x; }
inline void frame(){frame_start_time=0;}
// Vec2

inline glm::mat3 mat3FromBasisVectors(glm::vec3 x, glm::vec3 y, glm::vec3 z)
{
    glm::mat3 result;
    result[0][0] = x.x; result[0][1] = x.y; result[0][2] = x.z;
    result[1][0] = y.x; result[1][1] = y.y; result[1][2] = y.z;
    result[2][0] = z.x; result[2][1] = z.y; result[2][2] = z.z;
    return result;
}

inline glm::mat3 mat3RotationX(float rad)
{
    float si, co;
    SinCos(rad, &si, &co);
    glm::mat3 result = {};
    result[0][0] = 1.0f;
    result[1][1] = co;
    result[1][2] = si;
    result[2][1] = -si;
    result[2][2] = co;
    return result;
}

inline glm::mat3 mat3RotationY(float rad)
{
    float si, co;
    SinCos(rad, &si, &co);
    glm::mat3 result = {};
    result[0][0] = co;
    result[0][2] = -si;
    result[1][1] = 1.0f;
    result[2][0] = si;
    result[2][2] = co;
    return result;
}

inline glm::mat3 mat3RotationZ(float rad)
{
    float si, co;
    SinCos(rad, &si, &co);
    glm::mat3 result = {};
    result[0][0] = co;
    result[0][1] = si;
    result[1][0] = -si;
    result[1][1] = co;
    result[2][2] = 1.0f;
    return result;
}

//LIST
template<class T>
struct List
{
    T *data;
    GLuint num;
    GLuint cap;
};

template<class T>
void ListReserve(List<T> &list, GLuint n)
{
    assert(n > 0);
    if (list.cap < n)
    {
        T *new_data = (T*)malloc(n * sizeof(T));
        assert(new_data != nullptr);
        assert(((uintptr_t)new_data & (alignof(T) - 1)) == 0);

        if (list.data)
        {
            if (list.num > 0)
            {
                memcpy(new_data, list.data, list.num * sizeof(T));
            }
            free(list.data);
        }

        list.data = new_data;
        list.cap = n;
    }
}

template<class T>
void ListResize(List<T> &list, GLuint n)
{
    assert(n >= 0);
    if (list.cap < n)
    {
        ListReserve(list, n);
    }
    list.num = n;
}

template<class T>
void ListAdd(List<T> &list, T value)
{
    if (list.num == list.cap)
    {
        GLuint n = Max(list.cap * 2, (GLuint)96);
        ListReserve(list, n);
    }

    list.data[list.num++] = value;
}

template<class T>
void ListFree(List<T> &list)
{
    if (list.data)
    {
        free(list.data),
        list.data = nullptr;
        list.num = 0;
        list.cap = 0;
    }
}

inline float Q_rsqrt(float number)
{
  union
  {
    float f;
    long i;
  } conv;

  float x2;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  conv.f  = number;
  conv.i  = 0x5f3759df - ( conv.i >> 1 );	// what the fuck?
  conv.f  = conv.f * ( threehalfs - ( x2 * conv.f * conv.f ) );
  return conv.f;
}
#endif // MATH_HPP_INCLUDED
