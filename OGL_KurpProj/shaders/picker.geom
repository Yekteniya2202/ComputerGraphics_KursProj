#version 460


layout(points) in;

layout(triangle_strip, max_vertices = 4) out;

uniform float x;
uniform float y;
uniform float xWidth;
uniform float yWidth;
uniform float xDir;
void main() {
    gl_Position = vec4(x, y, 0, 1); 
    EmitVertex();

    gl_Position = vec4(x + xWidth, y, 0, 1); 
    EmitVertex();

    gl_Position = vec4(x, y + yWidth, 0, 1); 
    EmitVertex();
   
    gl_Position = vec4(x + xWidth, y + yWidth, 0, 1); 
    EmitVertex();

    
    EndPrimitive();
}