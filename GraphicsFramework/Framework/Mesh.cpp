#include "Mesh.h"

Mesh::Mesh()
{
	for (int i = 0; i < MAX_BUFFER; ++i)
	{
		bufferObject[i] = 0;
	}

	glGenVertexArrays(1, &arrayObject);

	vertices = NULL;
	colours = NULL;
	textureCoords = NULL;
	indices = NULL;
	normals = NULL;
	tangents = NULL;

	numVertices = 0;
	numIndices = 0;
	texture = 0;
	texture2 = 0;
	texture3 = 0;
	bumpTexture = 0;
	bumpTexture2 = 0;
	bumpTexture3 = 0;

	type = GL_TRIANGLES;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteVertexArrays(MAX_BUFFER, bufferObject);

	delete [] vertices;
	delete [] colours;
	delete [] textureCoords;
	delete [] indices;
	delete [] normals;
	delete [] tangents;	

	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &texture2);
	glDeleteTextures(1, &texture3);
	glDeleteTextures(1, &bumpTexture);
	glDeleteTextures(1, &bumpTexture2);
	glDeleteTextures(1, &bumpTexture3);
}

Mesh * Mesh::GenerateTriangle()
{
	Mesh *m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh * Mesh::GenerateQuad()
{
    Mesh* m = new Mesh();

    m->numVertices = 4;
    m->type = GL_TRIANGLE_STRIP;

    m->vertices	= new Vector3[m->numVertices];
    m->textureCoords = new Vector2[m->numVertices];
    m->colours = new Vector4[m->numVertices];
    m->normals = new Vector3[m->numVertices];
    m->tangents = new Vector3[m->numVertices];

    m->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
    m->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
    m->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
    m->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

    m->textureCoords[0] = Vector2(0.0f, 1.0f);
    m->textureCoords[1] = Vector2(0.0f, 0.0f);
    m->textureCoords[2] = Vector2(1.0f, 1.0f);
    m->textureCoords[3] = Vector2(1.0f, 0.0f);

    for(int i = 0; i < 4; ++i)
	{
        m->colours[i] = Vector4(1.0f, 1.0f,1.0f,1.0f);
        m->normals[i] = Vector3(0.0f, 0.0f,-1.0f);
        m->tangents[i] = Vector3(1.0f, 0.0f,0.0f);
    }

    m->BufferData();

    return m;
}

Mesh* Mesh::GenerateQuadWithColour(Vector4 colours[4])
{
	Mesh* m = new Mesh();

    m->numVertices = 4;
    m->type = GL_TRIANGLE_STRIP;

    m->vertices	= new Vector3[m->numVertices];
    m->textureCoords = new Vector2[m->numVertices];
    m->colours = new Vector4[m->numVertices];
    m->normals = new Vector3[m->numVertices];
    m->tangents = new Vector3[m->numVertices];

    m->vertices[0] = Vector3(0.0f, 0.0f, 0.0f);
    m->vertices[1] = Vector3(0.0f, 1.0f, 0.0f);
    m->vertices[2] = Vector3(1.0f, 0.0f, 0.0f);
    m->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

    for(int i = 0; i < 4; ++i)
	{
        m->colours[i] = colours[i];
        m->normals[i] = Vector3(0.0f, 0.0f,-1.0f);
        m->tangents[i] = Vector3(1.0f, 0.0f,0.0f);
    }

    m->BufferData();

    return m;
}

Mesh * Mesh::GenerateCrystal()
{
 	Mesh *m = new Mesh();
	m->numVertices = 24;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(-0.5f, 0.0f, 0.5f);
	m->vertices[2] = Vector3(0.5f, 0.0f, 0.5f);
	
	m->vertices[3] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[4] = Vector3(0.5f, 0.0f, 0.5f);
	m->vertices[5] = Vector3(0.5f, 0.0f, -0.5f);
	
	m->vertices[6] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[7] = Vector3(0.5f, 0.0f, -0.5f);
	m->vertices[8] = Vector3(-0.5f, 0.0f, -0.5f);
	
	m->vertices[9] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[10] = Vector3(-0.5f, 0.0f, -0.5f);
	m->vertices[11] = Vector3(-0.5f, 0.0f, 0.5f);
	
	m->vertices[12] = Vector3(0.0f, -0.5f, 0.0f);
	m->vertices[13] = Vector3(0.5f, 0.0f, 0.5f);
	m->vertices[14] = Vector3(-0.5f, 0.0f, 0.5f);
	
	m->vertices[15] = Vector3(0.0f, -0.5f, 0.0f);
	m->vertices[16] = Vector3(0.5f, 0.0f, -0.5f);
	m->vertices[17] = Vector3(0.5f, 0.0f, 0.5f);
	
	m->vertices[18] = Vector3(0.0f, -0.5f, 0.0f);
	m->vertices[19] = Vector3(-0.5f, 0.0f, -0.5f);
	m->vertices[20] = Vector3(0.5f, 0.0f, -0.5f);
	
	m->vertices[21] = Vector3(0.0f, -0.5f, 0.0f);
	m->vertices[22] = Vector3(-0.5f, 0.0f, 0.5f);
	m->vertices[23] = Vector3(-0.5f, 0.0f, -0.5f);

	m->colours = new Vector4[m->numVertices];

	for(unsigned int i = 0; i < m->numVertices; i+=3)
	{
		m->colours[i] = Vector4(1.0f, 0.5f, 0.5f, 0.8f);
		m->colours[i + 1] = Vector4(0.5f, 1.0f, 0.5f, 0.8f);
		m->colours[i + 2] = Vector4(0.5f, 0.5f, 1.0f, 0.8f);
	}

	m->GenerateNormals();
	m->GenerateTangents();

	m->BufferData();

	return m;
}

Mesh* Mesh::GeneratePyramid()
{
	Mesh *m = new Mesh();
	m->type = GL_TRIANGLES;
	m->numVertices = 30;

	m->vertices = new Vector3[m->numVertices];
	
	// side 1 - front

	m->vertices[0] = Vector3(-1.0f, 0.0f, 1.0f);
	m->vertices[1] = Vector3(1.0f, 0.0f, 1.0f);
	m->vertices[2] = Vector3(0.5f, 1.0f, 0.5f);

	m->vertices[3] = Vector3(-1.0f, 0.0f, 1.0f);
	m->vertices[4] = Vector3(0.5f, 1.0f, 0.5f);
	m->vertices[5] = Vector3(-0.5f, 1.0f, 0.5f);


	// side 2 - left side

	m->vertices[6] = Vector3(-1.0f, 0.0f, -1.0f);
	m->vertices[7] = Vector3(-1.0f, 0.0f, 1.0f);
	m->vertices[8] = Vector3(-0.5f, 1.0f, 0.5f);

	m->vertices[9] = Vector3(-1.0f, 0.0f, -1.0f);
	m->vertices[10] = Vector3(-0.5f, 1.0f, 0.5f);
	m->vertices[11] = Vector3(-0.5f, 1.0f, -0.5f);

	// side 3 - back side

	m->vertices[12] = Vector3(1.0f, 0.0f, -1.0f);
	m->vertices[13] = Vector3(-1.0f, 0.0f, -1.0f);
	m->vertices[14] = Vector3(-0.5f, 1.0f, -0.5f);

	m->vertices[15] = Vector3(1.0f, 0.0f, -1.0f);
	m->vertices[16] = Vector3(-0.5f, 1.0f, -0.5f);
	m->vertices[17] = Vector3(0.5f, 1.0f, -0.5f);

	// side 4 - right side

	m->vertices[18] = Vector3(1.0f, 0.0f, 1.0f);
	m->vertices[19] = Vector3(1.0f, 0.0f, -1.0f);
	m->vertices[20] = Vector3(0.5f, 1.0f, -0.5f);

	m->vertices[21] = Vector3(1.0f, 0.0f, 1.0f);
	m->vertices[22] = Vector3(0.5f, 1.0f, -0.5f);
	m->vertices[23] = Vector3(0.5f, 1.0f, 0.5f);

	// top

	m->vertices[24] = Vector3(-0.5f, 1.0f, 0.5f);
	m->vertices[25] = Vector3(0.5f, 1.0f, 0.5f);
	m->vertices[26] = Vector3(0.5f, 1.0f, -0.5f);

	m->vertices[27] = Vector3(-0.5f, 1.0f, 0.5f);
	m->vertices[28] = Vector3(0.5f, 1.0f, -0.5f);
	m->vertices[29] = Vector3(-0.5f, 1.0f, -0.5f);


	m->textureCoords = new Vector2[m->numVertices];

	m->textureCoords[0] = Vector2(0.0f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 0.0f);
	m->textureCoords[2] = Vector2(0.75f, 1.0f);

	m->textureCoords[3] = Vector2(0.0f, 0.0f);
	m->textureCoords[4] = Vector2(0.75f, 1.0f);
	m->textureCoords[5] = Vector2(0.25f, 1.0f);

	m->textureCoords[6] = Vector2(0.0f, 0.0f);
	m->textureCoords[7] = Vector2(1.0f, 0.0f);
	m->textureCoords[8] = Vector2(0.75f, 1.0f);

	m->textureCoords[9] = Vector2(0.0f, 0.0f);
	m->textureCoords[10] = Vector2(0.75f, 1.0f);
	m->textureCoords[11] = Vector2(0.25f, 1.0f);

	m->textureCoords[12] = Vector2(0.0f, 0.0f);
	m->textureCoords[13] = Vector2(1.0f, 0.0f);
	m->textureCoords[14] = Vector2(0.75f, 1.0f);

	m->textureCoords[15] = Vector2(0.0f, 0.0f);
	m->textureCoords[16] = Vector2(0.75f, 1.0f);
	m->textureCoords[17] = Vector2(0.25f, 1.0f);

	m->textureCoords[18] = Vector2(0.0f, 0.0f);
	m->textureCoords[19] = Vector2(1.0f, 0.0f);
	m->textureCoords[20] = Vector2(0.75f, 1.0f);

	m->textureCoords[21] = Vector2(0.0f, 0.0f);
	m->textureCoords[22] = Vector2(0.75f, 1.0f);
	m->textureCoords[23] = Vector2(0.25f, 1.0f);

	m->textureCoords[24] = Vector2(0.25f, 0.0f);
	m->textureCoords[25] = Vector2(0.75f, 0.0f);
	m->textureCoords[26] = Vector2(0.75f, 1.0f);

	m->textureCoords[27] = Vector2(0.25f, 0.0f);
	m->textureCoords[28] = Vector2(0.75f, 1.0f);
	m->textureCoords[29] = Vector2(0.25f, 1.0f);

	m->colours = new Vector4[m->numVertices];

	for(unsigned int i = 0; i < m->numVertices; i++)
	{
		m->colours[0] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	m->GenerateNormals();
	m->GenerateTangents();

	m->BufferData();

	return m;	
}

void Mesh::BufferData () {
	glBindVertexArray(arrayObject);
	glGenBuffers(1 , &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER , bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER , numVertices * sizeof (Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (textureCoords) { // This bit is new !
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if (colours) { // Just in case the data has no colour attribute ...
		glGenBuffers(1, & bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer (COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}

	if (indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	}
	if(normals){
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector3),
			normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE,0,0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}
	if(tangents){
		glGenBuffers(1,&bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector3),
			tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER,3,GL_FLOAT,GL_FALSE,0,0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}
	glBindVertexArray(0);
}

void Mesh::RebufferData()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)vertices);

	if (textureCoords) 
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vector2), (void*)textureCoords);
	}

	if(colours)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector4), (void*)colours);
	}

	if(normals)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)normals);
	}
	if(tangents)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)tangents);
	}
}

