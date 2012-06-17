#include <time.h>
#include <math.h>
#include "DebugDraw3D.h"

Mouse* DebugDraw3D::mouse = NULL;
Window* DebugDraw3D::window = NULL;

DebugDraw3D::DebugDraw3D()
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

DebugDraw3D::DebugDraw3D(int nv, GLuint type)
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
}

DebugDraw3D::~DebugDraw3D()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] vertices;
	delete[] colours;
}

void DebugDraw3D::DrawText3D(const char* str, int x, int y, uint32 colourargb)
{
	static DebugDraw3D* m = new DebugDraw3D();
	Vector4 clr = ConvertHexColour(colourargb);

	TextMesh *tmesh = new TextMesh(str, *m->basicFont);
	tmesh->Draw();
	delete tmesh;
}

void DebugDraw3D::DrawLine3D(const Vector3& from, const Vector3& to, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_from;
	static Vector3 static_to;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(2, GL_LINES);

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

void DebugDraw3D::DrawCross3D(const Vector3& pos, float scale, uint32 colourargb)
{
	static bool set = false;

	static Vector3 static_pos;
	static float static_scale;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(6, GL_LINES);

	if(static_pos != pos || static_scale != scale || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		// horizontal line
		m->vertices[0] = Vector3(pos.x - (scale / 2.0f), pos.y, pos.z);
		m->vertices[1] = Vector3(pos.x + (scale / 2.0f), pos.y, pos.z);

		// vertical line
		m->vertices[2] = Vector3(pos.x, pos.y - (scale / 2.0f), pos.z);
		m->vertices[3] = Vector3(pos.x, pos.y + (scale / 2.0f), pos.z);
	
		// Depth Line
		m->vertices[4] = Vector3(pos.x, pos.y, pos.z - (scale / 2.0f));
		m->vertices[5] = Vector3(pos.x, pos.y, pos.z + (scale / 2.0f));

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

void DebugDraw3D::DrawCross3DRGB(const Vector3& pos, float scale)
{
	static bool set = false;

	static Vector3 static_pos;
	static float static_scale;

	static DebugDraw3D* m = new DebugDraw3D(6,GL_LINES);

	if(static_pos != pos || static_scale != scale)
	{
		m->numVertices = 6;
		m->type = GL_LINES;

		// horizontal line
		m->vertices[0] = Vector3(pos.x - (scale / 2.0f), pos.y, pos.z);
		m->vertices[1] = Vector3(pos.x + (scale / 2.0f), pos.y, pos.z);

		// vertical line
		m->vertices[2] = Vector3(pos.x, pos.y - (scale / 2.0f), pos.z);
		m->vertices[3] = Vector3(pos.x, pos.y + (scale / 2.0f), pos.z);

		// depth line
		m->vertices[4] = Vector3(pos.x, pos.y - (scale / 2.0f), pos.z - (scale / 2.0f));
		m->vertices[5] = Vector3(pos.x, pos.y + (scale / 2.0f), pos.z + (scale / 2.0f));
	
		m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		m->colours[1] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		m->colours[2] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		m->colours[3] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		m->colours[4] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		m->colours[5] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
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

void DebugDraw3D::DrawTriangle3D( const Vector3& vA,const Vector3& vB,const Vector3& vC,uint32 colourargb )
{
	static bool set = false;

	static Vector3 static_vA;
	static Vector3 static_vB;
	static Vector3 static_vC;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(3, GL_TRIANGLES);
	
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

void DebugDraw3D::DrawArrow3D(const Vector3& from, const Vector3& to, uint32 colourargb)
{
	static bool set = false;

	static Vector3 static_from;
	static Vector3 static_to;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(6, GL_LINES);
	
	if(static_from != from || static_to != to || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		float length = sqrt(pow((to.x - from.x), 2) + pow((to.y - from.y), 2) + pow((to.z - from.z), 2));

		// horizontal line
		m->vertices[0] = from;
		m->vertices[1] = to;

		// arrow head1 line
		m->vertices[2] = Vector3(to.x - (length / 10), to.y, to.z);
		m->vertices[3] = Vector3(to.x + (length / 10), to.y, to.z);

		// arrow head2 line
		m->vertices[4] = Vector3(to.x, to.y + (length / 10), to.z);
		m->vertices[5] = Vector3(to.x, to.y - (length / 10), to.z);
	
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
	if(static_from != from || static_to != to || static_colour != colourargb)
	{
		m->RebufferData();
		static_from = from;
		static_to = to;
		static_colour = colourargb;
	}

	m->Draw();
}

//void DebugDraw3D::DrawSphere3D( const Vector3& pos, float radius, uint32 colourargb )
//{
//	static DebugDraw3D* m = new DebugDraw3D(361, GL_TRIANGLE_FAN);
//
//	Vector4 clr = ConvertHexColour(colourargb);
//
//	m->vertices[0] = pos;
//	m->colours[0] = clr;
//
//	int numPoints = 359;
//
//	for(int i = 0; i < numPoints; i++)
//	{
//		float increment = ((2.0f * 3.14159f) / numPoints ) * i;
//
//		m->vertices[i+1] =  Vector3(pos.x + (sin((float)increment) * radius), pos.y + (cos((float)increment) * radius), pos.z);
//		m->colours[i+1] = clr;
//	}
//
//	m->vertices[360] =  Vector3(pos.x + (sin((float)0) * radius), pos.y + (cos((float)0) * radius), pos.z);
//	m->colours[360] = clr;
//	
//	m->BufferData();
//	m->Draw();
//
//	for(int i = 0; i < numPoints; i++)
//	{
//		float increment = ((2.0f * 3.14159f) / numPoints ) * i;
//
//		m->vertices[i+1] =  Vector3(pos.x + (sin((float)increment) * radius), pos.y, pos.z + (cos((float)increment) * radius));
//	}
//
//	m->vertices[360] =  Vector3(pos.x + (sin((float)0) * radius), pos.y, pos.z + (cos((float)0) * radius));
//	
//	m->BufferData();
//	m->Draw();
//
//	for(int i = 0; i < numPoints; i++)
//	{
//		float increment = ((2.0f * 3.14159f) / numPoints ) * i;
//
//		m->vertices[i+1] =  Vector3(pos.x, pos.y + (sin((float)increment) * radius), pos.z + (cos((float)increment) * radius));
//	}
//
//	m->vertices[360] =  Vector3(pos.x, pos.y + (sin((float)0) * radius), pos.z + (cos((float)0) * radius));
//	
//	m->BufferData();
//	m->Draw();
//}

void DebugDraw3D::DrawSquare3D(const Vector3& pos, const Vector3& dims, uint32 colourargb)
{
	static bool set = false;

	static Vector3 static_pos;
	static Vector3 static_dims;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(36, GL_TRIANGLES);

	if(static_pos != pos || static_dims != dims || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);
    
		float offset_x = dims.x / 2.0f;
		float offset_y = dims.y / 2.0f;
		float offset_z = dims.z / 2.0f;

		// front face
		m->vertices[0] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[1] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[2] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[3] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[4] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[5] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);

		// left face
		m->vertices[6] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[7] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[8] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[9] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[10] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[11] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z - offset_z);

		// right face
		m->vertices[12] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[13] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[14] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[15] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[16] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[17] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);

		// back face
		m->vertices[18] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[19] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[20] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[21] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[22] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[23] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z - offset_z);

		// top face
		m->vertices[24] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[25] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[26] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[27] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[28] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[29] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z - offset_z);

		// bottom face
		m->vertices[30] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[31] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[32] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[33] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[34] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[35] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);

		for(int i = 0; i < 36; i++)
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

void DebugDraw3D::DrawPoint3D(const Vector3& pos, float radius, uint32 colourargb )
{
	static bool set = false;

	static Vector3 static_pos;
	static float static_radius;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(1, GL_POINTS);

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

void DebugDraw3D::DrawGrid3D(const Vector3& axisA, const Vector3& axisB, const Vector3& origin, float resolution, uint32 colourargb)
{
	static bool set = false;

	static Vector3 static_axisA;
	static Vector3 static_axisB;
	static Vector3 static_origin;
	static float static_resolution;
	static uint32 static_colour;

	if(!set) static_resolution = 100;

	static DebugDraw3D* m = new DebugDraw3D((int)resolution * 4, GL_LINES);

	if(static_axisA != axisA || static_axisB != axisB || static_origin != origin || static_resolution != resolution || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		int count = 0;

		for (float i = 0.0f; i <= 1.0f; i += (1.0f / (resolution + 1.0f))) 
		{
			m->vertices[count] = Vector3(origin + axisB * i);
			m->colours[count++] = clr;
			m->vertices[count] = Vector3(origin + axisA + axisB * i);
			m->colours[count++] = clr;
			m->vertices[count] = Vector3(origin + axisA * i);
			m->colours[count++] = clr;
			m->vertices[count] = Vector3(origin + axisB + axisA * i);
			m->colours[count++] = clr;
		}
	}

	if(!set)
	{
		m->BufferData();
		static_axisA = axisA;
		static_axisB = axisB;
		static_origin = origin;
		static_resolution = resolution;
		static_colour = colourargb;
		set = true;
	}
	else if(static_axisA != axisA || static_axisB != axisB || static_origin != origin || static_resolution != resolution || static_colour != colourargb)
	{
		m->RebufferData();
		static_axisA = axisA;
		static_axisB = axisB;
		static_origin = origin;
		static_resolution = resolution;
		static_colour = colourargb;
	}

	m->Draw();
}

//void DebugDraw3D::DrawGrid3D(const Vector3& pos, const Vector3& dims, int resolution, uint32 colourargb)
//{
//	static bool set = false;
//
//	static Vector3 static_pos;
//	static Vector3 static_dims;
//	static int static_resolution;
//	static uint32 static_colour;
//
//	if(!set) static_resolution = 100;
//
//	static DebugDraw3D* m = new DebugDraw3D(resolution * 4, GL_LINES);
//
//	if(static_pos != pos || static_dims != dims || static_resolution != resolution || static_colour != colourargb)
//	{
//		Vector4 clr = ConvertHexColour(colourargb);
//
//		// vertical  lines
//
//		float movex = -(dims.x / 2);
//		float movey = (dims.y / 2);
//		float vspace = dims.x / (resolution - 1);
//
//		for(int i = 0; i < resolution * 2; i += 2)
//		{
//			m->vertices[i] = Vector3((pos.x + vspace * i / 2) + movex, pos.y + movey, pos.z);
//			m->vertices[i+1] = Vector3((pos.x + vspace * i / 2) + movex, (pos.y - dims.y) + movey, pos.z);
//			m->colours[i] = clr;
//			m->colours[i+1] = clr;
//		}
//
//		//horizontal lines
//
//		float hspace = dims.y / (resolution - 1);
//
//		for(int i = resolution * 2; i < resolution * 4; i += 2)
//		{
//			m->vertices[i] = Vector3(pos.x + movex, pos.y - (hspace * (i - (resolution * 2)) / 2) + movey, pos.z); 
//			m->vertices[i+1] = Vector3(pos.x + dims.x + movex, pos.y - (hspace * (i - (resolution * 2)) / 2) + movey, pos.z);
//			m->colours[i] = clr;
//			m->colours[i+1] = clr;
//		}
//	}
//
//	if(!set)
//	{
//		m->BufferData();
//		static_pos = pos;
//		static_dims = dims;
//		static_colour = colourargb;
//		set = true;
//	}
//	else if(static_pos != pos || static_dims != dims || static_colour != colourargb)
//	{
//		m->RebufferData();
//		static_pos = pos;
//		static_dims = dims;
//		static_colour = colourargb;
//	}
//
//    m->Draw();
//}

void DebugDraw3D::DrawWireCube3D(const Vector3& pos, const Vector3& dims, uint32 colourargb)
{
	static bool set = false;
	
	static Vector3 static_pos; 
	static Vector3 static_dims;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(24, GL_LINES);

	if(static_pos != pos || static_dims != dims || static_colour != colourargb)
	{
		Vector4 clr = ConvertHexColour(colourargb);

		float offset_x = dims.x / 2.0f;
		float offset_y = dims.y / 2.0f;
		float offset_z = dims.z / 2.0f;

		// front face
		m->vertices[0] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[1] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[2] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[3] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[4] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[5] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[6] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[7] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);

		// back face
		m->vertices[8] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[9] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[10] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[11] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[12] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[13] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[14] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[15] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);

		//sides
		m->vertices[16] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[17] = Vector3(pos.x - offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[18] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[19] = Vector3(pos.x - offset_x, pos.y + offset_y, pos.z - offset_z);
		m->vertices[20] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z + offset_z);
		m->vertices[21] = Vector3(pos.x + offset_x, pos.y - offset_y, pos.z - offset_z);
		m->vertices[22] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z + offset_z);
		m->vertices[23] = Vector3(pos.x + offset_x, pos.y + offset_y, pos.z - offset_z);

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

void DebugDraw3D::DrawWireTriangle3D(const Vector3& vA, const Vector3& vB, const Vector3& vC, uint32 colourargb)
{
	static bool set = false;

	static Vector3 static_vA;
	static Vector3 static_vB;
	static Vector3 static_vC;
	static uint32 static_colour;

	static DebugDraw3D* m = new DebugDraw3D(3, GL_LINE_LOOP);

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
		static_vA != vA;
		static_vB != vB;
		static_vC != vC;
		set = true;
	}
	else if(static_vA != vA || static_vB != vB || static_vC != vC || static_colour != colourargb)
	{
		m->RebufferData();
		static_vA != vA;
		static_vB != vB;
		static_vC != vC;
	}

    m->Draw();
}

void DebugDraw3D::DrawWireSphere3D( const Vector3& pos, float radius, uint32 colourargb )
{
	static bool set = false;

	static DebugDraw3D* m = new DebugDraw3D(29, GL_LINE_LOOP);

	Vector4 clr = ConvertHexColour(colourargb);

	int numPoints = 29;
	
	for(int i = 0; i < numPoints; i++)
	{
		float increment = ((2.0f * 3.14159f) / numPoints ) * i;
		m->vertices[i] =  Vector3(pos.x + (sin((float)increment) * radius), pos.y + (cos((float)increment) * radius), pos.z);
		m->colours[i] = clr;
	}
	
	if(!set)
	{
		m->BufferData();
		set = true;
	}
	else m->RebufferData();
	
	m->Draw();

	for(int i = 0; i < numPoints; i++)
	{
		float increment = ((2.0f * 3.14159f) / numPoints ) * i;
		m->vertices[i] =  Vector3(pos.x + (sin((float)increment) * radius), pos.y, pos.z  + (cos((float)increment) * radius));
	}
	
	m->RebufferData();
	m->Draw();

	for(int i = 0; i < numPoints; i++)
	{
		float increment = ((2.0f * 3.14159f) / numPoints ) * i;
		m->vertices[i] =  Vector3(pos.x, pos.y + (sin((float)increment) * radius), pos.z + (cos((float)increment) * radius));
	}

	m->RebufferData();
	m->Draw();
}

Vector4 DebugDraw3D::ConvertHexColour(uint32 colourargb)
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

void DebugDraw3D::Draw()
{
	glBindVertexArray(arrayObject);

	glDrawArrays(type, 0, numVertices);

	glBindVertexArray(0);
}

void DebugDraw3D::BufferData()
{
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER2]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER2]);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER2);

	if(colours)
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER2]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER2]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER2, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER2);
	}

	glBindVertexArray(0);

}

void DebugDraw3D::RebufferData()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER2]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)vertices);

	if(colours)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector4), (void*)colours);
	}
}
