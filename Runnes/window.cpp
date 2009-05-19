#include "window.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <gl\glaux.h>

UINT g_Texture[MAX_TEXTURES] = {0};						// This holds the texture info, referenced by an ID
UINT g_TextureSkyBox[MAX_TEXTURES] = {0};						// This holds the texture info, referenced by an ID

#define BACK_ID		0								// The texture ID for the back side of the cube
#define FRONT_ID	1									// The texture ID for the front side of the cube
#define BOTTOM_ID	2									// The texture ID for the bottom side of the cube
#define TOP_ID		3									// The texture ID for the top side of the cube
#define LEFT_ID		4									// The texture ID for the left side of the cube
#define RIGHT_ID	5									// The texture ID for the right side of the cube


Window::Window(QWidget *parent) : QGLWidget(parent),wglSwapIntervalEXT(0)
{
	// Inicializar Widget
	//setCursor(Qt::CrossCursor);							//Setea el cursor de mouse como una cruz
	setCursor(QCursor(QPixmap("textures/transparent.png")));//Setea el cursor de mouse como transparente
	setMinimumSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	setGeometry(50,50,SCREEN_WIDTH, SCREEN_HEIGHT);

	// Auto llamadas cada 1ms al updateGL
	m_timer = new QTimer(this);
    m_timer->setInterval(1);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    m_timer->start();

	// Inicializar Tiempo para FPS
	m_time=QTime::currentTime();
	sec=0;
	fps=0;

	//Inicializar camara
	camera.eye = CVector3(0.0f,7.0f,15.0f);
	camera.up = CVector3(0.0f,1.0f,0.0f);
	camera.center = CVector3(0.0f,5.5f,0.0f);
	lastX=0;
	lastY=0;

	// Inicializar carga de OBJs
	memset(g_Texture,0,sizeof(g_Texture));
	g_RotateX=0.0f;
	g_RotationSpeed=0.8f;
}

Window::~Window()
{
}

void Window::resizeGL(int width, int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)width/(float)height, 0.1f, 1000.0f);
}

void Window::CreateSkyBoxTexture(UINT textureArray[], LPSTR strFileName, int textureID)
{
	QImage img(strFileName);			// Load the bitmap and store the data

	g_TextureSkyBox[textureID]=bindTexture(img, GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void Window::CreateSkyBox(float x, float y, float z, float width, float height, float length)
{
	// Bind the BACK texture of the sky map to the BACK side of the cube
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[BACK_ID]);

	// This centers the sky box around (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the BACK Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// Bind the FRONT texture of the sky map to the FRONT side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[FRONT_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);	
	
		// Assign the texture coordinates and vertices for the FRONT Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[BOTTOM_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
	
		// Assign the texture coordinates and vertices for the BOTTOM Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// Bind the TOP texture of the sky map to the TOP side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[TOP_ID]);
	
	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the TOP Side
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// Bind the LEFT texture of the sky map to the LEFT side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[LEFT_ID]);
	
	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the LEFT Side
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// Bind the RIGHT texture of the sky map to the RIGHT side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[RIGHT_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

		// Assign the texture coordinates and vertices for the RIGHT Side
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}


void Window::initializeGL()
{
	//showFullScreen();
	setVSync(1);

	//Skybox
	CreateSkyBoxTexture(g_TextureSkyBox, "Textures/SkyBox/Back.bmp",	BACK_ID	 );
	CreateSkyBoxTexture(g_TextureSkyBox, "Textures/SkyBox/Front.bmp",	FRONT_ID );
	CreateSkyBoxTexture(g_TextureSkyBox, "Textures/SkyBox/Bottom.bmp",	BOTTOM_ID);
	CreateSkyBoxTexture(g_TextureSkyBox, "Textures/SkyBox/Top.bmp",		TOP_ID	 );
	CreateSkyBoxTexture(g_TextureSkyBox, "Textures/SkyBox/Left.bmp",	LEFT_ID	 );
	CreateSkyBoxTexture(g_TextureSkyBox, "Textures/SkyBox/Right.bmp",	RIGHT_ID );

	//Model 1
	g_LoadObj.ImportObj(&g_3DModel, "Models/Box.obj");							//Load Model
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/bone.bmp", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);

	//Model 2
	//g_LoadObj.ImportObj(&g_3DModel, "Models/mm.obj");
	//g_LoadObj.AddMaterial(&g_3DModel, "text2", "Textures/t2.jpg", 255, 255, 255);
	//g_LoadObj.SetObjectMaterial(&g_3DModel, 1, 1);


	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// Check if the current material has a file name
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0) 
		{
			QImage img(g_3DModel.pMaterials[i].strFile);
			g_Texture[i]=bindTexture(img, GL_TEXTURE_2D);
		}
		
		// Assign the material ID to the current material
		g_3DModel.pMaterials[i].texureId = i;
	}

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color	
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnable(GL_CULL_FACE);								// Enables Backface Culling
	glCullFace(GL_BACK);

	//glEnable(GL_NORMALIZE);
	//glPolygonOffset(1.0f,0.0);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	/*glEnable (GL_FOG);
	float c[3]={0.0f,0.0f,0.0f};
	glFogi (GL_FOG_MODE, GL_EXP2); 
	glFogfv (GL_FOG_COLOR, c); 
	glFogf(GL_FOG_START,1.0);               
	glFogf(GL_FOG_END,50.0);                  
	glHint (GL_FOG_HINT, GL_NICEST);
	glFogf (GL_FOG_DENSITY,  0.3f);*/

	//Audio
	audio.Play("Footsteps.wav");	//play audio cue

}

