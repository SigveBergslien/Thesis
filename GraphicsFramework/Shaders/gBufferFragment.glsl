#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;
uniform sampler2D diffuseTex3;
uniform sampler2D bumpTex;
uniform sampler2D bumpTex2;
uniform sampler2D bumpTex3;
uniform sampler2DShadow shadowTex;

uniform bool multiTexture;
uniform float maxHeight;
uniform bool useBumpmap;
uniform bool useTexture;

uniform vec4 nodeColour;

uniform vec3 cameraPos;

uniform float fogDistance;

uniform vec3 pookaPos;
uniform float F_W_Radius;
uniform int snowCheck;
uniform float snowFactor;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 gl_FragColor[4];

void main(void)
{
	mat3 TBN;
	vec3 normal;
	if (useBumpmap)
	{
		TBN = mat3(IN.tangent, IN.binormal, IN.normal);
		normal = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
	}
	else
	{
		normal = IN.normal;
	}

	vec4 diffuse;
	if (useTexture)
	{
		if (multiTexture)
		{
			float grassHeight = 50;
			float rockHeight = 150;
			float snowHeight = 450;

			vec4 texColor1;
			vec4 texColor2;
			vec3 texBump1;
			vec3 texBump2;
			float lerpVal;

			if ( IN.worldPos.y < grassHeight )
			{
				texColor1 = texture2D(diffuseTex, IN.texCoord);
				texColor2 = texture2D(diffuseTex, IN.texCoord);
				if (useBumpmap) 
				{
					texBump1 = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
					texBump2 = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
				}
				lerpVal = 0;
			}
			else if ( IN.worldPos.y < rockHeight )
			{
				texColor1 = texture2D(diffuseTex, IN.texCoord);
				texColor2 = texture2D(diffuseTex2, IN.texCoord);
				if (useBumpmap) 
				{
					texBump1 = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
					texBump2 = normalize(TBN * (texture2D(bumpTex2, IN.texCoord).rgb * 2.0 - 1.0));
				}
				lerpVal = (IN.worldPos.y - grassHeight) / (rockHeight - grassHeight);
			}
			else if ( IN.worldPos.y < snowHeight )
			{
				texColor1 = texture2D(diffuseTex2, IN.texCoord);
				texColor2 = texture2D(diffuseTex3, IN.texCoord);
				if (useBumpmap) 
				{
					texBump1 = normalize(TBN * (texture2D(bumpTex2, IN.texCoord).rgb * 2.0 - 1.0));
					texBump2 = normalize(TBN * (texture2D(bumpTex3, IN.texCoord).rgb * 2.0 - 1.0));
				}
				lerpVal = (IN.worldPos.y - rockHeight) / (snowHeight - rockHeight);
			}
			else if ( IN.worldPos.y < maxHeight )
			{
				texColor1 = texture2D(diffuseTex3, IN.texCoord);
				texColor2 = texture2D(diffuseTex3, IN.texCoord);
				if (useBumpmap) 
				{
					texBump1 = normalize(TBN * (texture2D(bumpTex3, IN.texCoord).rgb * 2.0 - 1.0));
					texBump2 = normalize(TBN * (texture2D(bumpTex3, IN.texCoord).rgb * 2.0 - 1.0));
				}
				lerpVal = (IN.worldPos.y - snowHeight) / (maxHeight - snowHeight);
			}

			diffuse = mix(texColor1, texColor2, lerpVal);
			if (useBumpmap) 
			{
				normal = mix(texBump1, texBump2, lerpVal);
			}
		}
		else
		{
			diffuse = texture(diffuseTex, IN.texCoord);
		}
		
		//------- Snow -------
		float slope = clamp(dot(IN.normal, vec3(0.0f, 1.0f, 0.0f)), 0.0, 1.0);
		
		vec4 texColor1;
		vec4 texColor2;
		float lerpVal;
		
		slope *= snowFactor;
		
		if ( slope < 0.4 )
		{
			texColor1 = diffuse;
			texColor2 = diffuse;
			lerpVal = 0;
		}
		else if ( slope < 0.9 )
		{
			texColor1 = diffuse;
			texColor2 = vec4(1.0);
			lerpVal = (slope - 0.4) / (0.9 - 0.4);
		}
		else
		{
			texColor1 = vec4(1.0);
			texColor2 = vec4(1.0);
			lerpVal = (slope - 0.9) / (1.0 - 0.9);
		}
		
		diffuse = mix(texColor1, texColor2, lerpVal);
		//-----------------
		
	}
	else
	{
		diffuse = nodeColour;
	}
	
	gl_FragColor[0] = diffuse;
	gl_FragColor[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);

	float shadow = 1.0;
	if (IN.shadowProj.w > 0.0)
	{
		shadow = textureProj(shadowTex, IN.shadowProj);
	}
	
	//----- fog ------
	float fogRadius = 0.0;
	float fogMod = 0.0;
	float fogLength = length((IN.worldPos) - (cameraPos)) - fogDistance;
	
	if(fogLength > fogRadius)
	{
		fogMod = fogLength / 1000;
	}
	fogMod = clamp(fogMod, 0.0, 0.97);
	
	gl_FragColor[2] = vec4(0, shadow, 0, fogMod);
	
	{
		float dist_x = IN.worldPos.x - pookaPos.x;
		float dist_z = IN.worldPos.z - pookaPos.z;
		float F_W_dist = sqrt(dist_x * dist_x + dist_z * dist_z);

		float F_W_atten = 1.0 - clamp(F_W_dist / F_W_Radius, 0.0, 1.0);	
		if(F_W_atten > 0.2)
		{
			gl_FragColor[3] = vec4(0, 0, 0, 1);
		}
		else
		{
			gl_FragColor[3] = vec4(0, 0, 0, F_W_atten * 2 + 0.6);
		}
	}
 }