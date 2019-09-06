#include "glfwApp.h"

#include <galaga/Frame.h>
#include <galaga/ScreenDevice.h>
#include <array>
#include <chrono>
#include <iostream>
#include <map>
#include <thread>
#if defined(EMSCRIPTEN)
#include <emscripten.h>
#include <emscripten/bind.h>
#endif
#include "GameStateTracker.h"
#include "galaga/GalagaMachine.h"
#include "glObjects.h"

#if defined(EMSCRIPTEN)
// Binding code
EMSCRIPTEN_BINDINGS(galaga_machine)
{
    emscripten::class_<Galaga::GalagaMachine>("GalagaMachine")
        .function("set_InsertCoin", &Galaga::GalagaMachine::set_InsertCoin)
        .function("set_Start1Player", &Galaga::GalagaMachine::set_Start1Player)
        .function("set_Start2Player", &Galaga::GalagaMachine::set_Start2Player)
        .function("set_MoveLeft", &Galaga::GalagaMachine::set_MoveLeft)
        .function("set_MoveRight", &Galaga::GalagaMachine::set_MoveRight)
        .function("set_Button1", &Galaga::GalagaMachine::set_Button1);

    emscripten::class_<glfwApp>("glfwApp")
        .constructor<>()
        .function("initialize", &glfwApp::initialize)
        .function("runStep", &glfwApp::runStep)
        .function("handleAcceleration", &glfwApp::handleAcceleration)
        .function("handleRotation", &glfwApp::handleRotation)
        .function("getGalagaMachine", &glfwApp::getGalagaMachine,
                  emscripten::allow_raw_pointers())
        .function("enterFullscreen", &glfwApp::enterFullscreen);
    function("get_app", &glfwApp::get_app, emscripten::allow_raw_pointers());
}
#endif

glfwApp glfwApp::_app;
glfwApp *glfwApp::get_app() { return &_app; };
void glfwApp::step() { glfwApp::get_app()->runStep(); }

const glm::vec3 glfwApp::globalUpAxis =
    glm::vec3(0, 1, 0);  // Global globalUpAxis axis.

void error_callback(int error, const char *description)
{
    std::cout << "glfw error: " << description << "\n";
}

#if defined(EMSCRIPTEN)

static int canvasWidth;
static int canvasHeight;

EM_BOOL updateFramebufferSize(int eventType, const EmscriptenUiEvent *uiEvent,
                              void *userData)
{
    double width(0), height(0);
    emscripten_get_element_css_size("GLCanvas", &width, &height);
    emscripten_set_canvas_element_size("GLCanvas", int(width), int(height));
    PRINTIFCHANGED("width", width);
    return EM_TRUE;
}

EM_BOOL updateCanvasResize(int eventType, const void *reserved, void *userData)
{
    double width, height;
    emscripten_get_element_css_size("GLCanvas", &width, &height);
    PRINTIFCHANGED("width", width);
    canvasWidth = width;
    canvasHeight = height;

    double pixelRatio = emscripten_get_device_pixel_ratio();
    glfwApp::get_app()->setWindowSize(width * pixelRatio, height * pixelRatio);
    return EM_TRUE;
}

EM_BOOL updateFullscreenStatus(
    int eventType, const EmscriptenFullscreenChangeEvent *fullscreenChangeEvent,
    void *userData)
{
    std::cout << __FUNCTION__ << ":" << fullscreenChangeEvent->fullscreenEnabled
              << "\n";
    return EM_TRUE;
}

static EmscriptenTouchPoint firstTouch;
static EmscriptenTouchPoint currentTouch;
static size_t movementMemory = 0;
EM_BOOL touchStartCb(int eventType, const EmscriptenTouchEvent *touchEvent,
                     void *userData)
{
    Galaga::GalagaMachine *gM = glfwApp::get_app()->getGalagaMachine();
    if (touchEvent->numTouches > 0)
    {
        if (GameStateTracker::gameState == GameStateTracker::INTRO)
        {
            // Start the game with a tap.
            gM->set_Start1Player(true);
        }
        else if (GameStateTracker::gameState == GameStateTracker::HIGHSCORE &&
                 touchEvent->numTouches > 1)  // Double tap?
        {
            // Press button in highscore mode only with double tap.
            gM->set_Button1(true);
        }
        else  // any state is ok.
        {
            firstTouch = touchEvent->touches[0];  // Store a copy.
            gM->set_MoveLeft(firstTouch.canvasX < (canvasWidth / 2.));
            gM->set_MoveRight(firstTouch.canvasX > (canvasWidth / 2.));
        }
    }
    return true;  // Consume the event, i.e. preventDefault();
}

