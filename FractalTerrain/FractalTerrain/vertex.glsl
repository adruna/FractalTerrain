#version 400

in vec4 in_Position;

uniform mat4 world;
uniform mat4 proj;

out vec4 ex_color;

void main()
{
    gl_Position = proj * world * in_Position;
    ex_color = vec4(0.5,0.8,0.2,1) + vec4(1,1,1,1) * in_Position.y;
}