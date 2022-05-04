#version 460

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

uniform mat4 pv;
uniform mat4 model;

void main()
{
	vec4 vertPos = model * vec4(inPos + inNormal * 8, 1.0);
	gl_Position = pv * vertPos;
}