#include "Octree.h"   

int g_CurrentSubdivision = 0; 

// The current amount of subdivisions we are currently at.   
// This is used to make sure we don't go over the max amount  

Octree::Octree()   
{     
    InitOctree();      
}   
   
Octree::~Octree()   
{   
    DestroyOctree();   
}   
    
void Octree::InitOctree()   
{   
    subDivided = false;   
    dimensions = 0;    
    objectCount = 0;   
    center = Vector3(0, 0, 0);   
	bodies = NULL;
	for(int i=0; i<8;i++)
	{
		childNodes[i] = NULL;
	}
}   
   
    
//This sets our initial width of the scene, as well as our center point   
void Octree::SetSceneDimensions(Vector3 minCorner, Vector3 maxCorner)   
{   
    // We pass in minCorner and maxcorner of scene and work out center point and width of the whole scene.  
	//We use this information to subdivide our octree.
	center = (minCorner + maxCorner) / 2;

    //We use the fabsf() function to get the floating point absolute value because it might return a negative number.   
    float currentWidth  = fabsf(maxCorner.x - center.x);     
    float currentHeight = fabsf(maxCorner.y - center.y);         
    float currentDepth  = fabsf(maxCorner.z - center.z);     
 
    // Otherwise, we just have half the size because we are calculating from the center of the scene.   
    currentWidth *= 2;      currentHeight *= 2;     currentDepth *= 2;   
   
    // Check if the width is the highest value and assign that for the cube dimension   
    if(currentWidth > currentHeight && currentWidth > currentDepth) {
        dimensions = currentWidth;   
	}
    else if(currentHeight > currentWidth && currentHeight > currentDepth) {
        dimensions = currentHeight;   
	}
    else {
        dimensions = currentDepth;   
	}
}   

//This returns the center point of the new subdivided node, depending on the ID     
Vector3 Octree::GetNewNodeCenter(Vector3 center, float width, int nodeID)  
{    
    Vector3 nodeCenter(0, 0, 0);   
    Vector3 tmpCenter = center;   
   
    //Switch on the ID to see which subdivided node we are finding the center   
    switch(nodeID)                             
    {   
        case TOP_LEFT_FRONT:   
            nodeCenter = Vector3(tmpCenter.x - width/4, tmpCenter.y + width/4, tmpCenter.z + width/4);   
            break;   
        case TOP_LEFT_BACK:   
            nodeCenter = Vector3(tmpCenter.x - width/4, tmpCenter.y + width/4, tmpCenter.z - width/4);   
            break;   
        case TOP_RIGHT_BACK:    
            nodeCenter = Vector3(tmpCenter.x + width/4, tmpCenter.y + width/4, tmpCenter.z - width/4);   
            break;   
        case TOP_RIGHT_FRONT:   
            nodeCenter = Vector3(tmpCenter.x + width/4, tmpCenter.y + width/4, tmpCenter.z + width/4);   
            break;   
        case BOTTOM_LEFT_FRONT:    
            nodeCenter = Vector3(tmpCenter.x - width/4, tmpCenter.y - width/4, tmpCenter.z + width/4);   
            break;   
        case BOTTOM_LEFT_BACK:   
            nodeCenter = Vector3(tmpCenter.x - width/4, tmpCenter.y - width/4, tmpCenter.z - width/4);   
            break;   
        case BOTTOM_RIGHT_BACK:    
            nodeCenter = Vector3(tmpCenter.x + width/4, tmpCenter.y - width/4, tmpCenter.z - width/4);   
            break;   
        case BOTTOM_RIGHT_FRONT:   
            nodeCenter = Vector3(tmpCenter.x + width/4, tmpCenter.y - width/4, tmpCenter.z + width/4);   
            break;   
    }   

    return nodeCenter;   
}   
   
