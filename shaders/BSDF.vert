#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0, std140) uniform Global {
    mat4 transformation;
    float time;
    vec2 mouse;
    vec3 cameraPosition;
} global;

layout(binding = 0, set = 2, std140) uniform Object {
    mat4 model;
} object;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec3 inColor;
layout(location = 5) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 globalCoordinates;
layout(location = 3) out vec3 normal;
layout(location = 4) out vec3 tangent;
layout(location = 5) out vec3 bitangent;

void main() {
    gl_Position = global.transformation * object.model * vec4(inPosition, 1.0);
    globalCoordinates = (object.model * vec4(inPosition, 1.0)).xyz;
    normal = (object.model * vec4(inNormal, 0)).xyz;
    tangent = (object.model * vec4(inTangent, 0)).xyz;
    bitangent = (object.model * vec4(inBitangent, 0)).xyz;

    // normal = inNormal;
    // tangent = inTangent;
    // bitangent = inBitangent;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
