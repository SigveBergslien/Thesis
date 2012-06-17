#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 gl_FragColor[2];

void main(void)	{
	gl_FragColor[0] = IN.colour* texture(diffuseTex, IN.texCoord);
	gl_FragColor[1] = vec4(3,1,0,0);
}