//This figures out the new node information and then passes it into CreateNode()   
void Octree::CreateNewNode(RigidBody** bodies, std::vector<bool> pList, int numberOfObjects, Vector3 center, float dimension, int objCount, int nodeID)
{   
    // This function helps us set up the new node that is being created.  We only   
    // want to create a new node if it found objects in it's area.  If there were   
    // no objects found in this node's cube, then we ignore it and don't create a node.   
   
    // Check if the first node found some objects in it   
    //if(numberOfObjects)      
   // {   
		//std::vector<Vector3> *nodeObjects = new std::vector<Vector3>;   
		RigidBody* tmp[NUM_BODIES];
		int index = 0; 
		if(objCount)
		{
			// Allocate memory for the objects found in this node
			  
   
			// Go through all the objectsa and assign the objects to the node's list   
			for(int i = 0; i < numberOfObjects; i++)   
			{   
				// If this current triangle is in the node, assign it's vertices to it   
				if(pList[i])       
				{   
					tmp[index] = bodies[i]; 
					index++;   
				}   
			}   
		}
      
        // Allocate a new node for this octree     
        childNodes[nodeID] = new Octree;   
        // Get the new node's center point depending on the nodexIndex (which of the 8 subdivided cubes).   
        Vector3 nodeCenter = GetNewNodeCenter(center, dimension, nodeID);   
        // Increase the current level of subdivision   
        g_CurrentSubdivision++;   
   
        // Recurse through this node and subdivide it if necessary   
        childNodes[nodeID]->CreateNode(tmp, objCount, nodeCenter, dimension / 2);   
        g_CurrentSubdivision--;   
   
    //}   
}   
   
