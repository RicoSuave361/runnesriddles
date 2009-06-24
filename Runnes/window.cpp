#include "window.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include "include\glaux.h"

//#define DIS_SHADER

Window::Window(QWidget *parent) : QGLWidget(parent),wglSwapIntervalEXT(0)
{
	// Inicializar Widget
	//setCursor(Qt::CrossCursor);							//Setea el cursor de mouse como una cruz
	setCursor(Qt::BlankCursor);
	setMinimumSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	setGeometry(50,50,SCREEN_WIDTH, SCREEN_HEIGHT);

	memset(blur_s,0,sizeof(blur_s));
pas=0;
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
	camera.PositionCamera(	-220 , 22, 6,
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
	gluPerspective(45.0f, (float)width/(float)height, 0.1f, 10000.0f);
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
	
	g_LoadObj.ImportObj(&g_3DModel, "Models/plane.obj");							//Load Model
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/planeTexture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);

	g_LoadObj.ImportObj(&g_3DModel, "Models/tower1.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/tower1Texture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 1,1);

	g_LoadObj.ImportObj(&g_3DModel, "Models/tower2.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/tower2Texture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 2,2);

	g_LoadObj.ImportObj(&g_3DModel, "Models/tower3.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/tower3Texture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 3,3);

	g_LoadObj.ImportObj(&g_3DModel, "Models/tower4.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/tower4Texture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 4,4);

	g_LoadObj.ImportObj(&g_3DModel, "Models/castle.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/castleTexture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 5,5);

	g_LoadObj.ImportObj(&g_3DModel, "Models/room.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/roomTexture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 6, 6);

	g_LoadObj.ImportObj(&g_3DModel, "Models/tunnel.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/tunnelTexture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 7, 7);

	g_LoadObj.ImportObj(&g_3DModel, "Models/door.obj");							//Load 
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/doorTexture.jpg", 255, 255, 255);	//Load model's texture
	g_LoadObj.SetObjectMaterial(&g_3DModel, 8, 8);
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

	hp.LoadRawFile("Textures/planeHeightMap.raw", MAP_SIZE * MAP_SIZE,primitiveList+999);
	hp.texture=bindTexture(QImage("Textures/planeTexture.jpg"), GL_TEXTURE_2D);
/*
	escalera.LoadRawFile("Models/stairsHeightmap.raw", MAP_SIZE * MAP_SIZE,primitiveList+998);
	escalera.texture=bindTexture(QImage("Textures/stairTexture.jpg"), GL_TEXTURE_2D);
*/
	glEnable(GL_COLOR_MATERIAL);					// Allow color	
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glEnable(GL_CULL_FACE);								// Enables Backface Culling
	//glCullFace(GL_BACK);
	CVector3 c1 = g_3DModel.pObject[0].Min + CVector3(-20, 30, 0);
	CVector3 c2 = g_3DModel.pObject[0].Max + CVector3(-20, 30, 0);
	hp.setTransformation(c1 , c2);
	//escalera.setTransformation(CVector3(-30,-12,-30),CVector3(30,50,30));
	
	//Audio
	//audio.Play("Footsteps.wav");	//play audio cue


	if(playerController.IsConnected())
		printf("Player1 Controller XBOX is Connected...\n");
	else 
		printf("Player1 Controller XBOX not Connected...\n");
	setVisible(true);

	#ifndef DIS_SHADER
		glewInit();//shader
		if (glewIsSupported("GL_VERSION_2_0"))
			printf("Ready for OpenGL 2.0\n");
		else 
		{
			printf("OpenGL 2.0 not supported\n");
			//exit(1);
		}
	#endif
	//camera.PositionCamera( 280, 35, 225,  281, 35, 225,  0, 1, 0);

	#ifndef DIS_SHADER
		initShader("./glsl/phong.vert","./glsl/phong.frag",p); 
		initShader("./glsl/morph.vert","./glsl/morph.frag",p2); 
		initShader("./glsl/normalMap.vert","./glsl/normalMap.frag",normalMap); 
	#endif
	//applyShader();	
	//for(int i=0; i<5; i++)
	//	glUniform1i(getUniLoc(p, QString(QString("activeLight[")+QString::number(i)+QString("]")).toAscii()), 0);
 
//	sp=new SystemParticle(bindTexture(QImage("Textures/particle.bmp")));
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
			#ifndef DIS_SHADER
				glUniform1i(getUniLoc(p2, "text"), 1);
				glActiveTexture(GL_TEXTURE0);
				glUniform1i(getUniLoc(p2, "texture"), 0);
			#endif
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glEnable(GL_TEXTURE_2D);
		}else{
			#ifndef DIS_SHADER
				glUniform1i(getUniLoc(p2, "text"), 0);
			#endif
			glDisable(GL_TEXTURE_2D);
			glColor3ub(255, 255, 255);
		}
	} else {
		#ifndef DIS_SHADER
			glUniform1i(getUniLoc(p2, "text"), 0);
		#endif
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

//	unapplyShader();
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
	sky->CreateSkyBox(0, 0, 0, 4096, 4096, 4096); //Setea el skybox

	#ifndef DIS_SHADER
		applyShader(p);
		glUniform1i(getUniLoc(p, "activeLight[1]"),1);
	#endif

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT1);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	float white[]={1.0f,1.0f,1.0f,1.0f};
	float zero[]={0.0f,0.0f,0.0f,1.0f};
	float lightPos1[] = {49.0f, 1.50f, 38.0f, 1.0f};
	float fAmbiental1[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float fAmbiental2[] = {0.6f, 0.6f, 0.6f, 1.0f};
	float colorDiffuse1[] = {0.6f, 0.4f, 0.3f, 1.0f};
	float fSpecular2[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT1, GL_AMBIENT, fAmbiental2);
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
	


	float h=hp.Height2(vPos.x, vPos.z );
	//float hEscalera=escalera.Height2(vPos.x, vPos.z);	
	//float h;
	//if(fabs(vPos.y-hPiso)<fabs(vPos.y-hEscalera))
	//	h=hPiso;
	//else 
	//	h=hEscalera;

	if(vPos.y < h + 30 || vPos.y > h + 30)
	{
		
		blur_s[pas]=h;
		pas=(pas+1)%BLUR_STEP;
		prom=0.0f;
		for(int i=0;i<BLUR_STEP;++i) prom+=blur_s[i];
		prom/=float(BLUR_STEP);
		h=prom;
		vNewPos.y = h + 30;
		float temp = vNewPos.y - vPos.y;
		CVector3 vView = camera.eye;
		vView.y += temp;
		camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z, vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	#ifndef DIS_SHADER
		glUniform1i(getUniLoc(p, "text"), 1);
		glUniform1i(getUniLoc(p, "texture"), 0);
		glUniform1f(getUniLoc(p, "time"), GAMETIME/1000.0f);
		glActiveTexture(GL_TEXTURE0);
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, hp.texture);
	hp.RenderHeightMap();
	/*
	glBindTexture(GL_TEXTURE_2D, escalera.texture);
	escalera.RenderHeightMap();*/


	//Draw OBJ
	for(int i = 1; i < g_3DModel.numOfObjects; i++)
	{
		//break;
		drawObj(i);
	}

	#ifndef DIS_SHADER
		unapplyShader();
		//applyShader(p2);
		//glUniform1f(getUniLoc(p2, "time"), GAMETIME/1000.0f);
	#endif


	/* for(float i=-5.0f;i<5.0f;i+=1.0f){
		for(float j=-5.0f;j<5.0f;j+=1.0f){
			glPushMatrix();
			glTranslatef(i,0.0f,j);
			drawObj(g_3DModel.numOfObjects-1);
			glPopMatrix();
		}
	} */
/*
	unapplyShader();
		dead=GAMETIME/1000.0f;
		dt=dead-dt;
	sp->render(dt);  
		dt=dead;
*/

	glColor3f(1.0f,1.0f,1.0f);
	
	//Panel On Screen
	orthogonalStart();

		glBegin(GL_QUADS);
			glVertex2f(500, 500);
			glVertex2f(500, 375);
			glVertex2f(375, 375);
			glVertex2f(375, 500);
		glEnd();

	orthogonalEnd();

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
void Window::orthogonalStart (void) {
	GLint iViewport[4];

	// Get a copy of the viewport
	glGetIntegerv( GL_VIEWPORT, iViewport );

	// Save a copy of the projection matrix so that we can restore it 
	// when it's time to do 3D rendering again.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	// Set up the orthographic projection
	glOrtho( iViewport[0], iViewport[0]+iViewport[2],
			 iViewport[1]+iViewport[3], iViewport[1], -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	// Make sure depth testing and lighting are disabled for 2D rendering until
	// we are finished rendering in 2D
	glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

}
void Window::orthogonalEnd (void) {
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

}