#include "HeightMap.h"
#include <time.h>
#include <sstream>

HeightMap::HeightMap(std::string name, int i)
{
	generated = false;
	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	
	vertices = new Vector3[numVertices];
	colours = new Vector4[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	for(int x = 0; x < RAW_WIDTH; x++)
	{
		for(int z = 0; z < RAW_HEIGHT; z++)
		{
			int offset = (x * RAW_WIDTH) + z;

			vertices [offset] = Vector3(x * HEIGHTMAP_X, -1.0f, z * HEIGHTMAP_Z);
			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
		}
	}

	//Data driven part
	if(myParser.Parser("../Data/levelone.xml", (void*)&heightData, MAP))
	{
		//cout << "Map Data imported\n";
	}
	else
	{
		//cout << "Error Reading XML File: ../Data/levelone.xml \n";
	}
	
	SECTORSQUARED = heightData.sector * heightData.sector;
	ELEMENTS = (RAW_WIDTH - 1)/heightData.sector;
	ELEMENTSQUARED = (ELEMENTS * ELEMENTS);

	DIV_WIDTH = RAW_WIDTH / 2;
	DIV_HEIGHT = RAW_HEIGHT / 2;
	mid_points = new Vector3[DIV_WIDTH * DIV_HEIGHT];

	objectListCount = 0;
	generateMap();
	createVerticesMap();
	//AStarInit();
}

HeightMap::HeightMap(std::string fname, int i, int j)
{
	generated = false;

	ifstream myfile (fname.c_str());

	if(!myfile)
	{
		return;
	}

	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	colours = new Vector4[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	string line;
	float num;
	getline(myfile, line);
	istringstream iss(line);

	for(int x = 0; x < RAW_WIDTH; x++)
	{
		for(int z = 0; z < RAW_HEIGHT; z++)
		{
			iss >> num;

			int offset = (x * RAW_WIDTH) + z;

			vertices [offset] = Vector3(x * HEIGHTMAP_X, num, z * HEIGHTMAP_Z);

			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
		}
	}

	numIndices = 0;

	for(int x = 0; x < RAW_WIDTH - 1; x++)
	{
		for(int z = 0; z < RAW_HEIGHT - 1; z++)
		{
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}

	GenerateNormals();
	GenerateTangents();

	BufferData();
	generated = true;

	if(myParser.Parser("../Data/levelone.xml", (void*)&heightData, MAP))
	{
		//cout << "Map Data imported\n";
	}
	else
	{
		//cout << "Error Reading XML File: ../Data/levelone.xml \n";
	}
	
	SECTORSQUARED = heightData.sector * heightData.sector;
	ELEMENTS = (RAW_WIDTH - 1)/heightData.sector;
	ELEMENTSQUARED = (ELEMENTS * ELEMENTS);

	objectListCount = 0;
	baseRadius = 20;
	baseCoord = Vector3((float)(RAW_WIDTH - (baseRadius + heightData.border)), 0.0f, (float)(RAW_HEIGHT - (baseRadius + heightData.border)));

	createVerticesMap();

	Vector3 rp;

	routePointCount = 4;
	routePoints = new Vector2[routePointCount];
	routePoints[0] = Vector2(37.5f, 37.5f);
	rp = returnValidPlace(3);
	routePoints[1].x = rp.x / 16;
	routePoints[1].y = rp.z / 16;
	rp = returnValidPlace(3);
	routePoints[2].x = rp.x / 16;
	routePoints[2].y = rp.z / 16;
	rp = returnValidPlace(3);
	routePoints[3].x = rp.x / 16;
	routePoints[3].y = rp.z / 16;
	DIV_WIDTH = RAW_WIDTH / 2;
	DIV_HEIGHT = RAW_HEIGHT / 2;
	mid_points = new Vector3[DIV_WIDTH * DIV_HEIGHT];
	AStarInit();
	for(int i = 0;i<3;i++)
	{
		cout<<i<<"    "<<objectList[i]<<endl;
	}
}

HeightMap::HeightMap(std::string name)
{
//	srand((unsigned int)time(NULL));
	generated = false;
	std::ifstream file(name.c_str(), ios::binary);

	if (!file)
	{
		return;
	}

	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	unsigned char *data = new unsigned char[numVertices];

	file.read((char*)data, numVertices * sizeof(unsigned char));
	file.close();

	for (int x = 0; x < RAW_WIDTH; ++x)
	{
		for (int z = 0; z < RAW_HEIGHT; ++z)
		{
			int offset = (x * RAW_WIDTH) + z;

			vertices [offset] = Vector3(x * HEIGHTMAP_X, data[offset] * HEIGHTMAP_Y, z * HEIGHTMAP_Z);

			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
		}
	}

	delete data;

	numIndices = 0;

	for (int x = 0; x < RAW_WIDTH - 1; ++x)
	{
		for (int z = 0; z < RAW_HEIGHT - 1; ++z)
		{
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}

	GenerateNormals();
	GenerateTangents();

	DIV_WIDTH = RAW_WIDTH / 2;
	DIV_HEIGHT = RAW_HEIGHT / 2;
	mid_points = new Vector3[DIV_WIDTH * DIV_HEIGHT];
	AStarInit();
	BufferData();
	generated = true;
}

HeightMap::~HeightMap(void)
{
	if (indicesMap)
	{
		for(int i = 0; i < SECTORSQUARED; i++)
		{
			delete[] indicesMap[i];
		}
	}

	if(limits) delete[] limits;
	generated = false;
	if(mid_points) delete[] mid_points;
}

Vector3 HeightMap::getGroundPos(const Vector3 &cameraPos)
{
	Vector3 groundPos;

	if (cameraPos.x > 0.0f && cameraPos.x < HEIGHTMAP_X * RAW_WIDTH && cameraPos.z > 0.0f && cameraPos.z < HEIGHTMAP_Z * RAW_HEIGHT)	// only within the heighmap dimensions
	{
		int xVal = (int)(cameraPos.x / HEIGHTMAP_X);
		int offset = (int)(xVal * RAW_WIDTH + cameraPos.z / HEIGHTMAP_Z);
		
		groundPos = vertices[offset];
	}

	return groundPos;
}

void HeightMap::generateMap()
{

	for(int x = 0; x < RAW_WIDTH; x++)
	{
		for(int z = 0; z < RAW_HEIGHT; z++)
		{
			int offset = (x * RAW_WIDTH) + z;

			vertices[offset].y = -1.0f;
		}
	}

	float r;
	
	// random values for 4 corners

	r = (float)(rand() % heightData.random);
	vertices[0].y = r + 50;

	r = (float)(rand() % heightData.random);
	vertices[0 + RAW_WIDTH - 1].y = r + 50;

	r = (float)(rand() % heightData.random);
	vertices[RAW_HEIGHT * RAW_WIDTH - RAW_WIDTH].y = r + 50;
		
	r = (float)(rand() % heightData.random);
	vertices[RAW_HEIGHT * RAW_WIDTH - 1].y = r + 50;
	
	baseRadius = heightData.baseRadius;

	createBase(heightData.baseRadius);
	createRoute(heightData.routePoints, heightData.routeRadius);

	genMap(0, RAW_WIDTH - 1, (RAW_WIDTH - 1) * RAW_HEIGHT, (RAW_WIDTH * RAW_HEIGHT - 1), 1);

	for(int x = 0; x < RAW_WIDTH; x++)
	{
		for(int z = 0; z < RAW_HEIGHT; z++)
		{
			int offset = (x * RAW_WIDTH) + z;

			if(vertices[offset].y < 20.0f)
			{
				colours[offset] = Vector4(0.5f, 0.3f, 0.7f, 1);
			}

			if(vertices[offset].y >= 20.0f && vertices[offset].y < 250.0f)
			{
				colours[offset] = Vector4(1.3f, 1.8f, 1.0f, 2.0f);
			}

			if(vertices[offset].y >= 250.0f)
			{
				colours[offset] = Vector4(3.0f, 4.5f, 5.0f, 3.0f);
			}
		}
	}

	numIndices = 0;

	for(int x = 0; x < RAW_WIDTH - 1; x++)
	{
		for(int z = 0; z < RAW_HEIGHT - 1; z++)
		{
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}

	objectListCount = 0;

	for(int i = 0; i < 4; i++)
	{
		objectList[i] = routePoints[i];
		++objectListCount;
	}

	GenerateNormals();
	GenerateTangents();

	BufferData();

	if(!generated)
	{
		BufferData();
		generated = true;
	}
	else RebufferData();

	AStarInit();
}

void HeightMap::squareStep(int arr[4], int num, int granularity)
{
	int centre = 0;
	float f = 0.0f;
	float r = 0.0f;

	int randFactor = heightData.maxDiff / (int)pow((double)2,(double)granularity);

	if(num == 4)
	{
		centre = (arr[0] + arr[1] + arr[2] + arr[3]) / 2;
		f = (vertices[arr[0]].y + vertices[arr[1]].y + vertices[arr[3]].y + vertices[arr[3]].y) / 4.0f;
		r = max(0.0f, f + ((rand() % randFactor) - (randFactor / 4.0f)));
	}
	else
	{
		centre = (arr[0] + arr[1]) / 2;
		f = (vertices[arr[0]].y + vertices[arr[1]].y + vertices[arr[2]].y) / 3.0f;
		r = max(0.0f, f + ((rand() % randFactor) - (randFactor / 4.0f)));
	}

	(r < 0.0f) ? r = 0.0f : r = r; 
	
	if(vertices[centre].y == -1.0)
	{
		vertices[centre].y = r;
	}
}

int HeightMap::diamondStep(int arr[4], int granularity)
{
	int randFactor = heightData.maxDiff / (int)pow((double)2,(double)granularity);
	int centre = (arr[0] + arr[1] + arr[2] + arr[3]) / 4;
	float f = (vertices[arr[0]].y + vertices[arr[1]].y + vertices[arr[2]].y + vertices[arr[3]].y) / 4.0f;
	float r = max(0.0f, f + ((rand() % randFactor) - (randFactor / 4.0f)));

	(r < 0.0f) ? r = 0.0f : r = r;

	if(vertices[centre].y == -1.0)
	{
		vertices[centre].y = r;
	}

	return centre;
}

void HeightMap::genMap(int a, int b, int c, int d, int i)
{

	if(i < heightData.passes)
	{
		int arr1[4] = {a, b, c, d};
	
		int cp = diamondStep(arr1, i);

		int arr2[4] = {a, b, cp};
		squareStep(arr2, 3, i);

		arr2[1] = c;
		squareStep(arr2, 3, i);

		arr2[0] = b;
		arr2[1] = d;

		squareStep(arr2, 3, i);

		arr2[0] = c;

		squareStep(arr2, 3, i);

		int step = (int)pow((double)2, (double)i);

		step = (RAW_WIDTH - 1) / step;
		i++;

		genMap(a, (a + step), (a + (RAW_WIDTH * step)), cp, i);
		genMap((b - step), b, cp, (b + (RAW_WIDTH * step)), i);
		genMap((c - (RAW_WIDTH * step)), cp, c, (c + step), i);
		genMap(cp, (cp + step), (d - step), d, i);
	}
}

void HeightMap::createBase(int radius)
{
	createCircle(Vector2((float)(radius + heightData.border), (float)(radius + heightData.border)), radius);
	baseCoord = Vector3((float)(RAW_WIDTH - (radius + heightData.border)), 0.0f, (float)(RAW_HEIGHT - (radius + heightData.border)));
	createCircle(Vector2((float)(RAW_WIDTH - (radius + heightData.border)), (float)(RAW_HEIGHT - (radius + heightData.border))), radius);
}

void HeightMap::createCircle(Vector2 pos, int radius)
{
	int numPoints = 360;
	Vector2* verts = new Vector2[numPoints];

	for(int i = 0; i < numPoints; i++)
	{
		float increment = ((2.0f * 3.14159f) / numPoints ) * i;

		/*************************************
		*  DO NOT CHANGE THE LINE BELOW !!!  *
		*************************************/

		verts[i] =  Vector2((int)(pos.x + (sin((float)increment) * radius)), (int)(pos.y + (cos((float)increment) * radius)));
	}

	int minx, minz, maxx, maxz;

	minx = minz = 100000;
	maxx = maxz = 0;

	for(int i = 0; i < numPoints; i++)
	{
		(minx > verts[i].x) ? minx = (int)verts[i].x: minx = minx;
		(minz > verts[i].y) ? minz = (int)verts[i].y: minz = minz;
		(maxx < verts[i].x) ? maxx = (int)verts[i].x: maxx = maxx;
		(maxz < verts[i].y) ? maxz = (int)verts[i].y: maxz = maxz;
	}

	int count = 0;

	for(int i = minz; i <= maxz; i++)
	{
		int min = 100000;
		int max = 0;
		
		for(int j = 0; j < numPoints; j++)
		{
			if(verts[j].y == i)
			{
				(min > verts[j].x) ? min = (int)verts[j].x : min = min;
				(max < verts[j].x) ? max = (int)verts[j].x : max = max;
			}
		}
		count += (max - min + 1);
	}

	Vector2* flatten = new Vector2[count];
	count = 0;

	for(int i = minz; i <= maxz; i++)
	{
		int min = 100000;
		int max = 0;
		
		for(int j = 0; j < numPoints; j++)
		{
			if(verts[j].y == i)
			{
				(min > verts[j].x) ? min = (int)verts[j].x : min = min;
				(max < verts[j].x) ? max = (int)verts[j].x : max = max;
			}
		}
		
		int a = (max - min + 1);

		for(int k = 0; k < a; k++)
		{
			flatten[count] = Vector2((float)min, (float)i);
			min++;
			count++;
		}
	}

	for(int i = 0; i < count; i++)
	{
		int x = (int)flatten[i].x;
		int z = (int)flatten[i].y;
		int offset = (x * RAW_WIDTH) + z;
		vertices[offset].y = 0.0f;
	}

	delete[] verts;
	delete[] flatten;
}

void HeightMap::createRoute(int points, int radius)
{
	routePoints = new Vector2[points + 1];
	routePointCount = points;
	int counter = 0;
	int pointCount = 0;


	int b = heightData.border + radius;   // To ensure that points aren't generated to near edge of map

	for(int i = 0; i < points; i++)
	{
		int x, z;

		x = (rand() % (RAW_WIDTH - (2 * b))) + b;
		z = (rand() % (RAW_HEIGHT - (2 * b))) + b;

		if(((x < baseRadius * 2 + b) && (z < baseRadius * 2 + b)) || ((x > RAW_WIDTH - (baseRadius * 2 + b)) && (z > RAW_HEIGHT - (baseRadius * 2 + b))))
		{
			--i;
		}
		else
		{
			bool placeOK = true;

			for(int j = 0; j < pointCount; j++)
			{
				Vector2 tmp1 = routePoints[j];
				Vector2 tmp2 = Vector2((float)x, (float)z);

				float l = (tmp1 - tmp2).Length();
				l *= l;

				float r = (float)radius * 10;
				r *= r;

				if(l < r) placeOK = false;
			}

			if(placeOK)
			{
				routePoints[i] = Vector2((float)x, (float)z);
				createCircle(routePoints[i], radius);
				++pointCount;
			}
			else
			{
				--i;
			}
		}
	}

	routePoints[points] = Vector2((float)(RAW_WIDTH - b), (float)(RAW_HEIGHT - b));
	Vector2 current = Vector2((float)(radius + heightData.border), (float)(radius + heightData.border));

	int randx;
	int randz;

	for(int i = 0; i <= points; i++)
	{
		while(current != routePoints[i])
		{
			if(current.x < routePoints[i].x)
			{
				randx = (rand() % 4) - 1;
			}
			else if(current.x > routePoints[i].x)
			{
				randx = (rand() % 4) - 2;
			}
			else
			{
				randx = (rand() % 3) - 1;
			}

			if(current.y < routePoints[i].y)
			{
				randz = (rand() % 4) - 1;
			}
			else if(current.y > routePoints[i].y)
			{
				randz = (rand() % 4) - 2;
			}
			else
			{
				randz = (rand() % 3) - 1;
			}

			if(!((current.x + randx + b) > RAW_WIDTH || (current.x + randx - b) < 0 || (current.y + randz + b) > RAW_HEIGHT || (current.y + randz - b) < 0))
			{
				current = Vector2((current.x + randx), (current.y + randz));
				createCircle(current, radius);
			}
		}
	}
}

void HeightMap::createVerticesMap()
{
	// Create a 3-D array to hold sectorised pointers to original heightmap vectors

	Vector3**** verticesMap = new Vector3***[SECTORSQUARED];

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		verticesMap[i] = new Vector3**[ELEMENTS];

		for(int j = 0; j < ELEMENTS; j++)
		{
			verticesMap[i][j] = new Vector3*[ELEMENTS];
		}
	}

	// Split the 257 x 257 heightmap into sectors, with each sector being a 2-D array with pointers
	// to the original heightmap vectors.

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		for(int j = 0; j < ELEMENTS; j++)
		{
			for(int k = 0; k < ELEMENTS; k++)
			{
				verticesMap[i][j][k] = &vertices[(((i / heightData.sector) * (ELEMENTSQUARED * heightData.sector + ELEMENTS) + ((i % heightData.sector) * ELEMENTS)) + (j * RAW_WIDTH) + k)];
			}
		}
	}

	// Find the minimum and maximum x and z values for each sector

	limits = new Limits[SECTORSQUARED];

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		limits[i].maxX = 0.0f;
		limits[i].maxZ = 0.0f;
		limits[i].minX = 100000.0f;
		limits[i].minZ = 100000.0f;
	}

	// Set the limits for each sector.  The limits array can then be queried to find out which sector
	// a vertex can be found to make it faster to look up its information.

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		indexCount[i] = 0;

		for(int j = 0; j < ELEMENTS; j++)
		{
			for(int k = 0; k < ELEMENTS; k++)
			{
				if(limits[i].maxX < verticesMap[i][j][k]->x + HEIGHTMAP_X) limits[i].maxX = verticesMap[i][j][k]->x + HEIGHTMAP_X;
				if(limits[i].maxZ < verticesMap[i][j][k]->z + HEIGHTMAP_Z) limits[i].maxZ = verticesMap[i][j][k]->z + HEIGHTMAP_Z;
				if(limits[i].minX > verticesMap[i][j][k]->x) limits[i].minX = verticesMap[i][j][k]->x;
				if(limits[i].minZ > verticesMap[i][j][k]->z) limits[i].minZ = verticesMap[i][j][k]->z;
			}
		}
	}

	// Get x and z values for each vertex in the indices array and see which sector it belongs to and keep count.
	// Only need to store first index of each triangle, and will expand triangles out later when searching.

	for(unsigned int i = 0; i < numIndices; i+=3)
	{
		int xcheck0 = (int)vertices[indices[i]].x;
		int zcheck0 = (int)vertices[indices[i]].z;
		int xcheck1 = (int)vertices[indices[i + 1]].x;
		int zcheck1 = (int)vertices[indices[i + 1]].z;
		int xcheck2 = (int)vertices[indices[i + 2]].x;
		int zcheck2 = (int)vertices[indices[i + 2]].z;

		for(int j = 0; j < SECTORSQUARED; j++)
		{
			if((xcheck0 < limits[j].maxX && xcheck0 >= limits[j].minX && zcheck0 < limits[j].maxZ && zcheck0 >= limits[j].minZ) ||
				(xcheck1 < limits[j].maxX && xcheck1 >= limits[j].minX && zcheck1 < limits[j].maxZ && zcheck1 >= limits[j].minZ) ||
				(xcheck2 < limits[j].maxX && xcheck2 >= limits[j].minX && zcheck2 < limits[j].maxZ && zcheck2 >= limits[j].minZ))
			{
				for(int m = 0; m < ELEMENTS; m++)
				{
					for(int n = 0; n < ELEMENTS; n++)
					{
						if(verticesMap[j][m][n] == &vertices[indices[i]] || verticesMap[j][m][n] == &vertices[indices[i+1]] || verticesMap[j][m][n] == &vertices[indices[i+2]]) indexCount[j]++;	
					}
				}
			}
		}
	}

	// Create 2-D array to hold indices map - indices split up into sectors

	indicesMap = new GLuint**[SECTORSQUARED];

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		indicesMap[i] = new GLuint*[indexCount[i]];
		indexCount[i] = 0;
	}

	// Assign indices to map - only first index of each triangle needed

	for(unsigned int i = 0; i < numIndices; i+=3)
	{
		int xcheck0 = (int)vertices[indices[i]].x;
		int zcheck0 = (int)vertices[indices[i]].z;
		int xcheck1 = (int)vertices[indices[i + 1]].x;
		int zcheck1 = (int)vertices[indices[i + 1]].z;
		int xcheck2 = (int)vertices[indices[i + 2]].x;
		int zcheck2 = (int)vertices[indices[i + 2]].z;

		for(int j = 0; j < SECTORSQUARED; j++)
		{
			if((xcheck0 < limits[j].maxX && xcheck0 >= limits[j].minX && zcheck0 < limits[j].maxZ && zcheck0 >= limits[j].minZ) ||
				(xcheck1 < limits[j].maxX && xcheck1 >= limits[j].minX && zcheck1 < limits[j].maxZ && zcheck1 >= limits[j].minZ) ||
				(xcheck2 < limits[j].maxX && xcheck2 >= limits[j].minX && zcheck2 < limits[j].maxZ && zcheck2 >= limits[j].minZ))
			{
				for(int m = 0; m < ELEMENTS; m++)
				{
					for(int n = 0; n < ELEMENTS; n++)
					{
						if(verticesMap[j][m][n] == &vertices[indices[i]] || verticesMap[j][m][n] == &vertices[indices[i+1]] || verticesMap[j][m][n] == &vertices[indices[i+2]]) indicesMap[j][indexCount[j]++] = &indices[i];	
					}
				}
			}
		}
	}

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		for(int j = 0; j < ELEMENTS; j++)
		{
			delete [] verticesMap[i][j];
		}
		delete[] verticesMap[i];
	}
	delete[] verticesMap;
}

