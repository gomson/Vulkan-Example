#version 450

layout(set = 0, binding = 0) uniform sampler2D tex;

in vec2 texCoords;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = textureLod(tex, texCoords, 1);
}