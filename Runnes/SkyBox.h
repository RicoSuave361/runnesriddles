#pragma once
#include "headers.h"
#include <QGLWidget>
class SkyBox
{
public:
	SkyBox(QGLWidget *p);
public:
	~SkyBox(void);
	QGLWidget *parent;
	unsigned int g_TextureSkyBox[MAX_TEXTURES];		// This holds the texture info, referenced by an ID
	void CreateSkyBox(float x, float y, float z, float width, float height, float length);	//SkyBox
	void CreateSkyBoxTexture(const char* strFileName, int textureID);			//Create texture for skybox

};
