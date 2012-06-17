#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex
{
	vec2 texCoord;
	vec3 worldPos;
} OUT;

void main(void)
{
	OUT.texCoord = texCoord;
	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;
	
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position , 1.0);
}