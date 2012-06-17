#include "Cloth.h"


Cloth::Cloth(int width, int height, int size,Vector3 corner,Vector3 corner2,Vector3 corner3,Vector3 corner4 ) {

	bounding = BoundingSphere(max((float)width,(float)height)*(float)size*1.2f/2,corner + Vector3(((float)width*(float)size)/2,((float)height*(float)size)/2,0));
	int springCounter = 0;
	numVertices = width * height;
	numIndices = (width - 1) * (height - 1) * 6;
	numSprings = 2*(width -1)*width;
	int i = 0;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	springs = new ClothSpring[numSprings];
	bodies = new RigidBody[numVertices];
	
	for (int x = 0; x < width; ++x)
	{
		for (int z = 0; z < height; ++z) 
		{
			int offset = (x * width) + z;

			vertices [offset] = Vector3((x * size)+corner.x, 0+corner.y, (z *  size)+corner.z);
			bodies[offset].reset(vertices[offset]);
			bodies[offset].SetTypeSphere(6);

			textureCoords[offset] = Vector2(x/(width-1.0f), z /(height-1.0f));//Vector2(x *0.5, z*0.5);
		}
	}

	numIndices = 0;

	for (int x = 0; x < width - 1; ++x) {
		for (int z = 0; z < height - 1; ++z) {
			int a = (x * (width)) + z;
			int b = ((x + 1) * (width)) + z;
			int c = ((x + 1) * (width)) + (z + 1);
			int d = (x * (width)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;

		}
	}
	for(unsigned int i =0;i<numVertices-1;i++){
		if((i+1)%width !=0){
			springs[springCounter].initialize(1000.0f,(RigidBody*)&bodies[i],(RigidBody*)&bodies[i+1],(float)size);
			springCounter++;
		}
	}
	for(unsigned int i =0;i<numVertices-width;i++){
		springs[springCounter].initialize(1000.0f,(RigidBody*)&bodies[i],(RigidBody*)&bodies[i+width],(float)size);
		springCounter++;
	}
	staticSprings[0] = Spring(1000,bodies[0].GetPosition(),(RigidBody*)&bodies[0],0);
	staticSprings[1] = Spring(1000,corner2,(RigidBody*)&bodies[width-1],0);
	staticSprings[2] = Spring(1000,corner3,(RigidBody*)&bodies[width*(height-1)+1],0);
	staticSprings[3] = Spring(1000,corner4,(RigidBody*)&bodies[numVertices-1],0);
	GenerateNormals();
	GenerateTangents();
	BufferData();
}
void Cloth::update(float msec){
	bounding.updatePosition(bodies[numVertices-1].GetPosition()+((bodies[0].GetPosition()-bodies[numVertices-1].GetPosition())/2));
	if(msec > 18)msec = 18;
	for(int i=0;i<numSprings;i++){
		springs[i].update();
	}
	for(unsigned int i =0;i<numVertices;i++){
		bodies[i].AddForce(Vector3(0,-9.8f,0)*bodies[i].GetMass());
		bodies[i].Integrate(msec/1000);
		vertices[i] = bodies[i].GetPosition();
	}
	for(int i = 0;i<4;i++){
		staticSprings[i].update();
	}
	GenerateNormals();
	GenerateTangents();
	RebufferData();
}

void Cloth::AddForce(Vector3 force){
	for(unsigned int i = 0; i<numVertices;i++){
		bodies[i].AddForce(force);
	}
}

void Cloth::FusRoDah(Vector3 cam){
	for(unsigned int i = 0; i<numVertices;i++){
		bodies[i].AddForce(((bodies[i].GetPosition()-cam)*50000)/(bodies[i].GetPosition()-cam).Length());
	}
}

void Cloth::RebufferData(){
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)vertices);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector2), (void*)textureCoords);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(Vector3), (void*)normals);
}