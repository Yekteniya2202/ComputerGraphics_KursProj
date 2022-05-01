#version 330 core
in vec4 ParticleColor;
in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite1;

void main()
{
    vec4 texColor = texture(sprite1, TexCoords) * ParticleColor;
    if(texColor.a < 0.5)
        discard;
    color = texColor;
} 
