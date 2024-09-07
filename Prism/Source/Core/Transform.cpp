#include "Transform.hpp"

#include <glm/gtx/matrix_decompose.hpp>


namespace Prism::Core
{
    // Left handed coordinate system
    // +z is forward
    constexpr glm::vec3 Transform::localForward = glm::vec3(0.0f, 0.0f, 1.0f);
    // +y is up
    constexpr glm::vec3 Transform::localUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // +x is right
    constexpr glm::vec3 Transform::localRight = glm::vec3(1.0f, 0.0f, 0.0f);
}

void Prism::Core::Transform::translate(const glm::vec3& deltaTranslation)
{
    needsUpdate = true;
    position += deltaTranslation;
}

void Prism::Core::Transform::translate(const float deltaX, const float deltaY, const float deltaZ)
{
    translate(glm::vec3(deltaX, deltaY, deltaZ));
}

void Prism::Core::Transform::rotate(const glm::quat& deltaRotation)
{
    needsUpdate = true;
    rotation = deltaRotation * rotation;
}

void Prism::Core::Transform::rotate(const float angleDegrees, const glm::vec3& axis)
{
    rotate(glm::angleAxis(glm::radians(angleDegrees), axis));
}

void Prism::Core::Transform::rotate(const float angleDegrees, const float deltaX, const float deltaY,
                                      const float deltaZ)
{
    rotate(angleDegrees, glm::vec3(deltaX, deltaY, deltaZ));
}

void Prism::Core::Transform::rotate(const float deltaYawDegrees, const float deltaPitchDegrees,
                                      const float deltaRollDegrees)
{
    rotate(glm::quat(glm::radians(glm::vec3(deltaRollDegrees, deltaPitchDegrees, deltaYawDegrees))));
}

void Prism::Core::Transform::rotate(const glm::vec3& deltaRotationDegrees)
{
    rotate(glm::quat(glm::radians(deltaRotationDegrees)));
}

void Prism::Core::Transform::scale(const glm::vec3& deltaScale)
{
    needsUpdate = true;
    scaleVec *= deltaScale;
}

void Prism::Core::Transform::scale(const float deltaX, const float deltaY, const float deltaZ)
{
    scale(glm::vec3(deltaX, deltaY, deltaZ));
}

void Prism::Core::Transform::scale(const float factor)
{
    scale(glm::vec3(factor));
}

void Prism::Core::Transform::grow(const glm::vec3& deltaScale)
{
    needsUpdate = true;
    scaleVec += deltaScale;
}

void Prism::Core::Transform::grow(const float deltaX, const float deltaY, const float deltaZ)
{
    grow(glm::vec3(deltaX, deltaY, deltaZ));
}

void Prism::Core::Transform::grow(const float factor)
{
    grow(glm::vec3(factor));
}

void Prism::Core::Transform::setTranslation(const glm::vec3& translation)
{
    needsUpdate = true;
    position = translation;
}

void Prism::Core::Transform::setTranslation(const float x, const float y, const float z)
{
    setTranslation(glm::vec3(x, y, z));
}

void Prism::Core::Transform::setTranslationX(const float x)
{
    setTranslation(glm::vec3(x, position.y, position.z));
}

void Prism::Core::Transform::setTranslationY(const float y)
{
    setTranslation(glm::vec3(position.x, y, position.z));
}

void Prism::Core::Transform::setTranslationZ(const float z)
{
    setTranslation(glm::vec3(position.x, position.y, z));
}

void Prism::Core::Transform::setRotation(const glm::quat& rot)
{
    needsUpdate = true;
    rotation = rot;
}

void Prism::Core::Transform::setRotation(const float angleDegrees, const glm::vec3& axis)
{
    setRotation(glm::angleAxis(glm::radians(angleDegrees), axis));
}

void Prism::Core::Transform::setRotation(const float angleDegrees, const float x, const float y, const float z)
{
    setRotation(angleDegrees, glm::vec3(x, y, z));
}

void Prism::Core::Transform::setRotation(const float yawDegrees, const float pitchDegrees, const float rollDegrees)
{
    setRotation(glm::quat(glm::radians(glm::vec3(rollDegrees, pitchDegrees, yawDegrees))));
}