EM_BOOL touchMoveCb(int eventType, const EmscriptenTouchEvent *touchEvent,
                    void *userData)
{
    Galaga::GalagaMachine *gM = glfwApp::get_app()->getGalagaMachine();
    if (touchEvent->numTouches > 0)
    {
        if (GameStateTracker::gameState == GameStateTracker::HIGHSCORE &&
            touchEvent->numTouches > 1)
        {
            // Hold donw the button in highscore mode.
            gM->set_Button1(true);
        }
        else  // any state is ok.
        {
            firstTouch = touchEvent->touches[0];  // Store a copy.
            gM->set_MoveLeft(firstTouch.canvasX < (canvasWidth / 2.));
            gM->set_MoveRight(firstTouch.canvasX > (canvasWidth / 2.));
        }
    }
    return true;
}

EM_BOOL touchEndCb(int eventType, const EmscriptenTouchEvent *touchEvent,
                   void *userData)
{
    currentTouch = firstTouch;
    Galaga::GalagaMachine *gM = glfwApp::get_app()->getGalagaMachine();

    // Unpress button in highscore mode.
    if ((GameStateTracker::gameState == GameStateTracker::HIGHSCORE) &&
        (touchEvent->numTouches > 1))  // Double tap?
    {
        gM->set_Button1(false);
    }
    else  // any state is ok.
    {
        gM->set_MoveLeft(false);
        gM->set_MoveRight(false);
    }

    return true;
}

EM_BOOL touchCancelCb(int eventType, const EmscriptenTouchEvent *touchEvent,
                      void *userData)
{
    Galaga::GalagaMachine *gM = glfwApp::get_app()->getGalagaMachine();
    gM->set_MoveLeft(false);
    gM->set_MoveRight(false);
    std::cout << "touchCanceled:" << eventType << "\n";
    return true;
}

#endif

glfwApp::glfwApp()
    : _graphicsFrame(Galaga::ScreenDevice::Width, Galaga::ScreenDevice::Height),
      _window(nullptr),
      _galagaMachine(),
      _audioApp(std::make_unique<Audio>()),
      _lastAcc(glHelper::ZERO),
      _relativeMovement(glHelper::ZERO),
      _lastRelativeMovement(glHelper::ZERO),
      _lastRelAcc(glHelper::ZERO)
{
}

glfwApp::~glfwApp()
{
    // Intentionally void.
}

void glfwApp::step(double sec)
{
    static double oldT = 0;
    static double stopT[4] = {};

    stopT[0] = glfwGetTime();
    // Spin the galaga machine
    _galagaMachine.Run(sec * 1e6 /* convert sec to usec */, _graphicsFrame,
                       _audioFrame);
    stopT[1] = glfwGetTime();
    // Render graphics.
    onRender(sec);
    stopT[2] = glfwGetTime();
    // Play sound.
    onPlay(sec);
    stopT[3] = glfwGetTime();

    totalSimulatedTime += sec;
    totalRenderingTime += stopT[2] - stopT[1];
    totalMachineTime += stopT[1] - stopT[0];
    totalPlayTime += stopT[3] - stopT[2];
    totalStepTime += stopT[3] - stopT[0];
    totalTime += stopT[3] - oldT;
    frameTime = frameTime + ((stopT[3] - stopT[0]) - frameTime) * 0.01;

    oldT = stopT[0];

    this->handleKeys();
}

