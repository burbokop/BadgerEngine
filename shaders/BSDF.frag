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

layout(binding = 0, set = 3) uniform sampler2D baseColorSampler;
layout(binding = 0, set = 4) uniform sampler2D ambientOcclusionSampler;
layout(binding = 0, set = 5) uniform sampler2D normalMapSampler;
layout(binding = 0, set = 6) uniform sampler2DShadow shadowMapSampler;

layout(location = 0) in vec3 fragmentColor;
layout(location = 1) in vec2 fragmentUVCoordinates;
layout(location = 2) in vec3 fragmentGlobalCoordinates;
layout(location = 3) in vec3 fragmentNormal;
layout(location = 4) in vec3 fragmentTangent;
layout(location = 5) in vec3 fragmentBitangent;
layout(location = 6) in vec4 fragmentShadowMapPosition;

layout(location = 0) out vec4 outputColor;

vec3 calcAmbientLighting(vec3 baseColor) {
    const float aoIntancity = 1;
    return mix(1, texture(ambientOcclusionSampler, fragmentUVCoordinates).r, aoIntancity) * lighting.ambient.intensity * baseColor * lighting.ambient.color;
}

vec3 calcDiffuseLighting(vec3 baseColor, vec3 lightDirection, vec3 normal) {
    const vec3 norm = normalize(normal);
    const float diff = max(dot(norm, lightDirection), 0.0);
    return diff * lighting.directionalLight.color * baseColor;
}

vec3 calcSpecularLighting(vec3 lightDirection, vec3 normal) {
    float specularStrength = 0.9;
    const vec3 norm = normalize(normal);
    const vec3 viewDir = normalize(global.cameraPosition - fragmentGlobalCoordinates);
    const vec3 reflectDir = reflect(-lightDirection, norm);
    const float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    return specularStrength * spec * lighting.directionalLight.color;
}

void main() {
    const mat3 tbn = mat3(fragmentTangent, fragmentBitangent, fragmentNormal);
    const vec3 baseColor = texture(baseColorSampler, fragmentUVCoordinates).xyz;
    const float normalMapInfluence = 0.2;
    const vec3 localNormal = normalize(tbn * (mix(vec3(0,0,1), 2. * texture(normalMapSampler, fragmentUVCoordinates).xyz - 1., normalMapInfluence)));
    const vec3 lightDirection = normalize(-lighting.directionalLight.vector); // Direction TO light

    const vec3 result = calcAmbientLighting(baseColor) + calcDiffuseLighting(baseColor, lightDirection, localNormal) + calcSpecularLighting(lightDirection, localNormal);

    outputColor = vec4(result, 1.0) * textureProj(shadowMapSampler, fragmentShadowMapPosition);
}

