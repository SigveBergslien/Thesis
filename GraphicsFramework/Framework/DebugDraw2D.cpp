#include <time.h>
#include <math.h>
#include "DebugDraw2D.h"

Mouse* DebugDraw2D::mouse = NULL;
Window* DebugDraw2D::window = NULL;

//Font* DebugDraw2D::basicFont = new Font(SOIL_load_OGL_texture("../Textures/tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

DebugDraw2D::DebugDraw2D()
{
	for(int i = 0; i < MAX_BUFFER; i++)
	{
		bufferObject[i] = 0;
	}

	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices = NULL;
	colours = NULL;
	type = GL_TRIANGLES;

	basicFont = NULL;
	basicFont = new Font(SOIL_load_OGL_texture("../Textures/tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

	if(!basicFont)
	{
		//cout << "Font error" << endl;
		return;
	}
}

DebugDraw2D::DebugDraw2D(int nv, GLuint type)
{
	for(int i = 0; i < MAX_BUFFER; i++)
	{
		bufferObject[i] = 0;
	}

	glGenVertexArrays(1, &arrayObject);

	numVertices = nv;
	vertices = new Vector3[numVertices];
	colours = new Vector4[numVertices];
	this->type = type;

	////basicFont = NULL;
	////basicFont = new Font(SOIL_load_OGL_texture("../Textures/tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

	////if(!basicFont)
	////{
	////	cout << "Font error" << endl;
	////	return;
	////}
}

DebugDraw2D::~DebugDraw2D()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] vertices;
	delete[] colours;
}

void DebugDraw2D::DrawText2D(const char* str, int x, int y, uint32 colourargb)
{
	static DebugDraw2D* m = new DebugDraw2D();
	Vector4 clr = ConvertHexColour(colourargb);

	TextMesh *tmesh = new TextMesh(str, *m->basicFont);
	tmesh->Draw();
	delete tmesh;
}

void DebugDraw2D::DrawLine2D(const Vector3& from, const Vector3& to, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_from;
	static Vector3 static_to;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(2, GL_LINES);

	if(static_from != from || static_to != to || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		m->vertices[0] = from;
		m->vertices[1] = to;
	
		m->colours[0] = clr;
		m->colours[1] = clr;
	}

	if(!set)
	{
		m->BufferData();
		static_from = from;
		static_to = to;
		static_colour = colourargb;
		set = true;
	}
	else if(static_from != from || static_to != to || static_colour != colourargb)
	{
		m->RebufferData();
		static_from = from;
		static_to = to;
		static_colour = colourargb;
	}
	m->Draw();
}

void DebugDraw2D::DrawCross2D(const Vector3& pos, float scale, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_pos;
	static float static_scale;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(4, GL_LINES);

	if(static_pos != pos || static_scale != scale || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		// horizontal line
		m->vertices[0] = Vector3(pos.x - (scale / 2.0f), pos.y, 0.0f);
		m->vertices[1] = Vector3(pos.x + (scale / 2.0f), pos.y, 0.0f);

		// vertical line
		m->vertices[2] = Vector3(pos.x, pos.y - (scale / 2.0f), 0.0f);
		m->vertices[3] = Vector3(pos.x, pos.y + (scale / 2.0f), 0.0f);
	
		m->colours[0] = clr;
		m->colours[1] = clr;
		m->colours[2] = clr;
		m->colours[3] = clr;
	}

	if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_scale = scale;
		static_colour = colourargb;
		set = true;
	}
	else if(static_pos != pos || static_scale != scale || static_colour != colourargb)
	{
		m->RebufferData();
		static_pos = pos;
		static_scale = scale;
		static_colour = colourargb;
	}

	m->Draw();
}