Triangle HeightMap::returnTriangle(const Vector2& point) const
{
	int xcheck = (int)point.x;
	int zcheck = (int)point.y;

	Triangle t;
	t.point1 = NULL;
	t.point2 = NULL;
	t.point3 = NULL;

	Vector3 p = Vector3(point.x, 0.0f, point.y);

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		if(xcheck < limits[i].maxX && xcheck >= limits[i].minX && zcheck < limits[i].maxZ && zcheck >= limits[i].minZ)
		{
			for(int j = 0; j < indexCount[i]; j++)
			{
				bool inside = true;

				Vector3 ab = vertices[*indicesMap[i][j]] - vertices[*(indicesMap[i][j] + 1)];
				Vector3 bc = vertices[*(indicesMap[i][j] + 1)] - vertices[*(indicesMap[i][j] + 2)];
				Vector3 ca = vertices[*(indicesMap[i][j] + 2)] - vertices[*indicesMap[i][j]];

				ab = Vector3(ab.x, 0.0f, ab.z);
				bc = Vector3(bc.x, 0.0f, bc.z);
				ca = Vector3(ca.x, 0.0f, ca.z);

				ab.Normalise();
				bc.Normalise();
				ca.Normalise();

				float d, s;

				Vector3 n = Vector3::Cross(Vector3(0, 1, 0), ab);

				d = Vector3::Dot(n, vertices[*indicesMap[i][j]]);
				s = d - Vector3::Dot(n, p);

				if(s < 0) inside = false;

				if(inside)
				{
					n = Vector3::Cross(Vector3(0, 1, 0), bc);
					d = Vector3::Dot(n, vertices[*(indicesMap[i][j] + 1)]);
					s = d - Vector3::Dot(n, p);

					if(s < 0) inside = false;
				}

				if(inside)
				{
					n = Vector3::Cross(Vector3(0, 1, 0), ca);
					d = Vector3::Dot(n, vertices[*(indicesMap[i][j] + 2)]);
					s = d - Vector3::Dot(n, p);

					if(s < 0) inside = false;
				}

				if(inside)
				{
					t.point1 = &vertices[*indicesMap[i][j]];
					t.point2 = &vertices[*(indicesMap[i][j] + 1)];
					t.point3 = &vertices[*(indicesMap[i][j] + 2)];

					return t;
				}
			}
			return t;
		}
	}
	return t;
}

