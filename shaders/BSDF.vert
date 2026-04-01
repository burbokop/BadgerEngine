#version 450
#extension GL_ARB_separate_shader_objects : enable

struct PointLight {
    vec4 position;
    vec4 color;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 vector;
    vec3 color;
    float intensity;
    mat4 shadowMapTransformation;
};

layout(binding = 0, set = 0, std140) uniform Global {
    mat4 transformation;
    float time;
    vec2 mouse;
    vec3 cameraPosition;
    uint mode;
} global;

layout(binding = 0, set = 1, std140) uniform Lighting {
    PointLight lights[64];
    uint lightsCount;
    AmbientLight ambient;
    DirectionalLight directionalLight;
} lighting;

layout(binding = 0, set = 2, std140) uniform Object {
    mat4 model;
} object;

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec3 inputNormal;
layout(location = 2) in vec3 inputTangent;
layout(location = 3) in vec3 inputBitangent;
layout(location = 4) in vec3 inputColor;
layout(location = 5) in vec2 inputTexCoord;

layout(location = 0) out vec3 outputFragColor;
layout(location = 1) out vec2 outputFragTexCoord;
layout(location = 2) out vec3 outputGlobalCoordinates;
layout(location = 3) out vec3 outputNormal;
layout(location = 4) out vec3 outputTangent;
layout(location = 5) out vec3 outputBitangent;
layout(location = 6) out vec4 outputShadowMapPosition;

void main() {
    gl_Position = global.transformation * object.model * vec4(inputPosition, 1.0);
    outputGlobalCoordinates = (object.model * vec4(inputPosition, 1.0)).xyz;
    outputNormal = (object.model * vec4(inputNormal, 0)).xyz;
    outputTangent = (object.model * vec4(inputTangent, 0)).xyz;
    outputBitangent = (object.model * vec4(inputBitangent, 0)).xyz;
    outputFragColor = inputColor;
    outputFragTexCoord = inputTexCoord;
    outputShadowMapPosition = lighting.directionalLight.shadowMapTransformation * object.model * vec4(inputPosition, 1.0);
}