void Prism::Core::Transform::setRotation(const glm::vec3& rotationDegrees)
{
    setRotation(glm::quat(glm::radians(rotationDegrees)));
}

void Prism::Core::Transform::setScale(const glm::vec3& scale)
{
    needsUpdate = true;
    scaleVec = scale;
}

void Prism::Core::Transform::setScale(const float x, const float y, const float z)
{
    setScale(glm::vec3(x, y, z));
}

void Prism::Core::Transform::setScale(const float k)
{
    setScale(glm::vec3(k));
}

const glm::vec3& Prism::Core::Transform::getTranslation() const
{
    return position;
}

const glm::quat& Prism::Core::Transform::getRotationQuaternion() const
{
    return rotation;
}

glm::vec3 Prism::Core::Transform::getRotation() const
{
    return glm::degrees(glm::eulerAngles(rotation));
}

const glm::vec3& Prism::Core::Transform::getScale() const
{
    return scaleVec;
}

glm::mat4x4 Prism::Core::Transform::toMatrix(const bool isCamera)
{
    if (needsUpdate)
    {
        worldMatrix = glm::mat4x4(1.0f);
        if (isCamera)
        {
            // We always rotate to the opposite direction of everything
            worldMatrix = glm::inverse(glm::mat4_cast(glm::quat(rotation.w, -rotation.x, rotation.y, -rotation.z)));
            worldMatrix = glm::translate(worldMatrix, position * glm::vec3(-1, 1, -1));
        }
        else
        {
            worldMatrix = glm::translate(worldMatrix, position * glm::vec3(1, -1, 1));
            worldMatrix *= glm::mat4_cast(rotation);
            worldMatrix = glm::scale(worldMatrix, scaleVec);
        }

        needsUpdate = false;
    }
    return worldMatrix;
}

Prism::Core::Transform Prism::Core::Transform::combineWithParent(Transform& parent)
{
    Transform combined;

    const glm::mat4 parentMatrix = parent.toMatrix();
    const glm::mat4 childMatrix = toMatrix();
    const glm::mat4 combinedMatrix = parentMatrix * childMatrix;

    // Unused
    glm::vec3 combinedPosition;
    glm::quat combinedRotation;
    glm::vec3 combinedScale;
    glm::vec3 skewUnused;
    glm::vec4 perspectiveUnused;
    glm::decompose(combinedMatrix, combinedScale, combinedRotation, combinedPosition, skewUnused,
                   perspectiveUnused);
    // We don't need to conjugate here, a fix was seemingly implemented but reverted. https://github.com/g-truc/glm/pull/1012
    // According to the PR the issues stemmed from something else but has seemingly be fixed then! A test with godbolt has confirmed this!

    // Flip Y Axis to match the coordinate system
    combinedPosition.y *= -1;
    combined.setTranslation(combinedPosition);
    combined.setRotation(combinedRotation);
    combined.setScale(combinedScale);
    return combined;
}


glm::vec3 Prism::Core::Transform::forward() const
{
    return rotation * localForward;
}

glm::vec3 Prism::Core::Transform::up() const
{
    return rotation * localUp;
}

glm::vec3 Prism::Core::Transform::right() const
{
    return rotation * localRight;
}

glm::vec3 Prism::Core::Transform::lookAt(const glm::vec3& direction, const glm::vec3& up)
{
    const auto rotationQuat = glm::quatLookAt(direction, up);
    auto rotationDegrees = glm::degrees(glm::eulerAngles(rotationQuat));
    rotationDegrees.x = -rotationDegrees.x;
    return rotationDegrees;
}

glm::vec3 Prism::Core::Transform::rotate(const glm::vec3& vector, const glm::vec3& rotationDegrees)
{
    //Invert X axis, for whatever reason its flipped in the result of glm::rotate
    const auto invertedRotationDegrees = glm::vec3(-rotationDegrees.x, rotationDegrees.y, rotationDegrees.z);
    return glm::rotate(glm::quat(glm::radians(invertedRotationDegrees)), vector);
}
