#pragma once

#include <cassert>
#include <fstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/transform.hpp>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#if !defined(EMSCRIPTEN)
#include <glad/glad.h>
#else
#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#endif
#include <GLFW/glfw3.h>
#include "RingBuffer.h"
#include "lodepng.h"

struct FontAtlas
{
    void load(std::string path)
    {
        CHECK_OGL;
        unsigned int error = lodepng::decode(_data, _width, _height, path);
        assert(!error && "error loading and decoding file");

        // Flip upside down the image.
        for (size_t y = 0; y < _height / 2; y++)
            for (size_t x = 0; x < _width; x++)
                std::swap(
                    *(uint32_t*)&_data[y * _width * 4 + x * 4],
                    *(uint32_t*)&_data[(_height - y - 1) * _width * 4 + x * 4]);

        glGenTextures(1, &_texId);
        CHECK_OGL;
        glBindTexture(GL_TEXTURE_2D, _texId);
        CHECK_OGL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECK_OGL;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, &_data[0]);
        CHECK_OGL;
        glGenerateMipmap(GL_TEXTURE_2D);
        CHECK_OGL;
        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_OGL;
    }

    unsigned int width() const { return _width; }
    unsigned int height() const { return _height; }

    GLuint id() const { return _texId; }

   private:
    unsigned int _width, _height;
    std::vector<unsigned char> _data;
    GLuint _texId;
};

struct VertexBufferObject
{
    VertexBufferObject() { clear(); }

    void addVertexTexCoords(glm::vec3 pos, glm::vec2 tex)
    {
        // Keep bounding box updated
        _minBBox = glm::min(pos, _minBBox);
        _maxBBox = glm::max(pos, _maxBBox);

        // Insert new vertex and texcoords
        _positions[_currentBufferIndex].v = pos;
        _positions[_currentBufferIndex].st = tex;
        ++_currentBufferIndex;
    }

    glm::vec3 centerPosToBBox(glm::vec3 const& pos)
    {
        return pos - ((_maxBBox - _minBBox) / 2.f);
    }

    void clear()
    {
        _currentBufferIndex = 0;
        _positions.clear();
        _minBBox = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
        _maxBBox = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    void reset(size_t pointCount)
    {
        _currentBufferIndex = 0;
        if (pointCount > _positions.size()) _positions.resize(pointCount);
    }

    void update()
    {
        if (_currentBufferIndex > 0)
        {
            size_t bufferSizeInByte = sizeof(Point) * _currentBufferIndex;
            glBindVertexArray(_vaoId);
            glBindBuffer(GL_ARRAY_BUFFER, _positionId);
            if (_lastBuffersCount < _currentBufferIndex)
                glBufferData(GL_ARRAY_BUFFER, bufferSizeInByte, &_positions[0],
                             GL_DYNAMIC_DRAW);
            else
                glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeInByte,
                                &_positions[0]);
            CHECK_OGL;
            _lastBuffersCount = _currentBufferIndex;
        }
    }

    void draw()
    {
        glBindVertexArray(_vaoId);
        CHECK_OGL;
        glDrawArrays(GL_TRIANGLES, 0,
                     static_cast<GLsizei>(_currentBufferIndex));
        CHECK_OGL;
        glBindVertexArray(0);
        CHECK_OGL;
    }

