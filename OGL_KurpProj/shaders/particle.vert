#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoords;

out vec4 ParticleColor;
out vec2 TexCoords;
uniform mat4 pv;
uniform mat4 model;
uniform vec4 color;

void main()
{
    ParticleColor = color;
    TexCoords = texCoords;
    gl_Position = pv * model * vec4(vertex, 1.0);
}