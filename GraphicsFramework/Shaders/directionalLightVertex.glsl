#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex
{
	vec4 position;
	vec2 texCoord;
	mat4 inverseProjView;
} OUT;

void main(void)
{
	OUT.position = vec4(position, 1);
	OUT.texCoord = texCoord;
	OUT.inverseProjView = inverse(projMatrix * viewMatrix);
	gl_Position = projMatrix * vec4(position, 1.0);
}