BoundingTriangle* HeightMap::returnTriangleList(const Vector2& point, int &numTris)
{
	numTris = 0;
	BoundingTriangle* local = new BoundingTriangle[32];
	int index = 0;
	int xVal = (int)(point.x / 16);
	int zVal = (int)(point.y / 16);
	int offset = (xVal * 257) + zVal;

	xVal*=16;
	zVal*=16;

	if((unsigned int)offset > numVertices|| offset < 0)
	{
		return NULL;
	}

	Vector3 tmpVertex = vertices[offset];

	for(int i = 0; i < SECTORSQUARED; i++)
	{
		if(xVal < limits[i].maxX && xVal >= limits[i].minX && zVal < limits[i].maxZ && zVal >= limits[i].minZ)
		{
			for(int j = 0; j < indexCount[i]; ++j)
			{
				if(offset == *indicesMap[i][j]){
						const int i0 = *indicesMap[i][j];
						const int i1 = *(indicesMap[i][j]+1);
						const int i2 = *(indicesMap[i][j]+2);
						const Vector3& a = vertices[i0];
						const Vector3& b = vertices[i1];
						const Vector3& c = vertices[i2];
						local[numTris].initialize(a,b,c);
						numTris++;
				}
			}
		}
	}
	return local;
}

float HeightMap::getMaxHeight()
{
	float maxHeight = 0;

	for(unsigned int i = 0; i < numVertices; ++i)
	{
		if(maxHeight < vertices[i].y) maxHeight = vertices[i].y;
	}
	return maxHeight;
}