    void create()
    {
        clear();

        CHECK_OGL;
        glGenVertexArrays(1, &_vaoId);
        glBindVertexArray(_vaoId);

        glGenBuffers(1, &_positionId);
        glBindBuffer(GL_ARRAY_BUFFER, _positionId);
        CHECK_OGL;
        glBufferData(GL_ARRAY_BUFFER, _lastBuffersCount * sizeof(Point),
                     nullptr, GL_STREAM_DRAW);
        CHECK_OGL;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point),
                              reinterpret_cast<GLvoid*>(0));
        CHECK_OGL;
        glEnableVertexAttribArray(0);
        CHECK_OGL;
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point),
                              reinterpret_cast<GLvoid*>(sizeof(glm::vec3)));
        CHECK_OGL;
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        _positions.resize(_initialVertexCount);
    }

    glm::vec3 minBBox() { return _minBBox; }
    glm::vec3 maxBBox() { return _maxBBox; }

   private:
    GLuint _vaoId;       // The id of the VAO
    GLuint _positionId;  // The buffer id of the text vertices.
    struct Point
    {
        glm::vec3 v;
        glm::vec2 st;
    };
    std::vector<Point> _positions;
    /// The last size of the OGL buffers for storing vertices and texcoords.
    size_t _lastBuffersCount = _initialVertexCount;

    size_t _currentBufferIndex = 0;
    static constexpr size_t _initialVertexCount = 32;
    /// Initial size is suited for 32 characters,
    /// each char has 6 vertices (and 6 texture coordinates).
    static constexpr size_t _initialBufferSize =
        _initialVertexCount * 6 * sizeof(Point);

    glm::vec3 _minBBox;  // The minimum of the bounding box.
    glm::vec3 _maxBBox;  // The maximum of the bounding box.
};

struct GlyphInfo
{
    int _id;
    int _x;
    int _y;
    int _w;
    int _h;
    int _xoffset;
    int _yoffset;
    int _xadvance;
};

std::string contentAfter(std::string const& src, char afterChar)
{
    int pos = 0;

    int afterCharPos = 0;

    while (pos < src.size())
    {
        if (src[pos] == afterChar)
        {
            afterCharPos = pos;
            break;
        }
        pos++;
    }

    afterCharPos += 1;

    return src.substr(afterCharPos, src.size() - afterCharPos);
}

struct Font
{
    void load(std::string const& name)
    {
        _maxGlyphHeight = 0;

        _chars.clear();
        std::string fntPath = name + ".fnt";
        std::string texPath = name + ".png";
        std::vector<std::string> contents;
        std::ifstream fontfile(fntPath.c_str());
        assert(fontfile.is_open());
        std::string line;
        while (getline(fontfile, line))
        {
            std::stringstream ss(line);
            std::string attrib;
            while (getline(ss, attrib, ' '))
            {
                if (attrib.size()) contents.push_back(attrib);
            }
        }
        fontfile.close();

        for (size_t i = 0; i < contents.size(); i++)
        {
            if (contents[i] == "char")
            {
                GlyphInfo glyphInfo;
                glyphInfo._id =
                    atoi(contentAfter(contents[i + 1], '=').c_str());
                glyphInfo._x = atoi(contentAfter(contents[i + 2], '=').c_str());
                glyphInfo._y = atoi(contentAfter(contents[i + 3], '=').c_str());
                glyphInfo._w = atoi(contentAfter(contents[i + 4], '=').c_str());
                glyphInfo._h = atoi(contentAfter(contents[i + 5], '=').c_str());
                glyphInfo._xoffset =
                    atoi(contentAfter(contents[i + 6], '=').c_str());
                glyphInfo._yoffset =
                    atoi(contentAfter(contents[i + 7], '=').c_str());
                glyphInfo._xadvance =
                    atoi(contentAfter(contents[i + 8], '=').c_str());

                _chars[glyphInfo._id] = glyphInfo;
                // Store the maximum height, used for the \n character.
                if (_maxGlyphHeight < glyphInfo._h)
                    _maxGlyphHeight = glyphInfo._h;
            }
        }

        _texture.load(texPath);
    }

    FontAtlas const& texture() const { return _texture; }

    GlyphInfo getCharInfo(char character) const
    {
        auto glyphInfo = _chars.find(static_cast<int>(character));
        if (glyphInfo != _chars.end())
        {
            return glyphInfo->second;
        }
        assert(false);
        return GlyphInfo();
    }

    size_t maxGlyphHeight() const { return _maxGlyphHeight; }

   private:
    FontAtlas _texture;
    std::map<char, GlyphInfo> _chars;
    size_t _maxGlyphHeight = 0;
};

