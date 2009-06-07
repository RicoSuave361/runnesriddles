#pragma once
#include "headers.h"

class HeightMap
{
public:
	HeightMap(void);
	
	byte *pHeightMap;	
	CVector3 pMin,pMax;
	unsigned int texture;
	float dX,dY,dZ;
	GLuint prim;

	// This returns the height (0 to 255) from a heightmap given an X and Y
	float Height(float X, float Y);
	float Height2(float X, float Y);
	void genList();
	// This loads a .raw file of a certain size from the file
	void LoadRawFile(const char* strName, int nSize,GLuint primL);

	// This turns heightmap data into primitives and draws them to the screen
	void RenderHeightMap();

	void SetVertexColor(int x, int y);
	void SetTextureCoord(float x, float z);
	void SetNormals(int x, int z);
	CVector3 hpToOb(CVector3 a);
	void setTransformation(CVector3 Min,CVector3 Max);
public:
	~HeightMap(void);
};
