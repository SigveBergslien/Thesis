# version 150 core

uniform sampler2D diffuseTex;

uniform bool useTexture;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void)
{
	if (useTexture)
	{
		if(texture(diffuseTex, IN.texCoord).a < 0.5)
		{
			discard;
		}
		
		gl_FragColor = texture(diffuseTex, IN.texCoord);
	}
	else
	{
		gl_FragColor = IN.colour;
	}
}