void glfwApp::runStep()
{
    static double oldT = 0;
    double currT = glfwGetTime();

    double deltaTimeSec = oldT ? (currT - oldT) : 0.016;

    this->step(deltaTimeSec);

    glfwSwapBuffers(_window);

#if defined(EMSCRIPTEN)
    // No need to sleep with emscripten, the run_main_loop() do that for us.
#else

#if defined(DEBUG)
    /// double glfwWaitTime = glfwGetTime();
#endif
    deltaTimeSec = std::min(deltaTimeSec, 0.016);
    glfwWaitEventsTimeout(deltaTimeSec * .75);
#if defined(DEBUG)
    // std::cout << "glfwWaitTime=" << glfwGetTime() - glfwWaitTime << "\n";
#endif

#endif

    oldT = currT;
}

void glfwApp::runMessageLoop()
{
#if defined(EMSCRIPTEN)
    enterFullscreen();
    emscripten_set_main_loop(glfwApp::step, 0 /* fps */,
                             1 /* simulate_infinite_loop */);
#else
    while (!glfwWindowShouldClose(_window))
    {
        runStep();
    }
#endif
}

void glfwApp::handleRotation(float intervalTime, float alpha, float beta,
                             float gamma)
{
    float accRad = 0;
    float currRot = _oldAccRad + ((beta - _oldAccRad) * .2f);

    if (abs(currRot) > .7)
    {
        float radSecSpeedRot = (currRot > 0 ? 0.01f : -0.01f);
        _radSecSpeedRot =
            _radSecSpeedRot + ((radSecSpeedRot - _radSecSpeedRot) * .3);
    }
    _oldAccRad = currRot;
}

void glfwApp::handleAcceleration(float intervalTime, float ax, float ay,
                                 float az)
{
    glm::vec3 acc(ax, ay, az);
    if (_lastAcc == glHelper::ZERO) _lastAcc = acc;
    // Low pass filter on acceleration vector.
    glm::vec3 currAcc = _lastAcc + ((acc - _lastAcc) * .5f);
    float angleRad = acos(glm::clamp(
        glm::dot(glm::normalize(_lastAcc), glm::normalize(acc)), -1.f, 1.f));

    float speedRadSec = 0;
    if (intervalTime > 0) speedRadSec = angleRad / intervalTime;
    if (speedRadSec > 0.004)
    {
        glm::vec3 relAcc = currAcc - _lastAcc;
        if (_relativeMovement == glHelper::ZERO)
        {
            _lastRelAcc = currAcc;
            _relativeMovement = relAcc;
        }
        else
        {
            _relativeMovement += relAcc;
            float relMovLen = glm::length(_relativeMovement);
            if (relMovLen > 0.05)
            {
                auto currDot = glm::dot(glm::vec3(1, 0, 0), currAcc);
                auto lastDot = glm::dot(glm::vec3(1, 0, 0), _lastRelAcc);
                speedRadSec = currDot > lastDot ? 0.02 : -0.02;
                _radSecSpeedRot =
                    _radSecSpeedRot + ((speedRadSec - _radSecSpeedRot) * .1);
            }
        }
    }
    else
    {
        // Clear it out.
        _relativeMovement = glHelper::ZERO;
        _lastRelativeMovement = glHelper::ZERO;
    }
    _lastAcc = currAcc;
}

static void wndResizeCb(GLFWwindow *wnd, int w, int h)
{
    glfwApp::get_app()->updateWindowSize(w, h);
    PRINTIFCHANGED("wndResizeCb", w);
}

void glfwApp::updateWindowSize(int w, int h)
{
    _wndSize = glm::vec2(w, h);
#if defined(DEBUG)
    PRINTIFCHANGED("_wndSize.x", _wndSize.x);
#endif
}

void glfwApp::setWindowSize(int w, int h)
{
    PRINTIFCHANGED("SetWindowSizeWidth", w);
    glfwSetWindowSize(_window, w, h);
}

