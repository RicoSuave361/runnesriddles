#include "window.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <gl\glaux.h>


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
	lastX=0;
	lastY=0;

	// Inicializar carga de OBJs
	memset(g_Texture,0,sizeof(g_Texture));
	g_RotateX=0.0f;
	g_RotationSpeed=0.8f;
	camera.PositionCamera(	0 , 1.5f, 12,
							0 , 0.5f, 0,
							0 , 1   , 0);
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


void Window::initializeGL()
{
	//showFullScreen();
	setVSync(1);

	sky=new SkyBox(this);

	//Model 1
	g_LoadObj.ImportObj(&g_3DModel, "Models/mm.obj");							//Load Model
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/t2.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);

	//Model 2
	//g_LoadObj.ImportObj(&g_3DModel, "Models/mm.obj");
	g_LoadObj.AddMaterial(&g_3DModel, "text2", "Textures/texture1.bmp", 255, 255, 255);
	g_LoadObj.SetObjectMaterial(&g_3DModel, 1, 1);


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

	glEnable(GL_COLOR_MATERIAL);						// Allow color	
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnable(GL_CULL_FACE);								// Enables Backface Culling
	glCullFace(GL_BACK);

	
	//Audio
	audio.Play("Footsteps.wav");	//play audio cue
	if(playerController.IsConnected())
		printf("Player1 Controller XBOX is Connected...\n");
	else 
		printf("Player1 Controller XBOX not Connected...\n");
	setVisible(true);

	glewInit();//shader
	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else 
	{
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}
	initShader(); 
	//applyShader();	
	//for(int i=0; i<5; i++)
	//	glUniform1i(getUniLoc(p, QString(QString("activeLight[")+QString::number(i)+QString("]")).toAscii()), 0);
 

}

void Window::paintGL()
{ 

	unapplyShader();
	if(playerController.IsConnected())
	{
			//if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
			//{
			//	playerController.Vibrate(65535, 0);
			//}
			//DPAD Move
			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{
				camera.StrafeCamera(-0.8f);
			}

			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{
				camera.StrafeCamera(0.8f);
			}

			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				camera.MoveCamera(-0.8f);
			}

			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				camera.MoveCamera(0.8f);
			}

			//Analog Left Stick Move
			if(playerController.GetState().Gamepad.sThumbLX < -10000 || playerController.GetState().Gamepad.sThumbLX > 10000 || playerController.GetState().Gamepad.sThumbLY < -10000 || playerController.GetState().Gamepad.sThumbLY > 10000)
			{
				posX=(float)playerController.GetState().Gamepad.sThumbLX * 0.000027f;
				posZ=(float)playerController.GetState().Gamepad.sThumbLY * 0.000027f;
				
				camera.StrafeCamera(-posX);
				camera.MoveCamera(-posZ);
			}

			//Analog Right Stick Move
			if(playerController.GetState().Gamepad.sThumbRX < -10000 || playerController.GetState().Gamepad.sThumbRX > 10000 || playerController.GetState().Gamepad.sThumbRY < -10000 || playerController.GetState().Gamepad.sThumbRY > 10000)
			{
				viewX=(float)playerController.GetState().Gamepad.sThumbRX * 0.0006f;
				viewY=(float)playerController.GetState().Gamepad.sThumbRY * 0.0006f;
				camera.Rotate(-viewX, -viewY);
			}
	}


	//OpenGL Initialize
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Camara
	camera.Look();	
	glDisable(GL_LIGHTING);
	//Show SkyBox
	sky->CreateSkyBox(0, 0, 0, 400, 200, 400); //Setea el skybox

	applyShader();
    glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT1);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glUniform1i(getUniLoc(p, "activeLight[1]"),1);
	float white[]={1.0f,1.0f,1.0f,1.0f};
	float zero[]={0.0f,0.0f,0.0f,1.0f};
	float lightPos1[] = {49.0f, 1.50f, 38.0f, 1.0f};
	float fAmbiental1[] = {0.8f, 0.8f, 0.8f, 1.0f};
	float colorDiffuse1[] = {0.6f, 0.4f, 0.3f, 1.0f};
	float fSpecular2[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, fSpecular2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, colorDiffuse1);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fAmbiental1);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
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

			// Reset the color to normal again
//			glColor3ub(255, 255, 255);

			// Bind the texture map to the object by it's materialID (*ID Current Unused*)
			
			if(pObject->materialID >= 0 ){
				glUniform1i(getUniLoc(p, "text"), 1);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
				glUniform1i(getUniLoc(p, "texture"), 0);
			}else{
				
				glUniform1i(getUniLoc(p, "text"), 0);
				// Turn off texture mapping and turn on color
				glDisable(GL_TEXTURE_2D);

				// Reset the color to normal again
				glColor3ub(255, 255, 255);
			}
		} else {

			glUniform1i(getUniLoc(p, "text"), 0);
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
	unapplyShader();

	//FPS counter
	++fps;
	if(m_time.currentTime().second()!=sec && fps>0){
		ratio=double(fps)/double(abs(m_time.currentTime().second()-sec));
		sec=m_time.currentTime().second();
		setWindowTitle(QString("FPS: ")+QString::number(ratio));
		fps=0;
	}

	//Display FPS
	debugDisplay=QString("FPS: ")+QString::number(ratio)+
		QString(" Eye: ")+QString::number((double)camera.eye.x)+QString(" ")+QString::number((double)camera.eye.y)+QString(" ")+QString::number((double)camera.eye.z)+
		QString(" Center: ")+QString::number((double)camera.center.x)+QString(" ")+QString::number((double)camera.center.y)+QString(" ")+QString::number((double)camera.center.z)+
		//QString(" Up: ")+QString::number((double)camera.up.x)+QString(" ")+QString::number((double)camera.up.y)+QString(" ")+QString::number((double)camera.up.z) +
		QString(" GameTime: ")+QString::number(GAMETIME);
		
	renderText(10,10,debugDisplay);
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

	//printf("%d %d\n",mX,mY);
	camera.Rotate(lastX,-lastY);
	
}

void Window::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_Escape){
		close();
	}
	if(event->key()==Qt::Key_A){
		camera.StrafeCamera(1.8f);
	}
	if(event->key()==Qt::Key_S){
		camera.MoveCamera(1.8f);
	}
	if(event->key()==Qt::Key_D){
		camera.StrafeCamera(-1.8f);
	}
	if(event->key()==Qt::Key_W){
		camera.MoveCamera(-1.8f);
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

