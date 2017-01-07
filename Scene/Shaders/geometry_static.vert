#version 450 core

layout(push_constant) uniform Matrix_t {
    mat4 perspectiveView;
}Matrix;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangeant;

layout(location = 0) out vec2 texCoords;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 tangeant;


void main() {
    gl_Position = Matrix.perspectiveView * vec4(inPos, 1.0);
    texCoords = inTexCoords;
    normal = inNormal;
    tangeant = inTangeant;
}
