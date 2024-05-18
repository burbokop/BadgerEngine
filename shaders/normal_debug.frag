 
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 3) uniform sampler2D texSampler;

layout(binding = 0, set = 0) uniform UniformBufferObject {
        mat4 transformation;
        float time;
        vec2 mouse;
} global;

void main() {
    outColor = (texture(texSampler, fragTexCoord) * 0.8 + vec4(fragTexCoord, 0.0, 1.0) * 0.2);
}
