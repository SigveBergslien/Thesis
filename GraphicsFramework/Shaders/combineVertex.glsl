#version 150 core

uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex
{
	vec2 texCoord;
} OUT;

void main(void)
{
	OUT.texCoord = texCoord;
	gl_Position = projMatrix * vec4(position, 1.0);
}