Vector3 HeightMap::returnValidPlace(float r)
{
	int x, z;
	bool placeOK;
	int counter = 0;
	do
	{
		x = (rand() % (RAW_WIDTH - 1)) + 1 ;
		z = (rand() % (RAW_HEIGHT - 1)) + 1;

		placeOK = true;

		if(((x < baseRadius * 2 + 5) && (z < baseRadius * 2 + 5)) || ((x > RAW_WIDTH - (baseRadius * 2 + 5)) && (z > RAW_HEIGHT - (baseRadius * 2 + 5))))
		{
			placeOK = false;
		}
		else if(counter < 20)
		{
			Vector2 tmp1 = Vector2((float)x, (float)z);

			for(int i = 0; i < objectListCount; i++)
			{
				Vector2 tmp2 = objectList[i];
				tmp2.x /= 16;
				tmp2.y /= 16;

				//cout << "tmp1: " << tmp1 << " tmp2: " << tmp2 << endl;

				float l = (tmp1 - tmp2).Length();
				l *= l;

				float sumr = (r * 2 + (16 * 10)) / HEIGHTMAP_X;
				sumr *= sumr;

				//cout << "sumr: " << sqrt(sumr) << " sumrsquared: " << sumr << " length: " << sqrt(l) << " length squared: " << l << endl;
				if (l < sumr)
				{
					placeOK = false;
				}
			}
		}

		if(placeOK)

			{
						int offset0 = (x * RAW_WIDTH) + z;
						int offset1 = (x * RAW_WIDTH) + z - 2;
						int offset2 = (x * RAW_WIDTH) + z + 2;
						int offset3 = ((x + 2) * RAW_WIDTH) + z;
						int offset4 = ((x - 2) * RAW_WIDTH) + z;
						int offset5 = ((x + 2) * RAW_WIDTH) + z - 2;
						int offset6 = ((x + 2) * RAW_WIDTH) + z + 2;
						int offset7 = ((x - 2) * RAW_WIDTH) + z - 2;
						int offset8 = ((x - 2) * RAW_WIDTH) + z + 2;
						if(offset1 < 0 || offset1 > 66048 || offset2 < 0 || offset2 > 66048 || offset3 < 0 || offset3 > 66048 || offset4 < 0 || offset4 > 66048 || offset5 < 0 || offset5 > 66048 || offset6 < 0 || offset6 > 66048 || offset7 < 0 || offset7 > 66048 || offset8 < 0 || offset8 > 66048)
						{

							placeOK = false;

						}

						else if(vertices[offset0].y > 0.0f || vertices[offset1].y > 0.0f || vertices[offset2].y > 0.0f || vertices[offset3].y > 0.0f || vertices[offset4].y > 0.0f || vertices[offset5].y > 0.0f || vertices[offset6].y > 0.0f || vertices[offset7].y > 0.0f || vertices[offset8].y > 0.0f)
						{

							placeOK = false;

						}

		}
		counter++;
		//cout << "Counter: " << counter << endl;
	} while(!placeOK);

	Vector3 tmp = Vector3((float) x * HEIGHTMAP_X, 0.0f, (float) z * HEIGHTMAP_Z);
	objectList[objectListCount++] = Vector2(tmp.x, tmp.z);
	return(Vector3(tmp));
}

