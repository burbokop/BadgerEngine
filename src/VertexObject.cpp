#include "VertexObject.h"
#include <glm/ext/matrix_transform.hpp>

namespace BadgerEngine {

glm::mat4 VertexObject::rotation() const
{
    return m_rotation;
}

VertexObject& VertexObject::setRotation(const glm::mat4& rotation)
{
    m_rotation = rotation;
    return *this;
}

glm::mat4 VertexObject::translation() const
{
    return m_translation;
}

VertexObject& VertexObject::setTranslation(const glm::mat4& translation)
{
    m_translation = translation;
    return *this;
}

VertexObject& VertexObject::setTranslation(const glm::vec3& translation)
{
    return setTranslation(glm::translate(glm::mat4(1.f), translation));
}

glm::mat4 VertexObject::scale() const
{
    return m_scale;
}

VertexObject& VertexObject::setScale(const glm::mat4& scale)
{
    m_scale = scale;
    return *this;
}

VertexObject& VertexObject::setScale(const glm::vec3& scale)
{
    return setScale(glm::scale(glm::mat4(1.f), scale));
}

VertexObject& VertexObject::setTransformation(const glm::mat4& mat)
{
    m_additionalTransformation = mat;
    return *this;
}

glm::mat4 VertexObject::transformation() const
{
    return m_additionalTransformation;
}

}
