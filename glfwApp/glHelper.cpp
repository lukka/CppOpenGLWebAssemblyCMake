#include "glHelper.h"

bool glHelper::_check_gl_error(const char *file, int line)
{
    bool noErrors = false;
    GLenum err(glGetError());
    noErrors = (err == GL_NO_ERROR);
    std::string error;
    while (err != GL_NO_ERROR)
    {
        switch (err)
        {
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }

        std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line
                  << "\n";
        err = glGetError();
    }
    return noErrors;
}

/// len: the length of the shader source null terminated string.
GLint glHelper::createShader(GLint shaderType, GLchar const *src, GLint len)
{
    GLint shader = glCreateShader(shaderType);
    GLchar const *srcArray[] = {src};
    // Remove the null termination from the length.
    GLint lenArray[] = {len - 1};
    glShaderSource(shader, 1, srcArray, lenArray);
    glCompileShader(shader);
    // Check Shader
    glHelper::checkShader(shader);
    return shader;
}

void glHelper::checkProgram(GLint programShader)
{
    GLint result;
    int infoLogLen;
    glGetProgramiv(programShader, GL_LINK_STATUS, &result);
    CHECK_OGL;
    glGetProgramiv(programShader, GL_INFO_LOG_LENGTH, &infoLogLen);
    CHECK_OGL;
    if (infoLogLen)
    {
        std::vector<char> ProgramErrorMessage(infoLogLen + 1);
        glGetProgramInfoLog(programShader, infoLogLen, NULL,
                            &ProgramErrorMessage[0]);
        std::cout << &ProgramErrorMessage[0] << "\n";
    }
    assert(result == GL_TRUE);
}

void glHelper::prependShaderVersion(std::string &source)
{
#if defined(EMSCRIPTEN)
    source.insert(0, _OGL300ES);
#else
    source.insert(0, _OGL330CORE);
#endif
}

void glHelper::checkShader(GLint shader)
{
    GLint result;
    int infoLogLen;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    CHECK_OGL;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
    CHECK_OGL;
    if (infoLogLen)
    {
        std::vector<char> ProgramErrorMessage(infoLogLen + 1);
        glGetShaderInfoLog(shader, infoLogLen, NULL, &ProgramErrorMessage[0]);
        std::cout << &ProgramErrorMessage[0] << "\n";
    }
    assert(result == GL_TRUE);
}

const glm::vec3 glHelper::ZERO = glm::vec3(0, 0, 0);
