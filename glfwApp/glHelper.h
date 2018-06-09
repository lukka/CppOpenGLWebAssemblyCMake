#pragma once
#include <cassert>
#include <iostream>
#include <vector>
#if !defined(EMSCRIPTEN)
#include <glad/glad.h>
#else
#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#endif
#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif
#include <glm/glm.hpp>

// OpenGL helper functions.
struct glHelper
{
    static bool _check_gl_error(const char *file, int line);

#if defined(DEBUG)
#define CHECK_OGL                                           \
    do                                                      \
    {                                                       \
        if (!glHelper::_check_gl_error(__FILE__, __LINE__)) \
            ;                                               \
    } while (false)
#else
#define CHECK_OGL
#endif

    /// len: the length of the shader source null terminated string.
    static GLint createShader(GLint shaderType, GLchar const *src, GLint len);

    static void checkProgram(GLint programShader);

    static void prependShaderVersion(std::string &source);

    static void checkShader(GLint shader);

    static const glm::vec3 ZERO;

   private:
    static GLchar constexpr _OGL330CORE[] = "#version 330 core\r\n";
    static GLchar constexpr _OGL300ES[] = "#version 300 es\r\n";
};