void HeightMap::AStarInit()
{
	Open=Close=length=pathlength=0;

	for(int i = 0; i < DIV_HEIGHT; i++) {
		for(int j = 0; j < DIV_WIDTH; j++) {
			int ofs = DIV_WIDTH * i + j;
			int number = i * 2 * RAW_WIDTH + j * 2;
			mid_points[ofs] = Vector3(((float)i + 0.5f) * 32.0f, (float) ASconvert(number), ((float)j + 0.5f) * 32.0f);
		}
	}
}

int HeightMap::calculateHeuristic(GLuint test, GLuint goal)
{
	int h;
	int x = (int)abs(mid_points[test].x - mid_points[goal].x) / 32;
	int z = (int)abs(mid_points[test].z - mid_points[goal].z) / 32;
	if(x > z)
	{
		h = (x - z) * 10 + z * 14;
	}
	else
	{
		h = (z - x) * 10 + x * 14;
	}
	return h;
}

int HeightMap::calculateDistance(GLuint parent, GLuint test)
{
	int d;
	int x = (int)abs(mid_points[test].x - mid_points[parent].x) / 32;
	int z = (int)abs(mid_points[test].z - mid_points[parent].z) / 32;
	if(x == z)
	{
		d = 14;
	}
	else
	{
		d = 10;
	}
	return d;
}

