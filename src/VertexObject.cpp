#include "VertexObject.h"

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

glm::mat4 VertexObject::scale() const
{
    return m_scale;
}

VertexObject& VertexObject::setScale(const glm::mat4& scale)
{
    m_scale = scale;
    return *this;
}

}
