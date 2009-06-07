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
	setCursor(Qt::BlankCursor);
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
	kL=false;
	kD=false;
	kR=false;
	kU=false;
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
    primitiveList  = glGenLists(1000);
	//showFullScreen();
	setVSync(1);
	g_bIgnoreFrustum = false;
	sky=new SkyBox(this);

	printf("Load Model...");
	//Model 1
	
	g_LoadObj.ImportObj(&g_3DModel, "Models/stair.obj");							//Load Model
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/stairTexture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);
/*
	//Model 2
	//g_LoadObj.ImportObj(&g_3DModel, "Models/mm.obj");
	g_LoadObj.AddMaterial(&g_3DModel, "text2", "Textures/texture1.bmp", 255, 255, 255);
	g_LoadObj.SetObjectMaterial(&g_3DModel, 1, 1);

*/
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
	printf(" End...\n");
	hp.LoadRawFile("Models/floorHeightmap.raw", MAP_SIZE * MAP_SIZE,primitiveList+999);
	QImage img("Textures/floorTexture.jpg");
	hp.texture=bindTexture(img, GL_TEXTURE_2D);

	escalera.LoadRawFile("Models/stairsHeightmap.raw", MAP_SIZE * MAP_SIZE,primitiveList+998);
	img=QImage("Textures/stairTexture.jpg");
	escalera.texture=bindTexture(img, GL_TEXTURE_2D);

	glEnable(GL_COLOR_MATERIAL);					// Allow color	
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glEnable(GL_CULL_FACE);								// Enables Backface Culling
	//glCullFace(GL_BACK);

	hp.setTransformation(CVector3(-100,-10,-100),CVector3(100,10,100));
	escalera.setTransformation(CVector3(-30,-12,-30),CVector3(30,50,30));
	
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
	
	//camera.PositionCamera( 280, 35, 225,  281, 35, 225,  0, 1, 0);
	initShader(); 
	//applyShader();	
	//for(int i=0; i<5; i++)
	//	glUniform1i(getUniLoc(p, QString(QString("activeLight[")+QString::number(i)+QString("]")).toAscii()), 0);
 
	sp=new SystemParticle(bindTexture(QImage("Textures/particle.bmp")));
}
void Window::drawObj(int ID){
	if(g_3DModel.pObject.size() <= ID) return;
	t3DObject *pObject = &g_3DModel.pObject[ID];
	
	if(!g_bIgnoreFrustum && !g_Frustum.SphereInFrustum(pObject->center.x,pObject->center.y,pObject->center.z, pObject->radio)) 
		return;
	
	nrObjectDraw++;
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	if(pObject->bHasTexture) {			
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
			glDisable(GL_TEXTURE_2D);
			glColor3ub(255, 255, 255);
		}
	} else {
		glUniform1i(getUniLoc(p, "text"), 0);
		glDisable(GL_TEXTURE_2D);
		glColor3ub(255, 255, 255);
	}
	glBegin(GL_TRIANGLES);
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				int vertIndex = pObject->pFaces[j].vertIndex[whichVertex];
				glNormal3f(pObject->pNormals[ vertIndex ].x, pObject->pNormals[ vertIndex ].y, pObject->pNormals[ vertIndex ].z);
				if(pObject->bHasTexture) {
					if(pObject->pTexVerts) {
						int coordIndex = pObject->pFaces[j].coordIndex[whichVertex];
						glTexCoord2f(pObject->pTexVerts[ coordIndex ].x, pObject->pTexVerts[ coordIndex ].y);
					}
				} else {
					if(g_3DModel.pMaterials.size() && pObject->materialID >= 0) 
					{
						BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;
						glColor3ub(pColor[0], pColor[1], pColor[2]);
					}	
				}
				glVertex3f(pObject->pVerts[ vertIndex ].x, pObject->pVerts[ vertIndex ].y, pObject->pVerts[ vertIndex ].z);
			}
		}
	glEnd();
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
	
	//numeros de objetos pintados
	nrObjectDraw = 0;

	// Calcular frustum
	g_Frustum.CalculateFrustum();

	// Calcular Posicion de heightMap
	CVector3 vPos		= camera.center;
	CVector3 vNewPos    = vPos;
	bool piso;
	float hPiso=hp.Height2(vPos.x, vPos.z );
	float hEscalera=escalera.Height2(vPos.x, vPos.z);	
	float h;
	if(fabs(vPos.y-hPiso)<fabs(vPos.y-hEscalera))
		h=hPiso;
	else 
		h=hEscalera;

	if(vPos.y < h + 5 || vPos.y > h + 5)
	{
		vNewPos.y = h + 10;
		float temp = vNewPos.y - vPos.y;
		CVector3 vView = camera.eye;
		vView.y += temp;
		camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z, vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	glUniform1i(getUniLoc(p, "text"), 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hp.texture);
	glUniform1i(getUniLoc(p, "texture"), 0);

	hp.RenderHeightMap();
	escalera.RenderHeightMap();


	//Draw OBJ
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		//break;
		drawObj(i);
	}

	unapplyShader();

	
		dead=GAMETIME/1000.0f;
		dt=dead-dt;
	sp->render(dt);
		dt=dead;

		glColor4ub(255,255,255,255);
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
		QString(" GameTime: ")+QString::number(GAMETIME)+QString(" NRO objetos pintados: ")+QString::number(nrObjectDraw);
		
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
	if(event->key()==Qt::Key_A || event->key()==Qt::Key_Left){
		kL=true;
	}
	if(event->key()==Qt::Key_S || event->key()==Qt::Key_Down){
		kD=true;
	}
	if(event->key()==Qt::Key_D || event->key()==Qt::Key_Right){
		kR=true;
	}
	if(event->key()==Qt::Key_W || event->key()==Qt::Key_Up){
		kU=true;
	}

	if(kL) camera.StrafeCamera(1.8f);
	if(kD) camera.MoveCamera(1.8f);
	if(kR) camera.StrafeCamera(-1.8f);
	if(kU) camera.MoveCamera(-1.8f);

	if(event->key()==Qt::Key_0)
	{
		if(this->isFullScreen())
		{
			showNormal();
			setCursor(Qt::BlankCursor);
			//setCursor(Qt::CrossCursor);
		}
		else
		{
			showFullScreen();
			setCursor(Qt::BlankCursor);	
		}
	}
}
void Window::keyReleaseEvent(QKeyEvent *event)
{

	if(event->key()==Qt::Key_A || event->key()==Qt::Key_Left){
		kL=false;
	}
	if(event->key()==Qt::Key_S || event->key()==Qt::Key_Down){
		kD=false;
	}
	if(event->key()==Qt::Key_D || event->key()==Qt::Key_Right){
		kR=false;
	}
	if(event->key()==Qt::Key_W || event->key()==Qt::Key_Up){
		kU=false;
	}

}