//This is our recursive function that goes through and subdivides our nodes      
void Octree::CreateNode(RigidBody** bodies, int numberOfObjects, Vector3 cent, float dim)  
{   
    center = cent;     
    dimensions = dim;   
    
    // Check if we have too many objects in this node and we haven't subdivided   
    // above our max subdivisions. Both must be true to divide this node.   
	if( (numberOfObjects > MAX_OBJECTS) && (g_CurrentSubdivision < MAX_DEPTH ) )   
    {   
        subDivided = true;   
   
        // Create a list for each new node to store if a object should be stored in it's   
        // object list.  For each index it will be a true or false to tell us if that object   
        // is in the cube of that node.  Below we check every point to see where it's   
        // position is from the center (I.E. if it's above the center, to the left and    
        // back it's the TOP_LEFT_BACK node)
   
        std::vector<bool> pList1(numberOfObjects);       // TOP_LEFT_FRONT node list   
        std::vector<bool> pList2(numberOfObjects);       // TOP_LEFT_BACK node list   
        std::vector<bool> pList3(numberOfObjects);       // TOP_RIGHT_BACK node list   
        std::vector<bool> pList4(numberOfObjects);       // TOP_RIGHT_FRONT node list   
        std::vector<bool> pList5(numberOfObjects);       // BOTTOM_LEFT_FRONT node list   
        std::vector<bool> pList6(numberOfObjects);       // BOTTOM_LEFT_BACK node list   
        std::vector<bool> pList7(numberOfObjects);       // BOTTOM_RIGHT_BACK node list   
        std::vector<bool> pList8(numberOfObjects);       // BOTTOM_RIGHT_FRONT node list   
       

        // Go through all of the objects and check which node they belong too.  The way   
        // we do this is use the center of our current node and check where the point   
        // lies in relationship to the center.  For instance, if the point is    
        // above, left and back from the center point it's the TOP_LEFT_BACK node.    
        for(int i = 0; i  < numberOfObjects; i++)   
        {   
            // Create some variables to cut down the thickness of the code
			Vector3 objPos = bodies[i]->GetPosition(); 
            //TOP LEFT FRONT node   
            if( (objPos.x <= cent.x) && (objPos.y >= cent.y) && (objPos.z <= cent.z) )   
                pList1[i] = true;   
		
            //TOP LEFT BACK node   
            if( (objPos.x <= cent.x) && (objPos.y >= cent.y) && (objPos.z >= cent.z) )  
                pList2[i] = true;   
		
            //TOP RIGHT BACK node   
            if( (objPos.x >= cent.x) && (objPos.y >= cent.y) && (objPos.z >= cent.z) )  
                pList3[i] = true;   
		
            //TOP RIGHT FRONT node   
            if( (objPos.x >= cent.x) && (objPos.y >= cent.y) && (objPos.z <= cent.z) )   
                pList4[i] = true;   
			
            //BOTTOM LEFT FRONT node   
            if( (objPos.x <= cent.x) && (objPos.y <= cent.y) && (objPos.z <= cent.z) )  
                pList5[i] = true;   
		
            //BOTTOM LEFT BACK node   
            if( (objPos.x <= cent.x) && (objPos.y <= cent.y) && (objPos.z >= cent.z) )  
                pList6[i] = true;   
			
            //BOTTOM RIGHT BACK node   
            if( (objPos.x >= cent.x) && (objPos.y <= cent.y) && (objPos.z >= cent.z) )  
                pList7[i] = true;   
		
            //BOTTOM RIGHT FRONT node   
            if( (objPos.x >= cent.x) && (objPos.y <= cent.y) && (objPos.z <= cent.z) )  
                pList8[i] = true; 
        }      
   
        // Here we create a variable for each list that holds how many objects   
        // were found for each of the 8 subdivided nodes.   
        int objCount1 = 0;  int objCount2 = 0;  int objCount3 = 0;  int objCount4 = 0;   
        int objCount5 = 0;  int objCount6 = 0;  int objCount7 = 0;  int objCount8 = 0;   
           
        // Go through each of the lists and increase the object count for each node.   
        for(int i = 0; i < numberOfObjects; i++)     
        {   
            // Increase the triangle count for each node that has a "true" for the index i.   
            if(pList1[i])   objCount1++;    if(pList2[i])   objCount2++;   
            if(pList3[i])   objCount3++;    if(pList4[i])   objCount4++;   
            if(pList5[i])   objCount5++;    if(pList6[i])   objCount6++;   
            if(pList7[i])   objCount7++;    if(pList8[i])   objCount8++;   
        }   
       
        // Create the subdivided nodes if necessary and then recurse through them.   
        // The information passed into CreateNewNode() are essential for creating the   
        // new nodes.  We pass the 8 ID's in so it knows how to calculate it's new center.   
        CreateNewNode(bodies, pList1, numberOfObjects, cent, dim, objCount1, TOP_LEFT_FRONT);
        CreateNewNode(bodies, pList2, numberOfObjects, cent, dim, objCount2, TOP_LEFT_BACK );   
		CreateNewNode(bodies, pList3, numberOfObjects, cent, dim, objCount3, TOP_RIGHT_FRONT);   
		CreateNewNode(bodies, pList4, numberOfObjects, cent, dim, objCount4, TOP_RIGHT_BACK);   
		CreateNewNode(bodies, pList5, numberOfObjects, cent, dim, objCount5, BOTTOM_LEFT_BACK);   
		CreateNewNode(bodies, pList6, numberOfObjects, cent, dim, objCount6, BOTTOM_LEFT_FRONT);   
		CreateNewNode(bodies, pList7, numberOfObjects, cent, dim, objCount7, BOTTOM_RIGHT_BACK );   
		CreateNewNode(bodies, pList8, numberOfObjects, cent, dim, objCount8, BOTTOM_RIGHT_FRONT);  
	}   
    else   
    {    
        // Assign the objects to this node since we reached the end node.   
        // This will be the end node that actually gets called to be drawn.   
        // We just pass in the objects and object count to be assigned to this node.   
        AssignObjectsToNode(bodies, numberOfObjects);   
    }   
}   

//This allocates memory for the vertices to assign to the current end node   
void Octree::AssignObjectsToNode(RigidBody** bods, int numberOfObj)   
{   
    subDivided = false;   
    // Initialize the objects count of this leafs 
    objectCount = numberOfObj;   
   
	if(objectCount)
	{
		bodies = new RigidBody*[numberOfObj];   
		for(unsigned int i =0; i < (unsigned)objectCount; i++)
		{
			bodies[i] = bods[i];
		}
	} 
}   
   

