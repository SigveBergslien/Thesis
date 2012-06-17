#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "..\Framework\Mesh.h"

//Include for data driven
#include "../Framework/HeightMapData.h"
#include "../Framework/XmlParser.h"
#include "../Framework/Types.h"
#include "../Framework/BoundingTriangle.h"

#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 1.25f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

struct Triangle
{
	Vector3* point1;
	Vector3* point2;
	Vector3* point3;
};

#define rock 0

class ASNode
{
public:
	ASNode(void) { f=g=h=0;	};
	~ASNode(void) {};
	
	unsigned int f, g, h;
	unsigned int offset;
	ASNode* parent;
};

class HeightMap : public Mesh
{
public:
		HeightMap() {};
		HeightMap(std::string name, int);
		HeightMap(std::string fname, int, int);
		HeightMap(std::string name);
		~HeightMap();

		void generateMap();
		Triangle returnTriangle(const Vector2& point) const;
		BoundingTriangle* returnTriangleList(const Vector2& point, int &numTris); 
		float getMaxHeight();
		inline Vector2* getRoutePoints() const { return routePoints; }
		inline int getRoutePointCount() const { return routePointCount; }
		inline int getBaseRadius() const { return baseRadius; }
		inline Vector3 getBaseCoord() const { return baseCoord; }
		Vector3 returnValidPlace(float r = 5);

		Vector3 getGroundPos(const Vector3 &cameraPos);

		Vector3* pathfinding(Vector3 start,Vector3 end, int& counter);
		void resetObjList(){
			objectListCount =0; 
			for(int i = 0; i < 3; i++)
			{
				++objectListCount;
				cout<<i<<"    "<<objectList[i]<<endl;
			}
		}

		//collision
		Vector3* surrondingVerts(Vector3 pos);

private:
		struct Limits
		{
			float minX;
			float maxX;
			float minZ;
			float maxZ;
		};

		void squareStep(int arr[4], int num, int granularity);
		int diamondStep(int arr[4], int granularity);
		void genMap(int a, int b, int c, int d, int i);
		void createCircle(Vector2 pos, int radius);
		void createBase(int radius);
		void createRoute(int points, int radius);
		void createVerticesMap();
		void storeVariable(string, int);
		
		int indexCount[32 * 32]; 
		Limits* limits;
		GLuint*** indicesMap;
		Vector2* routePoints;
		int routePointCount;
		int baseRadius;
		Vector3 baseCoord;
		Vector2 objectList[200];
		int objectListCount;
		bool generated;

		//Data Driven Part
		HeightMapData heightData;
		XmlParser myParser;

		int SECTORSQUARED;
		int ELEMENTS;
		int ELEMENTSQUARED;
		
		void linkchild(ASNode* parent, GLuint childnumber, GLuint goal);
		void searchChild(ASNode* P, GLuint endNode);	
		void savepath(ASNode* Node);
		unsigned int path[10000];
		unsigned int pathlength;
		int calculateHeuristic(GLuint test, GLuint goal);
		int calculateDistance(GLuint start, GLuint test);
		int Open, Close, length;
		ASNode* openlist[128 * 128];
		ASNode* closedlist[128 * 128];
		void clearAsList();

		Vector3* mid_points;
		int DIV_WIDTH;
		int DIV_HEIGHT;
		int ASconvert(int number);
		void AStarInit();
};