#version 460
layout (location = 0) in vec2 point;

out vec2 outPoint;
void main(){
    outPoint = point;
}