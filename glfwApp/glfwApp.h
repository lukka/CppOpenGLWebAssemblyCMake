#pragma once

#if defined(EMSCRIPTEN)
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#if !defined(EMSCRIPTEN)
#include <glad/glad.h>
#else
#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#endif
#include <GLFW/glfw3.h>
#include "galaga/Frame.h"
#include "galaga/GalagaMachine.h"
#include "glfwApp/AnimationAlongPath.h"
#include "glfwApp/Audio.h"
#include "glfwApp/glHelper.h"
#include "glfwApp/glObjects.h"
#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>

struct glfwApp
{
    glfwApp();
    ~glfwApp();

    static glfwApp* get_app();
    static void step();

    // Acquire resources
    bool initialize();

    // Release resources
    bool finalize();

    // Process and dispatch messages
    void runMessageLoop();

    // Get the underlying GalagaMachine
    Galaga::GalagaMachine* getGalagaMachine() { return &_galagaMachine; }

    void step(double ms);
    void runStep();

    void handleAcceleration(float interval, float ax, float ay, float az);
    void handleRotation(float intervalTime, float alpha, float beta,
                        float gamma);
    void enterFullscreen();

    void updateWindowSize(int w, int h);
    void setWindowSize(int w, int h);

   private:
    // Initialize device-dependent resources.
    bool initGL();

    // Render graphics.
    bool onRender(float deltaTime);

    // Play sound.
    bool onPlay(float deltaTime);

    // Resize the render target.
    void onResize(int32_t width, int32_t height);

    void handleKeys();
    static void keysCallback(GLFWwindow* window, int key, int scancode,
                             int action, int mods);

    GLFWwindow* _window;
    glm::ivec2 _wndSize;

    Galaga::Frame _graphicsFrame;
    Galaga::GalagaMachine _galagaMachine;
    Galaga::AudioFrame _audioFrame;
    std::unique_ptr<Audio> _audioApp;

    GLuint _texture;

    GLuint _vbo[3];  // position and texcoords.
    GLuint _vao;
    GLuint _indices;
    GLuint _texcoords;
    GLuint _vertices;

    GLfloat _vertexArray[12] = {-.78f, -1., 0., -.78f, 1., 0.,
                                .78f,  -1., 0., .78f,  1., 0.};
    GLint _indexArray[4] = {0, 1, 2, 3};
    GLfloat _texcoordsArray[8] = {1., 1., 0., 1., 1, 0., 0, 0};
    GLuint _screenVertexShader;
    GLuint _screenFragmentShader;
    GLuint _screenProgramShader;
    glm::vec2 _appScreenPixelSize =
        glm::vec2{Galaga::ScreenDevice::Width, Galaga::ScreenDevice::Height};
    GLint _screenWorldLocation;
    GLint _viewProjectionLocation;
    GLint _screenTextureLocation;

    // The display is the quad where the game is rendered into.
    // Model quaternion for the display.
    glm::quat _displayQuat = glm::angleAxis(0.f, glm::vec3(0, 0, 1));

    // Quaternion for the camera view.
    glm::quat _cameraQuat = glm::angleAxis(0.f, glm::vec3(0, 0, -1));
    glm::vec3 _lastAcc;
    glm::vec3 _relativeMovement;
    glm::vec3 _lastRelativeMovement;
    glm::vec3 _lastRelAcc;
    float _oldAccRad = 0;
    float _radSecSpeedRot =
        0;  /// The degree per seconds speed of rotation of display.
    static const glm::vec3 globalUpAxis;

    // Whether autofire is enabled. It is enabled when touch callback setup
    // succeeded.
    bool autofireEnabled = false;

    double totalSimulatedTime = 0;
    double totalMachineTime = 0;
    double totalRenderingTime = 0;
    double totalPlayTime = 0;
    double totalStepTime = 0;
    double totalTime = 0;
    double frameTime =
        0;  // Time for drawing one frame, with a low pass filter.

    // The objects displayed on top of canvas.
    std::vector<AnimatedObject> animObjects;
    // Main entry animtaion.
    AnimationAlongPath _animation;

    // Unique glfwApp instance.
    static glfwApp _app;
};

//
// Dumb printf style debugging.
//
static std::map<std::string, float> oldValues;
void printIfChanged(std::string const& func, std::string const& key,
                    float value)
{
    std::string realkey = func + key;
    if (oldValues.find(realkey) != oldValues.end())
    {
        if (oldValues[realkey] != value)
        {
            std::cout << realkey << "=" << value << "\n";
            oldValues[realkey] = value;
        }
    }
    else
    {
        oldValues[realkey] = value;
        std::cout << realkey << "=" << value << "\n";
    }
}
#define PRINTIFCHANGED(k, v)            \
    do                                  \
    {                                   \
        printIfChanged(__func__, k, v); \
    } while (false)
