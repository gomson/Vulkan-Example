#version 450 core

out gl_PerVertex {
    vec4 gl_Position;
};

vec2 positions[] = {vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0)};

layout(location = 0) out vec2 texCoords;

void main() {
   gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
   texCoords = positions[gl_VertexIndex] * 0.5 + 0.5;
}
