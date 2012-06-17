#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D furTex;

uniform vec3 lightPos;

vec4 diffuse;

in Vertex
{
	vec2 texCoord;
	vec2 stexCoord;
	vec3 worldPos;
}IN;

out vec4 gl_FragColor[2];

void main(void)
{
	if (IN.worldPos.y != 0.0)
	{
		discard;
	}
	
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec4 fur = texture(furTex, IN.texCoord);
	vec4 shadow = texture(furTex, IN.stexCoord);
	
	
	if (fur.a > 0.04)
	{
		gl_FragColor[0] = (diffuse * 1.2);
		gl_FragColor[1] = vec4(2,1,0,1);
	}
	//else if (shadow.a > 0.04)
	//{
		//gl_FragColor[0] = vec4(diffuse.rgb * 0.6,diffuse.a);
	//}
	else
	{
		discard;
	}
	//gl_FragColor = diffuse;
}