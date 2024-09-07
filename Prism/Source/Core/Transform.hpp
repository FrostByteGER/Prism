#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Prism::Core
{
    class Transform
    {
    public:
        Transform() = default;
        void translate(const glm::vec3& deltaTranslation);
        void translate(float deltaX, float deltaY, float deltaZ);
        void rotate(const glm::quat& deltaRotation);
        void rotate(float angleDegrees, const glm::vec3& axis);
        void rotate(float angleDegrees, float deltaX, float deltaY, float deltaZ);
        void rotate(float deltaYawDegrees, float deltaPitchDegrees, float deltaRollDegrees);
        void rotate(const glm::vec3& deltaRotationDegrees);
        void scale(const glm::vec3& deltaScale);
        void scale(float deltaX, float deltaY, float deltaZ);
        void scale(float factor);
        void grow(const glm::vec3& deltaScale);
        void grow(float deltaX, float deltaY, float deltaZ);
        void grow(float factor);

        void setTranslation(const glm::vec3& translation);
        void setTranslation(float x, float y, float z);
        void setTranslationX(float x);
        void setTranslationY(float y);
        void setTranslationZ(float z);
        void setRotation(const glm::quat& rot);
        void setRotation(float angleDegrees, const glm::vec3& axis);
        void setRotation(float angleDegrees, float x, float y, float z);
        void setRotation(float yawDegrees, float pitchDegrees, float rollDegrees);
        void setRotation(const glm::vec3& rotationDegrees);
        void setScale(const glm::vec3& scale);
        void setScale(float x, float y, float z);
        void setScale(float k);

        [[nodiscard]] const glm::vec3& getTranslation() const;
        [[nodiscard]] const glm::quat& getRotationQuaternion() const;
        [[nodiscard]] glm::vec3 getRotation() const;
        [[nodiscard]] const glm::vec3& getScale() const;
        glm::mat4x4 toMatrix(bool isCamera = false);
        Transform combineWithParent(Transform& parent);

        [[nodiscard]] glm::vec3 forward() const;
        [[nodiscard]] glm::vec3 up() const;
        [[nodiscard]] glm::vec3 right() const;

        static const glm::vec3 localForward;
        static const glm::vec3 localUp;
        static const glm::vec3 localRight;

        [[nodiscard]] static glm::vec3 lookAt(const glm::vec3& direction, const glm::vec3& up);
        [[nodiscard]] static glm::vec3 rotate(const glm::vec3& vector, const glm::vec3& rotationDegrees);

    private:
        bool needsUpdate = true;
        // 1.0f = Identity Matrix
        glm::mat4x4 worldMatrix = glm::mat4x4(1.0f);

        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::identity<glm::quat>();
        glm::vec3 scaleVec = glm::vec3(1.0f);
    };
}
