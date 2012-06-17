#ifndef _DEBUGDRAW2D_H_
#define _DEBUGDRAW2D_H_

#define uint32 unsigned int

#include "OGLRenderer.h"
#include "TextMesh.h"

enum DB2Buffer
{
	VERTEX_BUFFER1,
	COLOUR_BUFFER1,
	MAX_BUFFER1
};


class DebugDraw2D
{
	public:
		
		DebugDraw2D();
		DebugDraw2D(int nv, GLuint type);
		~DebugDraw2D();
		static void DrawText2D(const char* str, int x, int y, uint32 colourargb=0xffff0000);
		static void DrawLine2D(const Vector3& from, const Vector3& to, uint32 colourargb=0xffff0000);
		static void DrawCross2D(const Vector3& pos, float scale, uint32 colourargb=0xffff0000);
		static void DrawCross2DRG(const Vector3& pos, float scale);
		static void DrawArrow2D(const Vector3& from, const Vector3& to, uint32 colourargb=0xffff00f0);
		static void DrawTriangle2D( const Vector3& vA,const Vector3& vB,const Vector3& vC,uint32 colourargb=0xffff0000 );
		static void DrawCircle2D( const Vector3& pos, float radius, uint32 colourargb=0xffff0000 );
		static void DrawSquare2D(const Vector3& pos, const Vector3& dims, uint32 colourargb=0xffff0000);
		static void DrawPoint2D(const Vector3& pos, float radius=0.1f, uint32 colourargb=0xffff00ff );
		static void DrawGrid2D(const Vector3& pos, const Vector3& dims, int resolution=10, uint32 colourargb=0xff000000);
		static void DrawWireSquare2D(const Vector3& pos, const Vector3& dims, uint32 colourargb=0xffff0000);
		static void DrawWireTriangle2D(const Vector3& vA, const Vector3& vB, const Vector3& vC, uint32 colourargb=0xfff0000);
		static void DrawWireCircle2D( const Vector3& pos, float radius, uint32 colourargb=0xffff0000 );
		static Vector3 GetCursorPosition();
		static Vector3 GetScreenDimensions();
		inline void SetScreenWidth(int sw) { SCREENWIDTH = sw; }
		inline void SetScreenHeight(int sh) { SCREENHEIGHT = sh; }

	protected:
		void BufferData();
		void RebufferData();
		GLuint arrayObject;
		GLuint bufferObject[MAX_BUFFER1];
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


#endif