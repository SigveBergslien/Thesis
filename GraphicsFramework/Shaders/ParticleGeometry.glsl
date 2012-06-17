/*
To show of a little of what geometry shaders can do, 
we're going to add one in between our texturedVertex
and texturedFragment shaders. In our program, we're
simply passing a list of points to the vertex shader,
which we translate into screen space, and pass to the
geometry shader (the shader order always goes:
	vertex->geometry->fragment)

In our geometry shader, we're going to take in those 
points, and transform them into quads, using 
triangle strips. As we're in clip space by this 
point in the pipeline (having transformed the positions
in the vertex shader), we can calculate a screen-oriented
quad easily, simply by manipulating the x and y values
we are receiving from the vertex shader. As long 
as we don't touch the z coordinate, the perspective
divide stage will make our quads the correct size
on screen. Neat!

*/
#version 150
 
uniform float particleSize;
uniform int weatherSelect;


layout(points) in;
layout(triangle_strip, max_vertices = 4) out;


in Vertex {
	vec4 colour;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
} OUT;
 
void main() {	

	// Snow
	for(int i = 0; i < gl_in.length(); ++i)
	{
		if(weatherSelect == 1)
		{
			OUT.colour = IN[i].colour;

			//top right
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x += particleSize/2;
			gl_Position.y += particleSize;
			OUT.texCoord = vec2(1,0);

			EmitVertex();

			//Then we do the other vertices of the quad...
			//Top Left
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x -= particleSize/2;
			gl_Position.y += particleSize;
			OUT.texCoord = vec2(0,0);
			EmitVertex();

			//bottom right
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x += particleSize/2;
			gl_Position.y -= particleSize;
			OUT.texCoord = vec2(1,1);
			EmitVertex();

			//bottom Left
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x -= particleSize/2;
			gl_Position.y -= particleSize;
			OUT.texCoord = vec2(0,1);
			EmitVertex();

			EndPrimitive();
		}
		
		// Rain
		else if(weatherSelect == 2)
		{
			OUT.colour = IN[i].colour;

			//top right
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x += particleSize/20;
			gl_Position.y += particleSize;
			OUT.texCoord = vec2(1,0);
			EmitVertex();

			//Then we do the other vertices of the quad...
			//Top Left
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x -= particleSize/20;
			gl_Position.y += particleSize;
			OUT.texCoord = vec2(0,0);
			EmitVertex();

			//bottom right
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x += particleSize/20;
			gl_Position.y -= particleSize;
			OUT.texCoord = vec2(1,1);
			EmitVertex();

			//bottom Left
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x -= particleSize/20;
			gl_Position.y -= particleSize;
			OUT.texCoord = vec2(0,1);
			EmitVertex();

			EndPrimitive();
		}
		
		// default
		else
		{
			OUT.colour = IN[i].colour;

			//top right
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x += particleSize;
			gl_Position.y += particleSize;
			OUT.texCoord = vec2(1,0);

			EmitVertex();

			//Then we do the other vertices of the quad...
			//Top Left
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x -= particleSize;
			gl_Position.y += particleSize;
			OUT.texCoord = vec2(0,0);
			EmitVertex();

			//bottom right
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x += particleSize;
			gl_Position.y -= particleSize;
			OUT.texCoord = vec2(1,1);
			EmitVertex();

			//bottom Left
			gl_Position = gl_in[ i ].gl_Position;
			gl_Position.x -= particleSize;
			gl_Position.y -= particleSize;
			OUT.texCoord = vec2(0,1);
			EmitVertex();

			EndPrimitive();
		}
	}
}