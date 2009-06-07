#include "HeightMap.h"

HeightMap::HeightMap(void)
{
	int tam=MAP_SIZE*MAP_SIZE;
	pHeightMap=new byte[tam];
	genList();
}

HeightMap::~HeightMap(void)
{
	delete pHeightMap;
}
void HeightMap::setTransformation(CVector3 Min,CVector3 Max){
	pMin=Min;
	pMax=Max;
	
	dX=pMax.x-pMin.x;
	dY=pMax.y-pMin.y;
	dZ=pMax.z-pMin.z;
	genList();
}

void HeightMap::genList(){

		int X = 0, Y = 0;						// Create some variables to walk the array with.
	int x, y, z;							// Create some variables for readability
	float fColor = 0.0f;					// Create a variable to hold our color of the polygon
	if(!pHeightMap) return;					// Make sure our height data is valid

    glNewList(prim,GL_COMPILE);   // Normals
		glBegin( GL_QUADS );					// Render Quads
		CVector3 v;	
		for ( X = 0; X < MAP_SIZE; X += STEP_SIZE )
			for ( Y = 0; Y < MAP_SIZE; Y += STEP_SIZE )
			{
				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;							
				y = Height(X, Y );	
				z = Y;							
				v=hpToOb(CVector3(x,y,z));
				SetTextureCoord(x, z);
				SetNormals(x, z);
				
				glVertex3f(v.x, v.y, v.z);			// Send this vertex to OpenGL to be rendered (integer points are faster)

				// Get the (X, Y, Z) value for the top left vertex		
				x = X;										
				y = Height(X, Y + STEP_SIZE );  
				z = Y + STEP_SIZE ;							
				v=hpToOb(CVector3(x,y,z));


				// Set the color value of the current vertex
				//SetVertexColor( x, z);
				SetTextureCoord(x, z);
				SetNormals(x, z);
				glVertex3f(v.x, v.y, v.z);			// Send this vertex to OpenGL to be rendered

				// Get the (X, Y, Z) value for the top right vertex		
				x = X + STEP_SIZE; 
				y = Height(X + STEP_SIZE, Y + STEP_SIZE ); 
				z = Y + STEP_SIZE ;
				v=hpToOb(CVector3(x,y,z));


				// Set the color value of the current vertex
				//SetVertexColor(x, z);
				SetTextureCoord(x, z);
				SetNormals(x, z);
				glVertex3f(v.x, v.y, v.z);			// Send this vertex to OpenGL to be rendered

				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + STEP_SIZE; 
				y = Height(X + STEP_SIZE, Y ); 
				z = Y;
				v=hpToOb(CVector3(x,y,z));

				// Set the color value of the current vertice
				//SetVertexColor(x, z);
				SetTextureCoord(x, z);
				SetNormals(x, z);
				glVertex3f(v.x, v.y, v.z);			// Send this vertex to OpenGL to be rendered
			}
		glEnd();	
    glEndList();
}
CVector3 HeightMap::hpToOb(CVector3 a){
	float pX=a.x/float(MAP_SIZE);
	float pY=a.y/255.0f;
	float pZ=a.z/float(MAP_SIZE);

	a.x=pMin.x+dX*pX;
	a.y=pMin.y+dY*pY;
	a.z=pMin.z+dZ*pZ;


	return a;

}
void HeightMap::RenderHeightMap()
{
	glCallList(prim);
}

void HeightMap::SetVertexColor(int x, int y)
{
	if(!pHeightMap) return;					// Make sure our height data is valid
	float fColor = -0.15f + (Height(x, y ) / 256.0f);
	glColor3f(0, fColor, 0 );
}
float HeightMap::Height(float X, float Y)
{
	if(!pHeightMap) return -100;
	int xMin=int(X);
	int yMin=int(Y);
	int xMax=xMin;
	if(X-(float)xMin>0.0f) xMax++;
	int yMax=yMin;
	if(Y-(float)yMin>0.0f) yMax++;


	int xA = xMin % MAP_SIZE;					// Error check our x value
	int yA = yMin % MAP_SIZE;					// Error check our y value
	int xB = xMax % MAP_SIZE;					// Error check our x value
	int yB = yMax % MAP_SIZE;					// Error check our y value

	float h1=pHeightMap[xA + (yA * MAP_SIZE)];
	float h2=pHeightMap[xB + (yA * MAP_SIZE)];
	float h3=pHeightMap[xA + (yB * MAP_SIZE)];
	float h4=pHeightMap[xB + (yB * MAP_SIZE)];

	float p1=X-(float)xMin;
	float p2=1.0f-p1;
	float p3=Y-(float)yMin;
	float p4=1.0f-p3;

	return p2*p4*h1 + p1*p4*h2 + p3*p2*h3 + p1*p3*h4;	// Index into our height array and return the height
}

float HeightMap::Height2(float X, float Y)
{

	if(X<pMin.x || X>pMax.x || Y<pMin.z || Y>pMax.z) return -100;
	if(!pHeightMap) return -100;
	X+=-pMin.x;
	Y+=-pMin.z;
	float pX=fabs(X/float(dX));
	float pZ=fabs(Y/float(dZ));

	
	float pY=Height(pX*MAP_SIZE,pZ*MAP_SIZE)/255.0f;

	return pMin.y+dY*pY;



}

void HeightMap::LoadRawFile(const char* strName, int nSize,GLuint primL)
{
	FILE *pFile = NULL;
	prim=primL;

	// Let's open the file in Read/Binary mode.
	pFile = fopen( strName, "rb" );

	// Check to see if we found the file and could open it
	if ( pFile == NULL )	
	{
		// Display our error message and stop the function
		printf("Can't find the height map!");
		return;
	}

	// Here we load the .raw file into our pHeightMap data array.
	// We are only reading in '1', and the size is the (width * height)
	fread( pHeightMap, 1, nSize, pFile );

	// After we read the data, it's a good idea to check if everything read fine.
	int result = ferror( pFile );

	// Check if we received an error.
	if (result)
	{
		printf("Can't get data!");
	}

	// Close the file.
	fclose(pFile);
}
void HeightMap::SetTextureCoord(float x, float z)
{

	glTexCoord2f(   (float)x / (float)MAP_SIZE,	
				  - (float)z / (float)MAP_SIZE	);
}
void HeightMap::SetNormals(int x, int z)
{
	int s=STEP_SIZE;

	int x1=max(0,x-s);
	int x2=min(MAP_SIZE-1,x+s);
	int z1=max(0,z-s);
	int z2=min(MAP_SIZE-1,z+s);


	CVector3 v(x,Height(x,z),z);

	CVector3 v1(x1,Height(x1,z1),z1);
	CVector3 v2(x2,Height(x2,z2),z2);
	CVector3 v3(x1,Height(x1,z2),z2);
	CVector3 v4(x2,Height(x2,z1),z1);

	CVector3 N(0,0,0);

	CVector3 N1=CVector3::Normalize(CVector3::Cross(v1-v,v4-v));
	CVector3 N2=CVector3::Normalize(CVector3::Cross(v3-v,v1-v));
	CVector3 N3=CVector3::Normalize(CVector3::Cross(v2-v,v3-v));
	CVector3 N4=CVector3::Normalize(CVector3::Cross(v4-v,v2-v));

//	if(x>16 && z>16) 
//		printf("a");
	N=(N1+N2+N3+N4)/4.0f;
	N=CVector3::Normalize(N);
	glNormal3f(N.x,N.y,N.z);
	
}