//This function recurses through all the nodes and draws the end node's vertices     
void Octree::DrawOctree(Octree *pNode, Frustum *frameFrustum)   
{   
	int count = 0;
    if(!pNode|| !frameFrustum ) {
		return;   
	}
	else if(pNode->IsSubDivided()) 
	{
		// Check if this node is subdivided. If so, then we need to recurse and draw it's nodes 
		DrawOctree(pNode->childNodes[TOP_LEFT_FRONT], frameFrustum );   
		DrawOctree(pNode->childNodes[TOP_LEFT_BACK], frameFrustum );   
		DrawOctree(pNode->childNodes[TOP_RIGHT_BACK], frameFrustum );   
		DrawOctree(pNode->childNodes[TOP_RIGHT_FRONT], frameFrustum );   
		DrawOctree(pNode->childNodes[BOTTOM_LEFT_FRONT], frameFrustum );   
		DrawOctree(pNode->childNodes[BOTTOM_LEFT_BACK], frameFrustum );   
		DrawOctree(pNode->childNodes[BOTTOM_RIGHT_BACK], frameFrustum );   
		DrawOctree(pNode->childNodes[BOTTOM_RIGHT_FRONT], frameFrustum );  
		  	   
	}
	else
	{
		//Leaf node! so test if this node in in view
		if(frameFrustum->CubeInFrustum(Vector3(pNode->center.x, pNode->center.y,pNode->center.z),pNode->dimensions /2))   
		{   
			// Make sure we have valid objects assigned to this node   
			if(!objectCount){
				//cout << "nothing\n";
				return; 
			}
			for(unsigned int i = 0; i < (unsigned)objectCount; i++)
			{
				count++;
				//cout << count << "\n";
			}
			return;
		}   
		else {   
			return;
		}
	}
}   

void Octree::DrawDebugCube(Octree *node)
{
	if(!node) 
	{
		return;   
	}
	else 
	{
		// Check if this node is subdivided. If so, then we need to recurse and draw it's nodes   
		if(node->IsSubDivided())   
		{   
			// Recurse to the bottom of these nodes and draw the end node's objects   
			// Like creating the octree, we need to recurse through each of the 8 nodes.   
			DrawDebugCube(node->childNodes[TOP_LEFT_FRONT]); 
			DrawDebugCube(node->childNodes[TOP_LEFT_BACK]);    
			DrawDebugCube(node->childNodes[TOP_RIGHT_BACK]);   
			DrawDebugCube(node->childNodes[TOP_RIGHT_FRONT]);   
			DrawDebugCube(node->childNodes[BOTTOM_LEFT_FRONT]);   
			DrawDebugCube(node->childNodes[BOTTOM_LEFT_BACK]);   
			DrawDebugCube(node->childNodes[BOTTOM_RIGHT_BACK]);   
			DrawDebugCube(node->childNodes[BOTTOM_RIGHT_FRONT]);    
		}
		else
		{
			
			DebugDraw3D::DrawWireCube3D(node->center, Vector3(node->dimensions,node->dimensions,node->dimensions), 0xff0000ff );
			
			//if(node->objectCount)
			//{
			//	for(unsigned int i = 0; i < node->objectCount; i++)
			//	{
			//		//object location
			//		DebugDraw3D::DrawWireCube3D(bodies[i]->GetPosition(), Vector3(10,10,10), 0xffff0000);
			//	}
			//}
			
			
		}	   
	}
}
  
void Octree::DestroyOctree()   
{   
    // Free the objects data if it's not NULL   
    if(objectCount)   
    {   
		delete[] bodies;   
    }   
   
	if(subDivided)
	{
		
		for(int i = 0; i < 8; i++)   
		{ 
			delete childNodes[i];  
			childNodes[i] = NULL;   
		}
	}		
}		

      
   