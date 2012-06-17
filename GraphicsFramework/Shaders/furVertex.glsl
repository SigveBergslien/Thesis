#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform float layer;
uniform float flength;
uniform vec3 camPos;
uniform float wind;

in vec3 position;
in vec2 texCoord;
in vec3 normal;

out Vertex
{
	vec2 texCoord;
	vec2 stexCoord;
	vec3 worldPos;
	vec3 normal;
} OUT;

void main(void)
{
	vec3 worldPos = (modelMatrix vec4(position, 1.0)).xyz;
	OUT.worldPos = worldPos;
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 tmpNormal = normalize(normalMatrix * normal);
	tmpNormal = normalize(tmpNormal + vec3(cos(wind), 0, 0));
	vec3 P = position + (tmpNormal * flength);
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp*vec4(P, 1.0);
	
	if(worldPos.y !=0.0)
	{
	
	
		float znormal = 1 - dot(tmpNormal, vec3(0, 0, 1));
	
		vec3 grav = (vec4(0, -0.4, 0, 1)*mvp).xyz;
	
		float k = pow(layer, 3.0);
		P = P + grav * k;
		
	
		float thick = layer / 5.0;
		if(thick < 1.0)
		{
			thick = 1.0;
		}
		OUT.texCoord = texCoord *4* thick;
		OUT.stexCoord = texCoord * 4 + (znormal * 0.0011);
	}
	OUT.texCoord = texCoord *4;
	OUT.stexCoord = texCoord * 4;
}
