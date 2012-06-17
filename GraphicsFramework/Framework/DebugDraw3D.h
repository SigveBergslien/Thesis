#ifndef _DEBUGDRAW3D_H_
#define _DEBUGDRAW3D_H_

#define uint32 unsigned int

#include "OGLRenderer.h"
#include "TextMesh.h"

enum DB3Buffer
{
	VERTEX_BUFFER2,
	COLOUR_BUFFER2,
	MAX_BUFFER2
};

class DebugDraw3D
{
	public:
		
		DebugDraw3D();
		DebugDraw3D(int nv, GLuint type);
		~DebugDraw3D();
		
		static void DrawText3D(const char* str, int x, int y, uint32 colourargb=0xffff0000);
		static void DrawLine3D(const Vector3& from, const Vector3& to, uint32 colourargb=0xffff0000);
		static void DrawCross3D(const Vector3& pos, float scale, uint32 colourargb=0xffff0000);
		static void DrawCross3DRGB(const Vector3& pos, float scale);
		static void DrawTriangle3D( const Vector3& vA,const Vector3& vB,const Vector3& vC,uint32 colourargb=0xffff0000 );
		static void DrawArrow3D(const Vector3& from, const Vector3& to, uint32 colourargb=0xffff0000);
		//static void DrawSphere3D( const Vector3& pos, float radius, uint32 colourargb=0xffff0000 );
		static void DrawSquare3D(const Vector3& pos, const Vector3& dims, uint32 colourargb=0xffff0000);
		static void DrawPoint3D(const Vector3& pos, float radius=0.1f, uint32 colourargb=0xffff0000 );
		static void DrawGrid3D(const Vector3& axisA, const Vector3& axisB, const Vector3& origin, float resolution = 10, uint32 colourargb=0xff00ff00);
		//static void DrawGrid3D(const Vector3& pos, const Vector3& dims, int resolution=10, uint32 colourargb=0xff000000);
		static void DrawWireCube3D(const Vector3& pos, const Vector3& dims, uint32 colourargb=0xffff0000);
		static void DrawWireTriangle3D(const Vector3& vA, const Vector3& vB, const Vector3& vC, uint32 colourargb=0xfff0000);
		static void DrawWireSphere3D( const Vector3& pos, float radius, uint32 colourargb=0xffff0000 );

	protected:
		void BufferData();
		void RebufferData();
		GLuint arrayObject;
		GLuint bufferObject[MAX_BUFFER2];
		GLuint numVertices;
		static Vector4 ConvertHexColour(uint32 colourargb);

		void Draw();

		GLuint type;

		Vector3* vertices;
		Vector4* colours;
		Font* basicFont;
		static Mouse* mouse;
		static Window* window;

		static int SCREENWIDTH;
		static int SCREENHEIGHT;

	private:

};

#endif;