bool glfwApp::initialize()
{
    bool ret = true;

    // Initialize the glfw library
    glfwSetErrorCallback(error_callback);
    ret = glfwInit();
    if (ret)
    {
        // Output glfw version.
        std::cout << glfwGetVersionString() << "\n";

        // Create main window.
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, 1);
        glfwWindowHint(GLFW_MAXIMIZED, 1);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
        _window = glfwCreateWindow(Galaga::ScreenDevice::Width,
                                   Galaga::ScreenDevice::Height,
                                   "app [glfw]", 0, 0);

        glfwSetWindowSizeCallback(_window, &wndResizeCb);
        glfwGetWindowSize(_window, &_wndSize.x, &_wndSize.y);
        //
        // Init OpenGL
        glfwMakeContextCurrent(_window);

#if defined(EMSCRIPTEN)
        EMSCRIPTEN_RESULT res = emscripten_set_touchstart_callback(
            "#canvas", 0 /*void *userData*/, true /* useCapture */,
            touchStartCb);
        res |= emscripten_set_touchmove_callback(
            "#canvas", 0 /*void *userData*/, true /* useCapture */,
            touchMoveCb);
        res |= emscripten_set_touchend_callback(
            "#canvas", 0 /*void *userData*/, true /* useCapture */, touchEndCb);
        res |= emscripten_set_touchcancel_callback(
            "#canvas", 0 /*void *userData*/, true /* useCapture */,
            touchCancelCb);
        if (res == EMSCRIPTEN_RESULT_SUCCESS)
            autofireEnabled = true;  // Autofire needed.
#endif
        glfwSetKeyCallback(_window, glfwApp::keysCallback);
        glfwSetInputMode(_window, GLFW_STICKY_KEYS, GLFW_TRUE);
        ret = initGL();
    }

#if defined(EMSCRIPTEN)
    // Init resize callback.
    double width, height;
    emscripten_get_element_css_size("GLCanvas", &width, &height);
    canvasWidth = width;
    canvasHeight = height;
    emscripten_set_canvas_element_size("GLCanvas", int(width), int(height));
    emscripten_set_resize_callback(nullptr, nullptr, false,
                                   &updateFramebufferSize);
    emscripten_set_fullscreenchange_callback(nullptr, nullptr, false,
                                             &updateFullscreenStatus);
#endif

    // Set the callback to keep track the game state based upon the program
    // counter register.
    std::function<void(uint16_t)> programCounterListener(
        GameStateTracker::identifyGameState);
    _galagaMachine.SetProgramCounterListener(programCounterListener);

    return ret;
}

bool glfwApp::finalize()
{
    glfwTerminate();
    return true;
}

GLchar const _vertexShaderSrc[] =
    R"(layout(location = 1) in vec3 vertexPosition_modelspace;
layout(location = 0) in vec2 vertexUV;
uniform mat4 viewProjection;
uniform mat4 world;
out vec2 uv;
void main()
{
    gl_Position = viewProjection * world * vec4(vertexPosition_modelspace, 1.f);
    uv = vertexUV;
}
)";

GLchar const _fragmentShaderSrc[] =
    R"(precision mediump float;
in vec2 uv;
uniform sampler2D appTexture;
out vec4 color;
void main()
{
    color = texture(appTexture, uv).rgba;
}
)";

bool glfwApp::initGL()
{
    bool ret = true;

    glfwMakeContextCurrent(_window);

#if !defined(EMSCRIPTEN)
    ret = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
#endif

    if (ret)
    {
        glViewport(0, 0, _wndSize.x, _wndSize.y);

        // VBO & VAO
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glGenBuffers(1, &_vertices);
        glGenBuffers(1, &_texcoords);
        glGenBuffers(1, &_indices);

        glBindBuffer(GL_ARRAY_BUFFER, _vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_vertexArray), _vertexArray,
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _texcoords);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_texcoordsArray), _texcoordsArray,
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indexArray), _indexArray,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(2, 1, GL_UNSIGNED_INT, false, 0, 0);
        glEnableVertexAttribArray(2);

        std::string vertexShaderSrcStr(_vertexShaderSrc);
        glHelper::prependShaderVersion(vertexShaderSrcStr);
        std::string fragmentShaderSrcStr(_fragmentShaderSrc);
        glHelper::prependShaderVersion(fragmentShaderSrcStr);

        // Screen vertex shader
        _screenVertexShader =
            glHelper::createShader(GL_VERTEX_SHADER, vertexShaderSrcStr.c_str(),
                                   vertexShaderSrcStr.size());
        // Screen fragment shader
        _screenFragmentShader = glHelper::createShader(
            GL_FRAGMENT_SHADER, fragmentShaderSrcStr.c_str(),
            fragmentShaderSrcStr.size());

        // Screen program shader
        _screenProgramShader = glCreateProgram();
        glAttachShader(_screenProgramShader, _screenFragmentShader);
        glAttachShader(_screenProgramShader, _screenVertexShader);
        glLinkProgram(_screenProgramShader);
        glHelper::checkProgram(_screenProgramShader);

        // Screen texture
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#if defined(EMPSCRIPTEN)
        // This is available in ES 3.0 and WebGL2, but not on OGL 4.0
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _appScreenPixelSize[0],
                       _appScreenPixelSize[1]);