void DebugDraw2D::DrawCross2DRG(const Vector3& pos, float scale)
{
	static bool set = false;
	
	static Vector3 static_pos;
	static float static_scale;

	static DebugDraw2D* m = new DebugDraw2D(4, GL_LINES);

	if(static_pos != pos || static_scale != scale)
	{
		// horizontal line
		m->vertices[0] = Vector3(pos.x - (scale / 2.0f), pos.y, 0.0f);
		m->vertices[1] = Vector3(pos.x + (scale / 2.0f), pos.y, 0.0f);

		// vertical line
		m->vertices[2] = Vector3(pos.x, pos.y - (scale / 2.0f), 0.0f);
		m->vertices[3] = Vector3(pos.x, pos.y + (scale / 2.0f), 0.0f);
	
		m->colours[0] = Vector4(0.0f, 255.0f, 0.0f, 1.0f);
		m->colours[1] = Vector4(0.0f, 255.0f, 0.0f, 1.0f);
		m->colours[2] = Vector4(255.0f, 0.0f, 0.0f, 1.0f);
		m->colours[3] = Vector4(255.0f, 0.0f, 0.0f, 1.0f);
	}

	if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_scale = scale;
		set = true;
	}
	else if(static_pos != pos || static_scale != scale)
	{
		m->RebufferData();
		static_pos = pos;
		static_scale = scale;
	}

	m->Draw();
}

void DebugDraw2D::DrawArrow2D(const Vector3& from, const Vector3& to, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_from;
	static Vector3 static_to;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(6, GL_LINES);

	if(static_from != from || static_to != to || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		float length = sqrt(pow((to.x - from.x), 2) + pow((to.y - from.y), 2));

		// horizontal line
		m->vertices[0] = from;
		m->vertices[1] = to;

		// arrow head1 line
		m->vertices[2] = Vector3(to.x - (length / 10), to.y, 0.0f);
		m->vertices[3] = Vector3(to.x + (length / 10), to.y, 0.0f);

		// arrow head2 line
		m->vertices[4] = Vector3(to.x, to.y + (length / 10), 0.0f);
		m->vertices[5] = Vector3(to.x, to.y - (length / 10), 0.0f);
	
		m->colours[0] = clr;
		m->colours[1] = clr;
		m->colours[2] = clr;
		m->colours[3] = clr;
		m->colours[4] = clr;
		m->colours[5] = clr;
	}

	if(!set)
	{
		m->BufferData();
		static_from = from;
		static_to = to;
		static_colour = colourargb;
		set = true;
	}
	else if(static_from != from || static_to != to || static_colour != colourargb)
	{
		m->RebufferData();
		static_from = from;
		static_to = to;
		static_colour = colourargb;
	}
	m->Draw();
}

void DebugDraw2D::DrawTriangle2D( const Vector3& vA,const Vector3& vB,const Vector3& vC,uint32 colourargb )
{
	static bool set = false;
	
	static Vector3 static_vA;
	static Vector3 static_vB;
	static Vector3 static_vC;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(3, GL_TRIANGLES);
	
	if(static_vA != vA || static_vB != vB || static_vC != vC || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		m->vertices[0] = vA;
		m->vertices[1] = vB;
		m->vertices[2] = vC;
	
		m->colours[0] = clr;
		m->colours[1] = clr;
		m->colours[2] = clr;
	}

	if(!set)
	{
		m->BufferData();
		static_vA = vA;
		static_vB = vB;
		static_vC = vC;
		static_colour = colourargb;
		set = true;
	}
	else if(static_vA != vA || static_vB != vB || static_vC != vC || static_colour != colourargb)
	{
		m->RebufferData();
		static_vA = vA;
		static_vB = vB;
		static_vC = vC;
		static_colour = colourargb;
	}
	m->Draw();
}

void DebugDraw2D::DrawCircle2D( const Vector3& pos, float radius, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_pos;
	static float static_radius;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(361, GL_TRIANGLE_FAN);

	if(static_pos != pos || static_radius != radius || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		m->vertices[0] = pos;
		m->colours[0] = clr;

		int numPoints = 359;

		for(int i = 0; i < numPoints; i++)
		{
			float increment = ((2.0f * 3.14159f) / numPoints ) * i;
			m->vertices[i+1] =  Vector3(pos.x + (sin((float)increment) * radius), pos.y + (cos((float)increment) * radius), 0.0);
			m->colours[i+1] = clr;
		}

		m->vertices[360] =  Vector3(pos.x + (sin((float)0) * radius), pos.y + (cos((float)0) * radius), 0.0);
		m->colours[360] = clr;
	}
	
	if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_radius = radius;
		static_colour = colourargb;
		set = true;
	}
	else if(static_pos != pos || static_radius != radius || static_colour != colourargb)
	{
		m->RebufferData();
		static_pos = pos;
		static_radius = radius;
		static_colour = colourargb;
	}
	m->Draw();
}