void HeightMap::linkchild(ASNode* parent, GLuint childnumber, GLuint goal)
{
	ASNode* check = NULL;
		
	int g_score = parent->g + calculateDistance(parent->offset, childnumber);

	for(int i = 0; i < Close; ++i)
	{
		if(closedlist[i]->offset == childnumber)
		{
			check =  closedlist[i];
			break;
		}
	}

	if(check)
	{
		if((unsigned)g_score < check->g)
		{
			check->parent = parent;
			check->g = g_score;
			check->f = check->g + check->h;
		}
	}
	else
	{
		for(int i = 0; i < Open; ++i)
		{
			if(openlist[i] != NULL)
			{
				if(openlist[i]->offset == childnumber)
				{
					check = openlist[i];
					break;
				}
			}
		}
		if(check)
		{
			if((unsigned)g_score < check->g)
			{
				check->parent = parent;
				check->g = g_score;
				check->f = check->g + check->h;
			}
		}
		else
		{
			ASNode* child = new ASNode;
			child->parent = parent;
			child->g = g_score;
			child->h = calculateHeuristic(childnumber, goal);
			child->f = child->g + child->h;
			child->offset = childnumber;
			openlist[Open++] = child;
			length++;
		}
	}
}

void HeightMap::searchChild(ASNode* P, GLuint end)
{
	int parentnumber = P->offset;
		int childnumber;

		if((parentnumber + 1) % DIV_WIDTH)
		{
			childnumber = parentnumber + 1;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}
		
		if(parentnumber % DIV_WIDTH)
		{
			childnumber = parentnumber - 1;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}

		if(parentnumber < DIV_WIDTH * (DIV_HEIGHT-1))
		{
			childnumber = parentnumber + DIV_WIDTH;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}

		if(parentnumber >= DIV_WIDTH)
		{
			childnumber = parentnumber - DIV_WIDTH;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}

		if(parentnumber < DIV_WIDTH * (DIV_HEIGHT-1) && (parentnumber + 1) % DIV_WIDTH)
		{
			childnumber = parentnumber + DIV_WIDTH + 1;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}

		if(parentnumber < DIV_WIDTH * (DIV_HEIGHT-1) && parentnumber % DIV_WIDTH)
		{
			childnumber = parentnumber + DIV_WIDTH - 1;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}
		
		if(parentnumber >= DIV_WIDTH && (parentnumber + 1) % DIV_WIDTH)
		{
			childnumber = parentnumber - DIV_WIDTH + 1;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}
		
		if(parentnumber >= DIV_WIDTH && parentnumber % DIV_WIDTH)
		{
			childnumber = parentnumber - DIV_WIDTH - 1;
			if(mid_points[childnumber].y <= rock)
			{
				linkchild(P,childnumber,end);
			}
		}
}

