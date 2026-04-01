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
layout(binding = 0, set = 7) uniform sampler2D shadowMapDebugSampler;

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
    return mix(1, texture(ambientOcclusionSampler, fragmentUVCoordinates).x, aoIntancity) * lighting.ambient.intensity * baseColor * lighting.ambient.color;
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

    // .x because gltf format stores ambient occlusion map in R channel of combined AO - Roughness - Metallness texture
    // const float ambientOcclusion = texture(ambientOcclusionSampler, fragTexCoord).x;




    // const float directionalLightIntensity = max(0, dot(normalize(normal), lighting.directionalLight.vector)) * lighting.directionalLight.intensity;
    // const float ambientLightIntensity = ambientOcclusion * lighting.ambient.intensity;


    const vec3 lightDirection = normalize(-lighting.directionalLight.vector); // Direction TO light

    vec3 result = calcAmbientLighting(baseColor) + calcDiffuseLighting(baseColor, lightDirection, localNormal) + calcSpecularLighting(lightDirection, localNormal);


    outputColor = vec4(result, 1.0);


    // outputColor = vec4(1.0) * textureProj(shadowMapSampler, fragmentShadowMapPosition);

    const vec3 shadowPos = fragmentShadowMapPosition.xyz / fragmentShadowMapPosition.w;

    // const float depth = shadowPos.z - texture(shadowMapDebugSampler, shadowPos.xy).r;

    if(global.mode == 0) {
        const float depth = 1. - texture(shadowMapDebugSampler, (shadowPos.xy + 1) / 2).r;
        outputColor = vec4(depth, depth, depth, 1);
    } else if(global.mode == 1) {
        const float depth = 1. - shadowPos.z;
        outputColor = vec4(depth, depth, depth, 1);
    } else if(global.mode == 2) {
        const float depth = (1. - texture(shadowMapDebugSampler, (shadowPos.xy + 1) / 2).r) > (1. - shadowPos.z) ? 0 : 1;
        outputColor = vec4(depth, depth, depth, 1);
    }


    // outputColor = vec4(fragmentShadowMapPosition.xyz / fragmentShadowMapPosition.w, 1);

    // outColor = vec4(texture(normalMapSampler, fragmentUVCoordinates).xyz, 1.0);
    // outColor = vec4((localNormal + 1) / 2, 1.);


    // outColor = vec4(baseColor * min(1, ambientLightIntensity + directionalLightIntensity), 1.);

    // vec3 lightsSumColor = baseColor * min(1, ambientLightIntensity + directionalLightIntensity);

    // for(uint i = 0; i < lighting.lightsCount; ++i) {
    //     const vec3 delta = lighting.lights[i].position.xyz - pixelGlobalCoordinates;
    //     const float intencity = lighting.lights[i].color.w / pow(length(delta), 2);
    //     const float d = dot(normalize(normal), normalize(delta));

    //     const float dd = d * intencity;

    //     lightsSumColor += mix(baseColor * ambientOcclusion, lighting.lights[i].color.xyz, dd);
    // }
    // lightsSumColor /= (lighting.lightsCount + 1);

    // outColor = vec4(lightsSumColor, 1.);
}