void DebugDraw2D::DrawSquare2D(const Vector3& pos, const Vector3& dims, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_pos;
	static Vector3 static_dims;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(4, GL_TRIANGLE_STRIP);

	if(static_pos != pos || static_dims != dims || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);
    
		float offset_x = (dims.x / 2.0f);
		float offset_y = (dims.y / 2.0f);

		m->vertices[0] = Vector3(pos.x - offset_x, pos.y + offset_y, 0.0f);
		m->vertices[1] = Vector3(pos.x + offset_x,  pos.y + offset_y, 0.0f);
		m->vertices[2] = Vector3(pos.x - offset_x, pos.y - offset_y, 0.0f);
		m->vertices[3] = Vector3(pos.x + offset_x,  pos.y - offset_y, 0.0f);

		m->colours[0] = clr;
		m->colours[1] = clr;
		m->colours[2] = clr;
		m->colours[3] = clr;
	}

	if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_dims = dims;
		static_colour = colourargb;
		set = true;
	}
	else if(static_pos != pos || static_dims != dims || static_colour != colourargb)
	{
		m->RebufferData();
		static_pos = pos;
		static_dims = dims;
		static_colour = colourargb;
	}

    m->Draw();
}

void DebugDraw2D::DrawPoint2D(const Vector3& pos, float radius, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_pos;
	static float static_radius;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(1, GL_POINTS);

	if(static_pos != pos || static_radius != radius || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);
    
		m->vertices[0] = pos;
		m->colours[0] = clr;
    
		glPointSize((GLfloat)radius);
	}

    if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_radius = radius;
		static_colour = colourargb;
		set = true;
	}
	else if(static_pos != pos || static_radius != radius || static_colour != colourargb)
	{
		m->RebufferData();
		static_pos = pos;
		static_radius = radius;
		static_colour = colourargb;
	}

    m->Draw();
}

void DebugDraw2D::DrawGrid2D(const Vector3& pos, const Vector3& dims, int resolution, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_pos;
	static Vector3 static_dims;
	static int static_resolution;
	static uint32 static_colour;

	if(!set) static_resolution = 100;

	static DebugDraw2D* m = new DebugDraw2D(resolution * 4, GL_LINES);

	if(static_pos != pos || static_dims != dims || static_resolution != resolution || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		// vertical  lines

		float movex = -(dims.x / 2);
		float movey = (dims.y / 2);
		float vspace = dims.x / (resolution - 1);

		for(int i = 0; i < resolution * 2; i += 2)
		{
			m->vertices[i] = Vector3((pos.x + vspace * i / 2) + movex, pos.y + movey, pos.z);
			m->vertices[i+1] = Vector3((pos.x + vspace * i / 2) + movex, (pos.y - dims.y) + movey, pos.z);
			m->colours[i] = clr;
			m->colours[i+1] = clr;
		}

		//horizontal lines

		float hspace = dims.y / (resolution - 1);

		for(int i = resolution * 2; i < resolution * 4; i += 2)
		{
			m->vertices[i] = Vector3(pos.x + movex, pos.y - (hspace * (i - (resolution * 2)) / 2) + movey, pos.z); 
			m->vertices[i+1] = Vector3(pos.x + dims.x + movex, pos.y - (hspace * (i - (resolution * 2)) / 2) + movey, pos.z);
			m->colours[i] = clr;
			m->colours[i+1] = clr;
		}
	}

	if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_dims = dims;
		static_resolution = resolution;
		static_colour = colourargb;
		set = true;
	}
	else if(static_pos != pos || static_dims != dims || static_resolution != resolution || static_colour != colourargb)
	{
		m->RebufferData();
		static_pos = pos;
		static_dims = dims;
		static_resolution = resolution;
	}


    m->Draw();
}