Vector3* HeightMap::pathfinding(const Vector3 start, const Vector3 end, int& counter)
{
	//outside
	if(end.x <= 0.0f || end.x >= HEIGHTMAP_X * RAW_WIDTH || end.z <= 0.0f || end.z >= HEIGHTMAP_Z * RAW_HEIGHT)
	{
		return NULL;
	}
	if(start.x <= 0.0f || start.x >= HEIGHTMAP_X * RAW_WIDTH || start.z <= 0.0f || start.z >= HEIGHTMAP_Z * RAW_HEIGHT)
	{
		return NULL;
	}
	
	int x = (int)start.x / (int)(HEIGHTMAP_X * 2.0f) ;
	int z = (int)start.z / (int)(HEIGHTMAP_Z * 2.0f);
	int s = x * DIV_WIDTH + z;
	x = (int)end.x / (int)(HEIGHTMAP_X *  (int)2.0f);
	z = (int)end.z / (int)(HEIGHTMAP_Z *  (int)2.0f);
	int e = x * DIV_WIDTH + z;

	if(mid_points[e].y > rock || mid_points[s].y > rock || s == e ) 
	{
		return NULL;
	}	

	ASNode* startNode = new ASNode;	
	startNode->offset = s;
	startNode->g = 0;
	startNode->h = calculateHeuristic(s, e);
	startNode->f = startNode->g + startNode->h;
	startNode->parent = NULL;

	Open = 0; Close = 0; length = 0;
	openlist[Open++] = startNode;
	length++;

	while(length)
	{
		int lowestF,number;
		ASNode* P = NULL;
		lowestF = (RAW_WIDTH + RAW_WIDTH) * 10;
		for(int i = 0; i < Open; ++i)
		{
			if(openlist[i] != NULL)
			{
				if(openlist[i]->f <= (unsigned)lowestF)
				{
					lowestF = openlist[i]->f;
					P = openlist[i];
					number = i;
				}
			}
		}
		closedlist[Close++] = P;
		openlist[number] = NULL;
		length--;

		if(P->offset == e)
		{
			pathlength = 0;
			savepath(P);
			break;
		}

		searchChild(P, e);
	}


	if(length ==0)
	{
		clearAsList();
		return NULL;
	}

	Vector3* tmp= new Vector3[pathlength];

	for(unsigned int i =0; i<pathlength; i++)
	{
		tmp[i] = mid_points[path[pathlength-i-1]];
	}
	counter = pathlength;
	clearAsList();
	return tmp;
}

