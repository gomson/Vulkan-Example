#version 450 core

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec2 inPos;
layout(location = 0) out vec2 texCoords;

void main() {
   gl_Position = vec4(inPos, 0.0, 1.0);
   texCoords = inPos * 0.5 + 0.5;
}
