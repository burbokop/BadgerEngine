
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform Global {
    mat4 transformation;
    float time;
    float _time;
    vec2 mouse;
} global;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(binding = 0, set = 1) uniform Lighting {
    PointLight lights[64];
    uint lightsCount;
    float ambient;
} lighting;

layout(binding = 0, set = 3) uniform sampler2D baseColorSampler;
layout(binding = 0, set = 4) uniform sampler2D ambientOcclusionSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 pixelGlobalCoordinates;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec4 outColor;

void main() {
    const vec3 baseColor = texture(baseColorSampler, fragTexCoord).xyz;
    const vec3 ambientOcclusionColor = texture(ambientOcclusionSampler, fragTexCoord).xyz;

    vec3 lightsSumColor = baseColor * ambientOcclusionColor * lighting.ambient;

    for(uint i = 0; i < lighting.lightsCount; ++i) {
        const vec3 delta = lighting.lights[i].position.xyz - pixelGlobalCoordinates;
        const float intencity = lighting.lights[i].color.w / pow(length(delta), 2);
        const float d = dot(normalize(normal), normalize(delta));

        const float dd = d * intencity;

        lightsSumColor += mix(baseColor * ambientOcclusionColor, lighting.lights[i].color.xyz, dd);
    }
    lightsSumColor /= (lighting.lightsCount + 1);

    outColor = vec4(lightsSumColor, 1.);
}