void HeightMap::savepath(ASNode* Node)
{
	if(Node!=NULL)
	{
		path[pathlength++] = Node->offset;
		savepath(Node->parent);
	}
}

void HeightMap::clearAsList() {
	for(int i = 0; i < Open; ++i) {
		delete openlist[i];	
		openlist[i] = NULL;
	}
	for(int i = 0; i < Close; ++i) {
		delete closedlist[i];
		closedlist[i] = NULL;
	}
}

int HeightMap::ASconvert(int number) 
{
	if(vertices[number].y > 0)
	{
		return 1;
	}
	if(vertices[number + 1].y > 0)
	{
		return 1;
	}
	if(vertices[number + 2].y > 0)
	{
		return 1;
	}
	if(vertices[number + RAW_WIDTH].y > 0)
	{
		return 1;
	}
	if(vertices[number + RAW_WIDTH + 1].y > 0)
	{
		return 1;
	}
	if(vertices[number + RAW_WIDTH + 2].y > 0)
	{
		return 1;
	}
	if(vertices[number + 2 * RAW_WIDTH].y > 0)
	{
		return 1;
	}
	if(vertices[number + 2 * RAW_WIDTH + 1].y > 0)
	{
		return 1;
	}
	if(vertices[number + 2 * RAW_WIDTH + 2].y > 0)
	{
		return 1;
	}

	return 0;
}

Vector3* HeightMap::surrondingVerts(Vector3 pos){
	int div = (int)pos.x / (int)HEIGHTMAP_X;
	int s = div * RAW_WIDTH + (int)(pos.z / HEIGHTMAP_Z);

	if((unsigned int)s>numVertices)
		return NULL;

	if(s<0){
		return NULL;
	}

	if(vertices[s-RAW_WIDTH-1].y > 0.0)
		return  &vertices[s-RAW_WIDTH-1];

	if(vertices[s-RAW_WIDTH].y > 0.0)
		return  &vertices[s-RAW_WIDTH];

	if(vertices[s-RAW_WIDTH+1].y > 0.0)
		return  &vertices[s-RAW_WIDTH+1];

	if(vertices[s-1].y > 0.0)
		return  &vertices[s-1];

	if(vertices[s+1].y > 0.0)
		return  &vertices[s+1];

	if(vertices[s+RAW_WIDTH-1].y > 0.0)
		return  &vertices[s+RAW_WIDTH-1];

	if(vertices[s+RAW_WIDTH].y > 0.0)
		return  &vertices[s+RAW_WIDTH];

	if(vertices[s+RAW_WIDTH+1].y > 0.0)
		return  &vertices[s+RAW_WIDTH+1];

	return NULL;
}