struct Object
{
    void create(std::string fontPath)
    {
        _font.load(fontPath);
        setup();
        _vertexBuffer.create();
        _backgroundBuffer.create();
    }

    void setText(std::string text)
    {
        _vertexBuffer.reset(text.size() * 6);  // 6 vertices per character.
        int cursorX = 0;
        int cursorY = 0;

        _data = text;

        for (int i = 0; i < _data.size(); i++)
        {
            if (_data[i] == '\n')
            {
                cursorX = 0;
                cursorY += _font.maxGlyphHeight();
            }
            else
            {
                GlyphInfo cd = _font.getCharInfo(_data[i]);

                float width = static_cast<float>(_font.texture().width());
                float height = static_cast<float>(_font.texture().height());
                float charX = (float)cd._x;
                float charY = (float)cd._y;
                float charW = (float)cd._w;
                float charH = (float)cd._h;
                _vertexBuffer.addVertexTexCoords(
                    glm::vec3(cd._xoffset + cursorX, cd._yoffset + cursorY,
                              .0f) *
                        _zoom,
                    glm::vec2(charX / width, charY / height));
                _vertexBuffer.addVertexTexCoords(
                    glm::vec3(cd._xoffset + cursorX + cd._w,
                              cd._yoffset + cursorY, .0f) *
                        _zoom,
                    glm::vec2((charX + charW) / width, charY / height));
                _vertexBuffer.addVertexTexCoords(
                    glm::vec3(cd._xoffset + cursorX,
                              cd._h + cd._yoffset + cursorY, .0f) *
                        _zoom,
                    glm::vec2(charX / width, (charY + charH) / height));
                _vertexBuffer.addVertexTexCoords(
                    glm::vec3(cd._xoffset + cursorX,
                              cd._h + cd._yoffset + cursorY, .0f) *
                        _zoom,
                    glm::vec2(charX / width, (charY + charH) / height));
                _vertexBuffer.addVertexTexCoords(
                    glm::vec3(cd._xoffset + cursorX + cd._w,
                              cd._yoffset + cursorY, .0f) *
                        _zoom,
                    glm::vec2((charX + charW) / width, charY / height));
                _vertexBuffer.addVertexTexCoords(
                    glm::vec3(cd._xoffset + cursorX + cd._w,
                              cd._yoffset + cd._h + cursorY, .0f) *
                        _zoom,
                    glm::vec2((charX + charW) / width,
                              (charY + charH) / height));

                cursorX += cd._xadvance;
            }
        }
        _vertexBuffer.update();

        // Recreate the background.
        _backgroundBuffer.reset(6);
        glm::vec3 bL = _vertexBuffer.minBBox();
        glm::vec3 tR = _vertexBuffer.maxBBox();
        glm::vec3 bR = glm::vec3(tR.x, bL.y, 0);
        glm::vec3 tL = glm::vec3(bL.x, tR.y, 0);
        // First triangle.
        _backgroundBuffer.addVertexTexCoords(bL, glm::vec2(0, 0));
        _backgroundBuffer.addVertexTexCoords(tL, glm::vec2(0, 0));
        _backgroundBuffer.addVertexTexCoords(tR, glm::vec2(0, 0));
        // Second triangle.
        _backgroundBuffer.addVertexTexCoords(tR, glm::vec2(0, 0));
        _backgroundBuffer.addVertexTexCoords(bR, glm::vec2(0, 0));
        _backgroundBuffer.addVertexTexCoords(bL, glm::vec2(0, 0));
        _backgroundBuffer.update();
    }

    void render()
    {
        auto centeredPosition = _vertexBuffer.centerPosToBBox(_position);
        _model = glm::translate(centeredPosition);
        // Flip Y, for some reason the text quads are upside down.
        //_model = glm::mat4(1.f, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
        _model = glm::scale(_model, glm::vec3(1.f, -1.f, 1.f));
        CHECK_OGL;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(_program);
        glUniformMatrix4fv(glGetUniformLocation(_program, "projection"), 1,
                           GL_FALSE, glm::value_ptr(_projection));
        glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE,
                           glm::value_ptr(_model));
        glUniform3f(glGetUniformLocation(_program, "color"), _color.r, _color.g,
                    _color.b);
        glUniform1f(glGetUniformLocation(_program, "opacity"), _opacity);
        glUniform1i(glGetUniformLocation(_program, "tex1"), 1);
        CHECK_OGL;
        glActiveTexture(GL_TEXTURE1);
        CHECK_OGL;
        glBindTexture(GL_TEXTURE_2D, _font.texture().id());
        CHECK_OGL;
        _vertexBuffer.draw();
        CHECK_OGL;