void Mesh::Draw()
{
	// Texture on texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Bumpmap on texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumpTexture);

	// Second texture on texture unit 11
	if (texture2)
	{
		glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_2D, texture2);
	}

	// Third texture on texture unit 12
	if (texture3)
	{
		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, texture3);
	}

	// Second bump texture on texture unit 13
	if (bumpTexture2)
	{
		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_2D, bumpTexture2);
	}

	// Third bump texture on texture unit 14
	if (bumpTexture3)
	{
		glActiveTexture(GL_TEXTURE14);
		glBindTexture(GL_TEXTURE_2D, bumpTexture3);
	}

	glBindVertexArray(arrayObject);
	
	if (bufferObject[INDEX_BUFFER])
	{
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(type, 0, numVertices);
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::GenerateNormals()
{
	if (!normals)
	{
		normals = new Vector3[numVertices];
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i] = Vector3();
	}

	// Generate per-vertex normals
	if (indices)
	{
		for (GLuint i = 0; i < numIndices; i+=3)
		{
			unsigned int a = indices[i];
			unsigned int b = indices[i+1];
			unsigned int c = indices[i+2];

			Vector3 normal = Vector3::Cross((vertices[b] - vertices[a]), 
											(vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	}
	else // It's just a list of triangles, so generate face normals
	{
		for (GLuint i = 0; i < numVertices; i+=3)
		{
			Vector3 &a = vertices[i];
			Vector3 &b = vertices[i+1];
			Vector3 &c = vertices[i+2];

			Vector3 normal = Vector3::Cross(b - a, c - a);

			normals[i] = normal;
			normals[i+1] = normal;
			normals[i+2] = normal;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i].Normalise();
	}
}

void Mesh::GenerateTangents()
{
	if(!textureCoords)
	{
		return;
	}

	if (!tangents)
	{
		tangents = new Vector3[numVertices];
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i] = Vector3();
	}

	if (indices)
	{
		for (GLuint i = 0; i < numIndices; i+=3)
		{
			int a = indices [i];
			int b = indices [i+1];
			int c = indices [i+2];

			Vector3 tangent = GenerateTangent(vertices[a], vertices[b], vertices[c],
											  textureCoords[a], textureCoords[b], textureCoords[c]);

			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;
		}
	}
	else
	{
		for (GLuint i = 0; i < numVertices; i +=3)
		{
			Vector3 tangent = GenerateTangent(vertices[i], vertices[i+1], vertices[i+2], 
											  textureCoords[i], textureCoords[i+1], textureCoords[i+2]);

			tangents[i] += tangent;
			tangents[i+1] += tangent;
			tangents[i+2] += tangent;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i].Normalise();
	}
}

Vector3 Mesh::GenerateTangent(const Vector3 &a, const Vector3 &b, const Vector3 &c, 
							  const Vector2 &ta, const Vector2 &tb, const Vector2 &tc)
{
	Vector2 coord1 = tb - ta;
	Vector2 coord2 = tc - ta;

	Vector3 vertex1 = b - a;
	Vector3 vertex2 = c - a;

	Vector3 axis = Vector3(vertex1 * coord2.y - vertex2 * coord1.y);

	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}

void Mesh::generateFurTex(int idensity, float windSpeed, float length){
	furWind = 0;
	windS = windSpeed;
	fLength = length;
	static UINT seed = 23234;
	srand(seed);
	generateFurTex(idensity, furTexL1);
	srand(seed);
	generateFurTex(idensity/10, furTexL2);
	srand(seed);
	generateFurTex(idensity/100, furTexL3);
	srand((unsigned int)(time(NULL)));
}

void Mesh::generateFurTex(int idensity, GLuint& tex){
	UINT size = 1024*1024*4;

	char *data = new char[size];
	for (UINT i = 0; i < size; i++)
	{
		data[i] = 0;
	}

	for(UINT density=0; density< (unsigned)idensity; density++)
	{
		int xrand = rand() % 1023;
		int yrand = rand() % 1023;

		data[xrand*4 + (yrand * 1024*4)] = rand()%185;//normal variance y;
		data[(xrand*4 + (yrand * 1024*4))+1] = rand()%100;//colour variance
		data[(xrand*4 + (yrand * 1024*4))+2] = rand()%185;//normal variance x
		data[(xrand*4 + (yrand * 1024*4))+3] = 255;

		//data[(xrand*4 + (yrand * 512*4))+4] = rand()%185;//normal variance x
		//data[(xrand*4 + (yrand * 512*4))+5] = rand()%100;
		//data[(xrand*4 + (yrand * 512*4))+6] = rand()%185;//normal variance y;
		//data[(xrand*4 + (yrand * 512*4))+7] = 255;

		//data[xrand*4 + (yrand * 512*4)+512] = rand()%185;//normal variance x;;
		//data[(xrand*4 + (yrand * 512*4))+513] = rand()%100;
		//data[(xrand*4 + (yrand * 512*4))+514] = rand()%185;//normal variance y;;
		//data[(xrand*4 + (yrand * 512*4))+515] = 255;
		//data[(xrand*4 + (yrand * 512*4))+516] = rand()%185;//normal variance x;;
		//data[(xrand*4 + (yrand * 512*4))+517] = rand()%100;
		//data[(xrand*4 + (yrand * 512*4))+518] = rand()%185;//normal variance y;;
		//data[(xrand*4 + (yrand * 512*4))+519] = 255;
	}
	glGenTextures(1,&tex);
	
	//repeat texture
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, true ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, true ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture (GL_TEXTURE4);
	glBindTexture (GL_TEXTURE_2D , tex);

	glTexImage2D ( GL_TEXTURE_2D , 0 , GL_RGBA ,
		1024 , 1024 , 0 ,  GL_RGBA ,  GL_UNSIGNED_BYTE , data );

	glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -10);
	glGenerateMipmap ( GL_TEXTURE_2D );
	/*glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER, GL_NEAREST );*/
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&fLargest);
	glTexParameterf ( GL_TEXTURE_2D ,GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );
	glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST );
	glBindTexture ( GL_TEXTURE_2D , 0);

}

void Mesh::drawFur(GLuint shader){
	GL_BREAKPOINT
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(int i =0;i<12;i++){
		float layer = (float)i / 12.0f;
		float length = fLength * layer;
		glUniform1f(glGetUniformLocation(shader, "layer"),	layer);
		glUniform1f(glGetUniformLocation(shader, "flength"),	length);
		glUniform1f(glGetUniformLocation(shader, "wind"),	furWind);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE4);
		if(i <13){
			glBindTexture(GL_TEXTURE_2D, furTexL1);
		}
		else if(i <16){
			glBindTexture(GL_TEXTURE_2D, furTexL2);
		}
		else{
			glBindTexture(GL_TEXTURE_2D, furTexL3);
		}

		glUniform1i(glGetUniformLocation(shader,"furTex"),4);
		glUniform1i(glGetUniformLocation(shader,"diffuseTex"),0);

		glBindVertexArray(arrayObject);
	
		if (bufferObject[INDEX_BUFFER]) {
			glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawArrays(type, 0, numVertices);
		}
		glBindVertexArray(0);
	}
	furWind+= windS;
	glDisable(GL_BLEND);
}