#else
        // In OGL 4.0 glTexStorage is not avail.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Galaga::ScreenDevice::Width,
                     Galaga::ScreenDevice::Height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     0);
#endif

        // Shader input locations
        _screenTextureLocation =
            glGetUniformLocation(_screenProgramShader, "appTexture");
        _screenWorldLocation =
            glGetUniformLocation(_screenProgramShader, "world");
        _viewProjectionLocation =
            glGetUniformLocation(_screenProgramShader, "viewProjection");
        CHECK_OGL;

        // Create the animated objects.
        constexpr float pngFontRatio = 18.f / 20.f;
        animObjects.resize(6);
        animObjects[0].create("./data/font");
        animObjects[0].setPosition(glm::vec3(-5, -.9, 0));
        animObjects[0].setTarget(glm::vec3(-.42, -.95, 0));
        animObjects[0].setSpeed(glm::vec3(0, .4, 0));
        animObjects[0].setZoom((1. / 288.f) * pngFontRatio);
        animObjects[0].setDrawBackground(false);

        animObjects[1].create("./data/font");
        animObjects[1].setPosition(glm::vec3(5, -.9, 0));
        animObjects[1].setTarget(glm::vec3(.5, -.95, 0));
        animObjects[1].setSpeed(glm::vec3(0, .4, 0));
        animObjects[1].setZoom((1. / 288.f) * pngFontRatio);
        animObjects[1].setDrawBackground(false);

        animObjects[2].create("./data/font");
        animObjects[2].setPosition(glm::vec3(0, -5, 0));
        animObjects[2].setTarget(glm::vec3(0, 1, 0));
        animObjects[2].setSpeed(glm::vec3(0, .5, 0));
        animObjects[2].setBehavior(AnimatedObject::RANDOM);
        animObjects[2].setZoom((1. / 288.f) * pngFontRatio);
        animObjects[2].setBoundingBox(glm::vec3(-.1, .8, 0),
                                      glm::vec3(.1, .9, 0));

        animObjects[4].create("./data/font");
        animObjects[4].setPosition(glm::vec3(-4, .8, 0));
        animObjects[4].setTarget(glm::vec3(0, .8, 0));
        animObjects[4].setSpeed(glm::vec3(0, .5, 0));
        animObjects[4].setBehavior(AnimatedObject::RANDOM);
        animObjects[4].setBoundingBox(glm::vec3(-.6, .8, 0),
                                      glm::vec3(-.55, .9, 0));
        animObjects[4].setZoom((1. / 288.f) * pngFontRatio);
        animObjects[4].setText("   Tap   left\n  halfscreen\n to move left");

        animObjects[5].create("./data/font");
        animObjects[5].setPosition(glm::vec3(4, .8, 0));
        animObjects[5].setTarget(glm::vec3(0, .8, 0));
        animObjects[5].setSpeed(glm::vec3(0, .5, 0));
        animObjects[5].setBehavior(AnimatedObject::RANDOM);
        animObjects[5].setBoundingBox(glm::vec3(.55, .8, 0),
                                      glm::vec3(.6, .9, 0));
        animObjects[5].setZoom((1. / 288.f) * pngFontRatio);
        animObjects[5].setText("   Tap  right\n  halfscreen\n to move right");

        ret = (glGetError() == GL_NO_ERROR);
    }
    return ret;
}

// Note Height and Width are swapped in ScreenDevice...
static constexpr float W_TO_H = Galaga::ScreenDevice::Height /
                                static_cast<float>(Galaga::ScreenDevice::Width);
static constexpr float H_TO_W =
    Galaga::ScreenDevice::Width /
    static_cast<float>(Galaga::ScreenDevice::Height);

