#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;
uniform sampler2D diffuseTex3;

uniform float maxHeight;
uniform bool multiTexture;
uniform vec4 colour;
uniform vec3 avatarPos;

in Vertex
{
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 gl_FragColor;

void main(void)
{
	if (multiTexture)
	{
		float grassHeight = 50;
		float rockHeight = 300;
		float snowHeight = maxHeight;

		vec4 texColor1;
		vec4 texColor2;
		float lerpVal;

		if ( IN.worldPos.y < grassHeight )
		{
			 texColor1 = texture2D(diffuseTex, IN.texCoord);
			 texColor2 = texture2D(diffuseTex, IN.texCoord);
			 lerpVal = 0;
		}
		else if ( IN.worldPos.y < rockHeight )
		{
			 texColor1 = texture2D(diffuseTex, IN.texCoord);
			 texColor2 = texture2D(diffuseTex2, IN.texCoord);
			 lerpVal = (IN.worldPos.y - grassHeight) / (rockHeight - grassHeight);
		}
		else if ( IN.worldPos.y < snowHeight )
		{
			 texColor1 = texture2D(diffuseTex2, IN.texCoord);
			 texColor2 = texture2D(diffuseTex3, IN.texCoord);
			 lerpVal = (IN.worldPos.y - rockHeight) / (snowHeight - rockHeight);
		}

		vec4 terrainColor = mix(texColor1, texColor2, lerpVal);
		
		gl_FragColor = terrainColor;

	}
	else
	{
		//gl_FragColor = texture2D(diffuseTex, IN.texCoord);
		gl_FragColor = colour;
	}
	
	vec3 delta = avatarPos - IN.worldPos;
	float atten = 1.0 / (1.0 + 0.000004 * dot(delta, delta));
	
	if (atten > 0.5)
	{
		gl_FragColor *= atten;
	}
	else
	{
		gl_FragColor *= vec4(vec3(0.5), 1.0);
	}
 }