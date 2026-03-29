#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0, std140) uniform Global {
    mat4 transformation;
} global;

layout(binding = 0, set = 1, std140) uniform Object {
    mat4 model;
} object;

layout(location = 0) in vec3 inputPosition;

void main() {
    gl_Position = global.transformation * object.model * vec4(inputPosition, 1.0);
}
