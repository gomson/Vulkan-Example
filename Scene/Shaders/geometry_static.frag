#version 450

layout(location = 0) in vec2 texCoords;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangeant;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0, std140) uniform Buffer {
    vec4 color;
    int hasDiffuseTexture;
};

layout(set = 0, binding = 1) uniform sampler2D diffuseTexture;
void main() {
    if(hasDiffuseTexture == 1)
        outColor = texture(diffuseTexture, texCoords);

    else
        outColor = color;
}