        // Render background.
        if (_drawBackground)
        {
            glUseProgram(_programBackground);
            glUniformMatrix4fv(
                glGetUniformLocation(_programBackground, "projection"), 1,
                GL_FALSE, glm::value_ptr(_projection));
            glUniformMatrix4fv(
                glGetUniformLocation(_programBackground, "model"), 1, GL_FALSE,
                glm::value_ptr(_model));
            glUniform3f(glGetUniformLocation(_programBackground, "color"), .5f,
                        .8f, .8f);
            glUniform1f(glGetUniformLocation(_programBackground, "opacity"),
                        .2);
            CHECK_OGL;
            _backgroundBuffer.draw();
            CHECK_OGL;
        }

        glUseProgram(0);
        CHECK_OGL;
        glDisable(GL_BLEND);
    }

    void setFont(std::string path) { _font.load(path); }

    glm::vec2 position() const { return _position; }
    void setPosition(glm::vec3 const& pos) { _position = pos; }

    void setColor(glm::vec3 const& color) { _color = color; }
    void setOpacity(float amount) { _opacity = amount; }
    void setZoom(float zoom) { _zoom = zoom; }

    void setViewProjectionMatrix(glm::mat4 viewproj) { _projection = viewproj; }

    size_t width() const { return _width; }
    size_t height() const { return _height; }

    void setDrawBackground(bool drawBack) { _drawBackground = drawBack; }

    void setup()
    {
        // Object vertex and fragment.
        CHECK_OGL;
        _vertexprogram = 0;
        _fragmentprogram = 0;
        std::string vsStr = _vs;
        glHelper::prependShaderVersion(vsStr);
        _vertexprogram = glHelper::createShader(GL_VERTEX_SHADER, vsStr.c_str(),
                                                vsStr.size());
        std::string fsStr = _fs;
        glHelper::prependShaderVersion(fsStr);
        _fragmentprogram = glHelper::createShader(GL_FRAGMENT_SHADER,
                                                  fsStr.c_str(), fsStr.size());
        _program = glCreateProgram();
        glAttachShader(_program, _vertexprogram);
        glAttachShader(_program, _fragmentprogram);
        glLinkProgram(_program);
        glHelper::checkProgram(_program);
        CHECK_OGL;

        // Background fragment.
        std::string fsBackgroundStr = _fsBackground;
        glHelper::prependShaderVersion(fsBackgroundStr);
        _fragmentprogramBackground =
            glHelper::createShader(GL_FRAGMENT_SHADER, fsBackgroundStr.c_str(),
                                   fsBackgroundStr.size());
        _programBackground = glCreateProgram();
        glAttachShader(_programBackground, _vertexprogram);
        glAttachShader(_programBackground, _fragmentprogramBackground);
        glLinkProgram(_programBackground);
        glHelper::checkProgram(_programBackground);
        CHECK_OGL;

        _color = glm::vec3(1, 1, 1);
        _opacity = 0.99f;
    }

   protected:
    Font _font;
    std::string _data;
    VertexBufferObject _vertexBuffer;
    VertexBufferObject _backgroundBuffer;

    glm::mat4 _model;
    glm::mat4 _projection;
    glm::vec3 _position;

    glm::vec3 _color;
    float _opacity;
    float _zoom = 1.f;
    size_t _width, _height;

    GLuint _program;
    GLuint _programBackground;
    GLuint _vertexprogram;
    GLuint _fragmentprogram;
    GLuint _fragmentprogramBackground;

    bool _drawBackground = true;

    static GLchar constexpr _vs[] =
        R"(layout(location=0) in vec3 vPosition;
