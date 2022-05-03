#version 460

uniform sampler2D gSampler;

smooth in vec2 vTexCoord;
flat in vec4 vColorPart;

out vec4 FragColor;

void main()
{
  vec4 Color = texture2D(gSampler, vTexCoord) * vColorPart;
  if (Color.a < 0.4)
	discard;
  FragColor = Color;
}