bool glfwApp::onRender(float secDeltaTime)
{
    bool ret = true;
    CHECK_OGL;  // Ensure no outstanding OGL errors.

    _animation.advance(secDeltaTime);
    if (!_animation.isAnimationRunning())
    {
        _displayQuat *= glm::angleAxis(
            _radSecSpeedRot * (secDeltaTime * 1000 /*sec to msec*/),
            glm::vec3(0., 1, 0));
        // Apply 'friction' to rotation.
        _radSecSpeedRot -= (_radSecSpeedRot * .5f);
    }

    //
    // Canvas size management
#if defined(EMSCRIPTEN)
    double width, height;
    emscripten_get_element_css_size("GLCanvas", &width, &height);
    _wndSize.x = width;
    _wndSize.y = height;
    PRINTIFCHANGED("_wndSize.x", _wndSize.x);
#endif
    glViewport(0, 0, _wndSize.x, _wndSize.y);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(_screenProgramShader);

    //
    // Compute and set 'display' position in world space..
    //
    glm::mat4 model;
    if (_animation.isAnimationRunning())
    {
        model = _animation.getMatrix();
    }
    else
    {
        model = glm::toMat4(_displayQuat);
    }
    glUniformMatrix4fv(_screenWorldLocation, 1, GL_FALSE,
                       glm::value_ptr(model));

    //
    // Compute camera view.
    //

    // Camera distance according this formula:
    // tan(fovy/2.) = (edgeX / 2) / cameraDistance
    float cameraDistance = 1.f;
    float edgeX = 1.f;
    float aspectRatio = _wndSize.x / static_cast<float>(_wndSize.y);
    if (aspectRatio < W_TO_H)  // actual window's height greater
    {
        edgeX = 0.78f / aspectRatio + 0.05f;
    }
    else  // actual window's width greater
    {
        edgeX = (W_TO_H * (1.f / 0.78f)) + 0.05f;
    }
    static constexpr float FOV = glm::radians(45.f);
    cameraDistance = (edgeX) / tan(FOV / 2.f);

#if defined(DEBUG)
    // std::cout <<"camDist="<< cameraDistance << "\n";
#endif

    float cosAngle = 0.9f;
    float alpha =
        0.5512502f - 0.2813567f * cosAngle - 0.2488394f * pow(cosAngle, 2);
    if (!_animation.isAnimationRunning())
        _cameraQuat = glm::slerp(_cameraQuat, _displayQuat, alpha);
    glm::vec4 cameraPos = glm::vec4(0, 0, cameraDistance + 0.1f, 1);
    cameraPos = _cameraQuat * cameraPos;
    glm::mat4 cameraMat =
        glm::lookAt(glm::vec3(cameraPos), glm::vec3(0, 0, 0), globalUpAxis);
    glm::mat4 proj = glm::perspective(FOV, aspectRatio, .1f, 100.0f);
    glm::mat4 viewProj = glm::mat4(proj * cameraMat);
    glUniformMatrix4fv(_viewProjectionLocation, 1, GL_FALSE,
                       glm::value_ptr(viewProj));

    //
    // Render the display.
    //
    glUniform1i(_screenTextureLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _appScreenPixelSize[0],
                    _appScreenPixelSize[1], GL_RGBA, GL_UNSIGNED_BYTE,
                    _graphicsFrame.Pixels);
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //
    // Render text objects.
    if (!_animation.isAnimationRunning(2))
    {
        animObjects[0].update(secDeltaTime);
        animObjects[0].setViewProjectionMatrix(viewProj);
        animObjects[1].update(secDeltaTime);
        animObjects[1].setViewProjectionMatrix(viewProj);
        std::array<char, 32> shownFPS;
        std::array<char, 32> shownRealOverSimulTime;
        snprintf(
            shownRealOverSimulTime.data(), shownRealOverSimulTime.size(),
            "SimTime:%d%%",
            static_cast<int>((totalMachineTime / totalSimulatedTime) * 100.));
        snprintf(shownFPS.data(), shownFPS.size(), "FPS:%d",
                 static_cast<int>(1.f / frameTime));

        animObjects[0].setText(shownRealOverSimulTime.data());
        animObjects[0].render();
        animObjects[1].setText(shownFPS.data());
        animObjects[1].render();

        if (GameStateTracker::gameState == GameStateTracker::INTRO)
        {
            animObjects[2].setViewProjectionMatrix(viewProj);
            animObjects[2].setText("   Tap\n   to\nstart");
            animObjects[2].update(secDeltaTime);
            animObjects[2].render();

            animObjects[4].setViewProjectionMatrix(viewProj);
            animObjects[4].update(secDeltaTime);
            animObjects[4].render();

            animObjects[5].setViewProjectionMatrix(viewProj);
            animObjects[5].update(secDeltaTime);
            animObjects[5].render();
        }
        else if (GameStateTracker::gameState == GameStateTracker::HIGHSCORE)
        {
            animObjects[2].setText(" Double  Tap\nto  Next  Letter");
            animObjects[2].setViewProjectionMatrix(viewProj);
            animObjects[2].update(secDeltaTime);
            animObjects[2].render();
        }
    }
    return ret;
}

