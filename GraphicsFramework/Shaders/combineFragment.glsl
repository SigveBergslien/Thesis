#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;
uniform sampler2D materialTex;

uniform sampler2D fowTex;
uniform bool hfow;
uniform float fogOverride;
uniform vec3 fogColor;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void)
{
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 light = texture(emissiveTex, IN.texCoord).xyz;
	vec3 specular = texture(specularTex, IN.texCoord).xyz;
	vec4 material = texture(materialTex, IN.texCoord).rgba;

	float fogBlend = material.a;

	if(hfow) 
	{	
		vec4 fow = texture(fowTex, IN.texCoord).rgba;
		diffuse *= fow.a;
	}
		
	if (material.r == 0)
	{
		gl_FragColor.xyz = diffuse * 0.3;
		gl_FragColor.xyz += diffuse * light * material.g;
		gl_FragColor.xyz += specular * material.g;
		gl_FragColor.rgb = mix(gl_FragColor.rgb, fogColor, fogBlend);
	}
	
	else if (material.r == 1)
	{
		gl_FragColor.rgb = mix(diffuse, fogColor, fogOverride);
	}
	
	else if (material.r == 2)
	{
		gl_FragColor.rgb = mix(diffuse, fogColor, fogBlend);
	}
	
	else if (material.r == 3)
	{
		gl_FragColor.rgb = mix(diffuse, fogColor, 0.0);
		//gl_FragColor.rgb = gl_FragColor.rgb;
	}
	
	else
	{
		gl_FragColor.xyz = diffuse;
	}
	
	gl_FragColor.a = 1.0;
}