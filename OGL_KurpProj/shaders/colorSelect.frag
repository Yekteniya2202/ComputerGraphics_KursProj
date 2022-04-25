#version 330 core

//uniform int code;
uniform vec4 PickingColor;
out vec4 outputF;

void main() {
	outputF = PickingColor;
}