layout(location=1) in vec2 vertexUV;
out vec2 uv;
uniform mat4 projection;
uniform mat4 model;
void main()
{
    uv = vertexUV;
    gl_Position = projection * model * vec4(vPosition, 1.f);
}
)";

    static GLchar constexpr _fs[] =
        R"(precision mediump float;
in vec2 uv;
out vec4 out_color;
uniform sampler2D tex1;
uniform vec3 color;
uniform float opacity;
void main()
{
     vec4 rgba = texture(tex1, vec2(uv.x,-uv.y)).rgba;
     out_color = vec4(rgba.rgb * color, opacity * rgba.a);
}
)";

    static GLchar constexpr _fsBackground[] =
        R"(precision mediump float;
in vec2 uv;
out vec4 out_color;
uniform vec3 color;
uniform float opacity;
void main()
{
    uv;// Useless input, this suppresses the warning 'unused'.
    out_color = vec4(color, opacity);
}
)";
};

struct AnimatedObject : public Object
{
    enum Behavior
    {
        STANDSTILL,
        RANDOM
    };

    void update(float sec)
    {
        _elapsedTime += sec;

        auto time = std::min(
            1.f, _elapsedTime / (_nextBehaviorChange / glm::length(_speed)));
        // Time must be [0..1], 0 is the second and 1 is the third vector.
        _position = glm::catmullRom(_spline[0], _spline[1], _spline[2],
                                    _spline[3], time)
                        .xyz;
        if (time >= 1.f) doBehavior(sec);
    }

    void render() { Object::render(); }

    float zoom() const { return _zoom; }
    void setZoom(float zoom) { Object::setZoom(zoom); }

    glm::vec3 speed() const { return _speed; };
    void setSpeed(glm::vec3 speed) { _speed = speed; }

    glm::vec3 target() const { return _target; };
    void setTarget(glm::vec3 target)
    {
        _target = target;

        // Fill initial part of the split path if needed.
        if (!_spline.size())
        {
            _spline.push_back(_position);
            _spline.push_back(_position);
            _spline.push_back(_target);
            _spline.push_back(_target);
        }
        else
        {
            _spline.pop_front();         // ... and remove the first.
            _spline.push_back(_target);  // Append the argument...
        }
        assert(_spline.size() == 4);
    }

    void setBehavior(Behavior b) { _behavior = b; }
    void setBoundingBox(glm::vec3 minBBox, glm::vec3 maxBBox)
    {
        _minBBox = minBBox;
        _maxBBox = maxBBox;
    }

   protected:
    // Contains the 4 points of the current spline. The object is moving
    // from second to third point. The first and last point are for 'rounded
    // corners' movement.
    RingBuffer<glm::vec3, 4> _spline;

    float _elapsedTime = 0;
    float _nextBehaviorChange = 1;    // Change behavior period.
    Behavior _behavior = STANDSTILL;  // Default behavior.

    void doBehavior(float sec)
    {
        switch (_behavior)
        {
            case RANDOM:
            {
                if (_nextBehaviorChange < _elapsedTime)
                {
                    std::random_device randomDevice;
                    std::default_random_engine randomEngine(randomDevice());
                    std::uniform_real_distribution<float> nextTargetX(
                        _minBBox.x, _maxBBox.x);
                    std::uniform_real_distribution<float> nextTargetY(
                        _minBBox.y, _maxBBox.y);

                    _elapsedTime = 0;
                    setTarget(glm::vec3(nextTargetX(randomEngine),
                                        nextTargetY(randomEngine), 0));
                }
                break;
            }
            case STANDSTILL:
                // Nothing to do, just let the time passes.
                break;
        }
    }

    glm::vec3 _target = glm::vec3(0, 0, 0);
    glm::vec3 _speed = glm::vec3(0, 0, 0);
    glm::vec3 _minBBox = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    glm::vec3 _maxBBox = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
};