void DebugDraw2D::DrawWireSquare2D(const Vector3& pos, const Vector3& dims, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_pos;
	static Vector3 static_dims;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(4, GL_LINE_LOOP);

	if(static_pos != pos || static_dims != dims || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		m->vertices[0] = Vector3(pos.x - dims.x / 2, pos.y + dims.y / 2, pos.z);
		m->vertices[1] = Vector3(pos.x - dims.x / 2, pos.y - dims.y / 2, pos.z);
		m->vertices[2] = Vector3(pos.x + dims.x / 2, pos.y - dims.y / 2, pos.z);
		m->vertices[3] = Vector3(pos.x + dims.x / 2, pos.y + dims.y / 2, pos.z);

		for(unsigned int i = 0; i < m->numVertices; i++)
		{
			m->colours[i] = clr;
		}
	}

	if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_dims = dims;
		static_colour = colourargb;
		set = true;
	}
	else if(static_pos != pos || static_dims != dims || static_colour != colourargb)
	{
		m->RebufferData();
		static_pos = pos;
		static_dims = dims;
		static_colour = colourargb;
	}

    m->Draw();
}

void DebugDraw2D::DrawWireTriangle2D(const Vector3& vA, const Vector3& vB, const Vector3& vC, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_vA;
	static Vector3 static_vB;
	static Vector3 static_vC;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(3, GL_LINE_LOOP);

	if(static_vA != vA || static_vB != vB || static_vC != vC || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		// vertical  lines

		m->vertices[0] = vA;
		m->vertices[1] = vB;
		m->vertices[2] = vC;

		for(unsigned int i = 0; i < m->numVertices; i++)
		{
			m->colours[i] = clr;
		}
	}

	if(!set)
	{
		m->BufferData();
		static_vA = vA;
		static_vB = vB;
		static_vC = vC;
		static_colour = colourargb;
		set = true;
	}
	else if(static_vA != vA || static_vB != vB || static_vC != vC || static_colour != colourargb)
	{
		m->RebufferData();
		static_vA = vA;
		static_vB = vB;
		static_vC = vC;
		static_colour = colourargb;
	}

    m->Draw();
}

void DebugDraw2D::DrawWireCircle2D(const Vector3& pos, float radius, uint32 colourargb )
{
	static bool set = false;
	
	static Vector3 static_pos;
	static float static_radius;
	static uint32 static_colour;

	static DebugDraw2D* m = new DebugDraw2D(359, GL_LINE_LOOP);

	if(static_pos != pos || static_radius != radius || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		int numPoints = 359;

		for(int i = 0; i < numPoints; i++)
		{
			float increment = ((2.0f * 3.14159f) / numPoints ) * i;
			m->vertices[i] =  Vector3(pos.x + (sin((float)increment) * radius), pos.y + (cos((float)increment) * radius), 0.0);
			m->colours[i] = clr;
		}
	}
	
	if(!set)
	{
		m->BufferData();
		static_pos = pos;
		static_radius = radius;
		static_colour = colourargb;
		set = true;
	}
	else if(static_pos != pos || static_radius != radius || static_colour != colourargb)
	{
		m->RebufferData();
		static_pos = pos;
		static_radius = radius;
		static_colour = colourargb;
	}

	m->Draw();
}

Vector3 DebugDraw2D::GetCursorPosition()
{
	mouse = Window::GetMouse();
	return(Vector3(mouse->GetAbsolutePosition().x, mouse->GetAbsolutePosition().y, 0.0f));
}

 Vector3 DebugDraw2D::GetScreenDimensions()
 {
	window = Window::GetWindow();
	return(Vector3(window->GetScreenSize().x, window->GetScreenSize().y, 0.0f));
 }

Vector4 DebugDraw2D::ConvertHexColour(uint32 colourargb)
{
	unsigned int r, g, b, a;

	uint32 mask = 255;

	b = colourargb & mask;
	colourargb = colourargb >> 8;
	g = colourargb & mask;
	colourargb = colourargb >> 8;
	r = colourargb & mask;
	colourargb = colourargb >> 8;
	a = colourargb & mask;

	return(Vector4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
}

void DebugDraw2D::Draw()
{
	glBindVertexArray(arrayObject);

	glDrawArrays(type, 0, numVertices);

	glBindVertexArray(0);
}

void DebugDraw2D::BufferData()
{
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER1]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER1]);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector3), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER1);

	if(colours)
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER1]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER1]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector4), colours, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER1);
	}

	glBindVertexArray(0);

}

void DebugDraw2D::RebufferData()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)vertices);

	if(colours)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector4), (void*)colours);
	}
}