bool glfwApp::onPlay(float us)
{
    bool ret = true;

    _audioApp->RenderAudio(_audioFrame.Samples, _audioFrame.Size);

    return ret;
}

void glfwApp::keysCallback(GLFWwindow *window, int key, int scancode,
                           int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_RELEASE)
    {
        glfwApp::get_app()->enterFullscreen();
    }
}

void glfwApp::handleKeys()
{
    _galagaMachine.set_InsertCoin(glfwGetKey(_window, GLFW_KEY_0) ==
                                  GLFW_PRESS);
    _galagaMachine.set_Start1Player(glfwGetKey(_window, GLFW_KEY_1) ==
                                    GLFW_PRESS);
    _galagaMachine.set_Start2Player(glfwGetKey(_window, GLFW_KEY_2) ==
                                    GLFW_PRESS);
    if (!autofireEnabled)
    {
        _galagaMachine.set_MoveLeft(glfwGetKey(_window, GLFW_KEY_A) ==
                                    GLFW_PRESS);
        _galagaMachine.set_MoveRight(glfwGetKey(_window, GLFW_KEY_D) ==
                                     GLFW_PRESS);
    }

    _galagaMachine.set_Button1(glfwGetKey(_window, GLFW_KEY_K) == GLFW_PRESS);

    if (autofireEnabled &&
        (GameStateTracker::gameState == GameStateTracker::PLAY))
    {
        static size_t autofire = 0;
        static size_t clickFire = 0;
        if ((autofire++ & 15) == 0)  // each # of frame, click fire once.
            clickFire = 2;           // Press 2 cycles long.
        _galagaMachine.set_Button1(--clickFire > 0);
    }
}

void glfwApp::enterFullscreen()
{
#if defined(EMSCRIPTEN)
    EmscriptenFullscreenStrategy fsStrategy = {};
    fsStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
    fsStrategy.canvasResolutionScaleMode =
        EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
    fsStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST;
    fsStrategy.canvasResizedCallback = &updateCanvasResize;
    fsStrategy.canvasResizedCallbackUserData = nullptr;
    EMSCRIPTEN_RESULT res;
    res = emscripten_request_fullscreen_strategy(nullptr, false, &fsStrategy);
    std::cout << __FUNCTION__ << ":" << res << "\n";
#else
    static glm::ivec2 oldSize;
    static glm::ivec2 oldPos;
    auto primaryMonitor = glfwGetPrimaryMonitor();
    GLFWmonitor *monitor = glfwGetWindowMonitor(_window);
    GLFWvidmode const *mode =
        glfwGetVideoMode(monitor ? monitor : primaryMonitor);
    if (monitor)
        glfwSetWindowMonitor(_window, 0, oldPos.x, oldPos.y, oldSize.x,
                             oldSize.y, mode->refreshRate);
    else
    {
        oldSize = glm::vec2(_wndSize);
        glfwGetWindowPos(_window, &oldPos.x, &oldPos.y);
        glfwSetWindowMonitor(_window, primaryMonitor, 0, 0, mode->width,
                             mode->height, mode->refreshRate);
    }
#endif
}

int main()
{
    int ret = 1;
    if (glfwApp::get_app()->initialize())
    {
        glfwApp::get_app()->runMessageLoop();
        ret = 0;
    }
    return ret;
}
