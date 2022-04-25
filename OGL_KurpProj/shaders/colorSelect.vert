#version 330 core
layout (location = 0) in vec3 inPos;

uniform mat4 pvm;

void main(){
	gl_Position = pvm * vec4(inPos, 1.0f);
}