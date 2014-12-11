#version 400

in vec4 in_Position;

uniform mat4 world;
uniform mat4 proj;

out vec4 ex_color;

void main()
{
    gl_Position = proj * world * in_Position;

	// This will be interpolated through the fragment shader!!
	// TODO: play with these values to find best color scheme.
    ex_color = vec4(0.5,0.7,0.2,1) + vec4(in_Position.y/1000,in_Position.y/500,in_Position.y/100,0);
}