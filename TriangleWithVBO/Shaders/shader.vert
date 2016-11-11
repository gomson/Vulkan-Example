#version 450 core

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec2 inPosition;

void main() {
   gl_Position = vec4(inPosition, 0.0, 1.0);
}
