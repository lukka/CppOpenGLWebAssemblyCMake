#pragma once

#include <random>
#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct AnimationAlongPath
{
    AnimationAlongPath()
    {
        std::random_device randomDevice;
        std::default_random_engine randomEngine(randomDevice());
        std::uniform_real_distribution<float> randomSlope(.2f, 20);
        float z = 200;
        float slope = randomSlope(randomEngine);
        std::uniform_real_distribution<float> randomAngle(0., 6.28f);
        glm::quat rotation =
            glm::angleAxis(randomAngle(randomEngine), glm::vec3(0, 0, -1.f));
        _points.resize(INTERPOLATE_SEGMENTS);
        _normals.resize(INTERPOLATE_SEGMENTS);
        size_t i = 0;
        for (auto &p : _points)
        {
            p.z = (z /= 2.f);
            p.x =
                (sqrt(p.z / slope));  // z = slope * x**2 -> x = sqrt(z / slope)
            p.y = 0;
            p = p * rotation;
        }
        i = 0;
        for (auto &n : _normals)
        {
            if (i < INTERPOLATE_SEGMENTS - 1)
                n = glm::cross(glm::vec3(0.f, 1.f, 0.f) * rotation,
                               glm::normalize(_points[i] - _points[i + 1]));
            else
                n = glm::vec3(0, 0, -1);  // Last normal is toward view point.
            ++i;
        }
    }

    void advance(float seconds) { _elapsedTime += seconds; }

    bool isAnimationRunning(float delay = 0)
    {
        return (_elapsedTime - delay) < ANIMATION_TOTAL_TIME;
    }

    glm::mat4 getMatrix() const
    {
        size_t currentBucket =
            static_cast<size_t>(_elapsedTime / segmentTimeUnit);
        size_t nextBucket = std::min(currentBucket + 1, _normals.size() - 1);
        float alpha = (_elapsedTime - currentBucket * (segmentTimeUnit)) /
                      segmentTimeUnit;
        glm::vec3 interpolatedNormal =
            glm::mix(_normals[currentBucket], _normals[nextBucket], alpha);
        glm::vec3 interpolatedPosition =
            glm::mix(_points[currentBucket], _points[nextBucket], alpha);

        return inverse(lookAt(interpolatedPosition,
                              interpolatedPosition + interpolatedNormal,
                              glm::vec3(0, 1, 0)));
    }

   private:
    std::vector<glm::vec3> _points;
    std::vector<glm::vec3> _normals;

    // Animate for few seconds...
    static constexpr float ANIMATION_TOTAL_TIME = 12.5f;  // Seconds
    static constexpr size_t INTERPOLATE_SEGMENTS = 50;
    static constexpr float segmentTimeUnit =
        ANIMATION_TOTAL_TIME / INTERPOLATE_SEGMENTS;
    float _elapsedTime = 0;
};