void Window::paintGL()
{ 
	//Display FPS
	debugDisplay=QString("FPS: ")+QString::number(ratio)+QString(" Eye: ")+QString::number((double)camera.eye.x)+QString(" ")+QString::number((double)camera.eye.y)+QString(" ")+QString::number((double)camera.eye.z)+QString(" Center: ")+QString::number((double)camera.center.x)+QString(" ")+QString::number((double)camera.center.y)+QString(" ")+QString::number((double)camera.center.z)+QString(" Up: ")+QString::number((double)camera.up.x)+QString(" ")+QString::number((double)camera.up.y)+QString(" ")+QString::number((double)camera.up.z);
	renderText(10,10,debugDisplay);

	//FPS counter
	++fps;
	if(m_time.currentTime().second()!=sec && fps>0){
		ratio=double(fps)/double(abs(m_time.currentTime().second()-sec));
		sec=m_time.currentTime().second();
		setWindowTitle(QString("FPS: ")+QString::number(ratio));
		fps=0;
	}

	//OpenGL Initialize
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z, camera.center.x, camera.center.y, camera.center.z, camera.up.x, camera.up.y, camera.up.z);	
	
	CreateSkyBox(0, 0, 0, 400, 200, 400); //Setea el skybox

	//glRotatef(g_RotateX, 0, 1.0f, 0);						// Rotate the object around the Y-Axis
	//g_RotateX = (m_time.msecsTo(QTime::currentTime())/10)*g_RotationSpeed;							// Increase the speed of rotation
    
	//glScalef(5.3f,5.3f,5.3f);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if(g_3DModel.pObject.size() <= 0) break;

		// Get the current object that we are displaying
		t3DObject *pObject = &g_3DModel.pObject[i];

		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture) {

			// Turn on texture mapping and turn off color
			glEnable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);

			// Bind the texture map to the object by it's materialID (*ID Current Unused*)
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} else {

			// Turn off texture mapping and turn on color
			glDisable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);
		}

		// This determines if we are in wire frame or normal mode
		glBegin(GL_TRIANGLES);					// Begin drawing with our selected mode (triangles or lines)

			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Go through each corner of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the vertex index for each point of the face
					int vertIndex = pObject->pFaces[j].vertIndex[whichVertex];
			
					// Give OpenGL the normal for this vertex.
					glNormal3f(pObject->pNormals[ vertIndex ].x, pObject->pNormals[ vertIndex ].y, pObject->pNormals[ vertIndex ].z);
				
					// If the object has a texture associated with it, give it a texture coordinate.
					if(pObject->bHasTexture) {

						// Make sure there was a UVW map applied to the object or else it won't have tex coords.
						if(pObject->pTexVerts) {

							// Get the texture coordinate index
							int coordIndex = pObject->pFaces[j].coordIndex[whichVertex];

							// Assign the UV coordinates to the current vertex being rendered
							glTexCoord2f(pObject->pTexVerts[ coordIndex ].x, pObject->pTexVerts[ coordIndex ].y);
						}
					} else {

						// Make sure there is a valid material/color assigned to this object.
						// You should always at least assign a material color to an object, 
						// but just in case we want to check the size of the material list.
						// if the size is at least one, and the material ID != -1,
						// then we have a valid material.
						if(g_3DModel.pMaterials.size() && pObject->materialID >= 0) 
						{
							// Get and set the color that the object is, since it must not have a texture
							BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;

							// Assign the current color to this model
							glColor3ub(pColor[0], pColor[1], pColor[2]);
						}
						
					}

					// Pass in the current vertex of the object (Corner of current face)
					glVertex3f(pObject->pVerts[ vertIndex ].x, pObject->pVerts[ vertIndex ].y, pObject->pVerts[ vertIndex ].z);
				}
			}

		glEnd();								// End the drawing
	}
}
void Window::mousePressEvent(QMouseEvent *event)
{	
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
	mX=event->x();
	mY=event->y();

	QCursor::setPos(width()/2 + geometry().left(), height()/2 + geometry().top());
	
	lastX=width()/2 /* + geometry().left() */- mX;
	lastY=height()/2/* + geometry().top() */- mY;
	
	if(lastX==0 && lastY==0) 
		return; 

	printf("%d %d\n",lastX,lastY);
	camera.Rotate(lastX,-lastY);
	
	//camera.Rotate(lastX*8,0);
	//camera.Rotate(0,lastY*8);

	//lastX=event->x();
	//lastY=event->y();
}

void Window::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_Escape){
		close();
	}
	if(event->key()==Qt::Key_A){
		camera.StrafeCamera(0.5f);
	}
	if(event->key()==Qt::Key_S){
		camera.MoveCamera(0.5f);
	}
	if(event->key()==Qt::Key_D){
		camera.StrafeCamera(-0.5f);
	}
	if(event->key()==Qt::Key_W){
		camera.MoveCamera(-0.5f);
	}


	if(event->key()==Qt::Key_0)
	{
		if(this->isFullScreen())
		{
			showNormal();
			setCursor(QCursor(QPixmap("textures/transparent.png"))); //Mouse transparente
			//setCursor(Qt::CrossCursor);
		}
		else
		{
			showFullScreen();
			setCursor(QCursor(QPixmap("textures/transparent.png"))); //Mouse transparente
		}
	}
}

