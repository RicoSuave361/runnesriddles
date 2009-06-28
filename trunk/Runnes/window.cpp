#include "window.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include "include\glaux.h"

#define DIS_SHADER

Window::Window(QWidget *parent) : QGLWidget(parent),wglSwapIntervalEXT(0)
{
	// Inicializar Widget
	//setCursor(Qt::CrossCursor);							//Setea el cursor de mouse como una cruz
	setCursor(Qt::BlankCursor);
	setMinimumSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	setGeometry(50,50,SCREEN_WIDTH, SCREEN_HEIGHT);
angCof=0;
	memset(blur_s,0,sizeof(blur_s));
pas=0;
dk=0.0f;
	 dl=0.0f;
	 dk2=0.0f;
	 dl2=0.0f;
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
	cofre=-1;
	door=-1;
	angCof=0.0f;
	angDoor=0.0f;
	memset(objetos,false,sizeof(objetos));
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

bool Window::isColliding(t3DObject A, t3DObject B){

	/*If the max x position of A is less than the min x position of B they do not collide
	* If the min x position of A is greater than the max x position of B they do not collide
	* and the same goes for y and z
	*/
	for(int i=0;i<B.numOfFaces;i++){
		CVector3 N=B.pNormalsFaces[i];
		CVector3 q=A.center;
		int vertIndex = B.pFaces[i].vertIndex[0];
		CVector3 p=B.pVerts[vertIndex];
					
		if(N.x*q.x + N.y*q.y + N.z*q.z - (N.x*p.x + N.y*p.y+N.z*p.z) <= 0 ) return false;
	}
	return true;/*

	if( A.Max.x < B.Min.x ) 		return false;

	if( A.Min.x > B.Max.x ) 		return false;

	if( A.Max.y < B.Min.y ) 		return false;

	if( A.Min.y > B.Max.y ) 		return false;

	if( A.Max.z < B.Min.z ) 		return false;

	if( A.Min.z > B.Max.z ) 		return false;

	return true;
*/
}

void Window::initializeGL()
{
	primitiveList  = glGenLists(1000);
	//showFullScreen();
	setVSync(1);
	g_bIgnoreFrustum = false;


	printf("Load Model...");

	//Texturas del Panel On Screen
	panel[0]=bindTexture(QImage("Textures/gem1.jpg"), GL_TEXTURE_2D);
	panel[1]=bindTexture(QImage("Textures/gem2.jpg"), GL_TEXTURE_2D);
	panel[2]=bindTexture(QImage("Textures/gem3.jpg"), GL_TEXTURE_2D);
	panel[3]=bindTexture(QImage("Textures/gem4.jpg"), GL_TEXTURE_2D);
	panel[4]=bindTexture(QImage("Textures/gem5.jpg"), GL_TEXTURE_2D);
	panel[5]=bindTexture(QImage("Textures/key.jpg"), GL_TEXTURE_2D);

	//Models
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairL.obj");
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairR.obj");

	piso1=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/plane.obj",		bindTexture(QImage("Textures/planeTexture.jpg"), GL_TEXTURE_2D));

	/*g_LoadObj.ImportObj(&g_3DModel, "Models/tower1.obj",	bindTexture(QImage("Textures/tower1Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tower2.obj",	bindTexture(QImage("Textures/tower2Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tower4.obj",	bindTexture(QImage("Textures/tower3Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tower3.obj",	bindTexture(QImage("Textures/tower4Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/castle.obj",	bindTexture(QImage("Textures/castleTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/room.obj",		bindTexture(QImage("Textures/roomTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tunnel.obj",	bindTexture(QImage("Textures/tunnelTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/key.obj",		bindTexture(QImage("Textures/keyTexture.jpg"), GL_TEXTURE_2D));
	//g_LoadObj.ImportObj(&g_3DModel, "Models/trees.obj",		bindTexture(QImage("Textures/treesTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/checker.obj",	bindTexture(QImage("Textures/checkerTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/table.obj",		bindTexture(QImage("Textures/tableTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/lamp.obj",		bindTexture(QImage("Textures/lampTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairL.obj",		bindTexture(QImage("Textures/stairLTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairR.obj",		bindTexture(QImage("Textures/stairRTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/indoorFloor.obj",		bindTexture(QImage("Textures/indoorFloorTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/chest.obj",		bindTexture(QImage("Textures/chestTexture.jpg"), GL_TEXTURE_2D));
	*/
	cofre=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/chestTop.obj",	bindTexture(QImage("Textures/chestTopTexture.jpg"), GL_TEXTURE_2D));

	door=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/door.obj",		bindTexture(QImage("Textures/doorTexture.jpg"), GL_TEXTURE_2D));

	/*g_LoadObj.ImportObj(&g_3DModel, "Models/rune1.obj",		bindTexture(QImage("Textures/rune1Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune2.obj",		bindTexture(QImage("Textures/rune2Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune3.obj",		bindTexture(QImage("Textures/rune3Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune4.obj",		bindTexture(QImage("Textures/rune4Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune5.obj",		bindTexture(QImage("Textures/rune5Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/planeStart.obj",bindTexture(QImage("Textures/planeStartTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem1.obj",		bindTexture(QImage("Textures/gem1Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem2.obj",		bindTexture(QImage("Textures/gem2Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem3.obj",		bindTexture(QImage("Textures/gem3Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem4.obj",		bindTexture(QImage("Textures/gem4Texture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem5.obj",		bindTexture(QImage("Textures/gem5Texture.jpg"), GL_TEXTURE_2D));
*/
	initCol=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/colision.obj");
	printf(" End...\n");

	glEnable(GL_COLOR_MATERIAL);					// Allow color	
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glEnable(GL_CULL_FACE);								// Enables Backface Culling
	//glCullFace(GL_BACK);

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
	#endif;

	#ifndef DIS_SHADER
		initShader("./glsl/phong.vert","./glsl/phong.frag",p); 
		initShader("./glsl/morph.vert","./glsl/morph.frag",p2); 
		initShader("./glsl/normalMap.vert","./glsl/normalMap.frag",normalMap);
		//applyShader();	
		//for(int i=0; i<5; i++)
		//	glUniform1i(getUniLoc(p, QString(QString("activeLight[")+QString::number(i)+QString("]")).toAscii()), 0);
	#endif

//	sp=new SystemParticle(bindTexture(QImage("Textures/particle.bmp")));
	
	sky=new SkyBox(this);

	g_BlurRate = 50;
	g_Viewport = 512;
	CreateRenderTexture(g_Texture2, 512, 3, GL_RGB, 0);
	float dist=0.0f;
	
	sonidos.push_back(new Audio(1,"ambienceMusic.wav"));
	sonidos.push_back(new Audio(2,"doorLock.wav"));
	sonidos.push_back(new Audio(3,"doorOpen.wav"));

}
void Window::drawObj(int ID){
	if(g_3DModel.pObject.size() <= ID) return;
	t3DObject *pObject = &g_3DModel.pObject[ID];
	
	if(!g_bIgnoreFrustum && !g_Frustum.SphereInFrustum(pObject->center.x,pObject->center.y,pObject->center.z, pObject->radio)) 
		return;
	
	nrObjectDraw++;
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	if(pObject->bHasTexture) {			
		if(pObject->materialID > 0 ){
			glEnable(GL_TEXTURE_2D);
			#ifndef DIS_SHADER
				if(pObject->normalID!=-1){
					glActiveTexture(GL_TEXTURE1);
					glUniform1i(getUniLoc(normalMap, "normalMap"), 1);
					glBindTexture(GL_TEXTURE_2D, pObject->normalID);
					glActiveTexture(GL_TEXTURE0);
					glEnable(GL_TEXTURE_2D);
					glUniform1i(getUniLoc(normalMap, "colorMap"), 0);
				}else{
					glUniform1i(getUniLoc(p2, "text"), 1);
					glActiveTexture(GL_TEXTURE0);
					glUniform1i(getUniLoc(p2, "texture"), 0);
				}
			#endif
			glBindTexture(GL_TEXTURE_2D, pObject->materialID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}else{
			#ifndef DIS_SHADER
				glUniform1i(getUniLoc(p2, "text"), 0);
			#endif
			glDisable(GL_TEXTURE_2D);
		}
	} else {
		#ifndef DIS_SHADER
			glUniform1i(getUniLoc(p2, "text"), 0);
		#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(ID<initCol){
		if(ID==cofre){
			if(CVector3::Distance(pObject->center,camera.center)<=70){
				intClose=false;
				if(!intOpen){
					intOpen=true;
					tFI=float(GAMETIME)/1000.0f;
				}
				if(angCof<90){
					angCof+=(float(GAMETIME)/1000.0f-tFI) * 20.0f;
					tFI=float(GAMETIME)/1000.0f;
				}
			}else{
				intOpen = false;
				if(!intClose){
					intClose=true;
					tFI=float(GAMETIME)/1000.0f;
				}
				if(angCof>0){
					angCof-=(float(GAMETIME)/1000.0f-tFI) * 20.0f;
					if(angCof<0)angCof=0;
					tFI=float(GAMETIME)/1000.0f;
					
				}
			}
			
			glTranslatef(pObject->center.x,pObject->Min.y,pObject->Max.z);
			glRotatef(angCof,1.0f,0.0f,0.0f);
			glTranslatef(-pObject->center.x,-pObject->Min.y,-pObject->Max.z);
		}
		if(ID==door){
			if(CVector3::Distance(pObject->center,camera.center)<=100){
				intCloseDoor=false;
				if(!intOpenDoor){
					intOpenDoor=true;
					tFIDoor=float(GAMETIME)/1000.0f;
				}
				if(angDoor<70){
					angDoor+=(float(GAMETIME)/1000.0f-tFIDoor) * 50.0f;
					tFIDoor=float(GAMETIME)/1000.0f;
				}
			}else{
				intOpenDoor = false;
				if(!intCloseDoor){
					intCloseDoor=true;
					tFIDoor=float(GAMETIME)/1000.0f;
				}
				if(angDoor>0){
					angDoor-=(float(GAMETIME)/1000.0f-tFIDoor) * 50.0f;
					if(angDoor<0)angDoor=0;
					tFIDoor=float(GAMETIME)/1000.0f;
					
				}
			}
			
			glTranslatef(pObject->center.x,pObject->center.y,pObject->Max.z);
			glRotatef(angDoor,0.0f,1.0f,0.0f);
			glTranslatef(-pObject->center.x,-pObject->center.y,-pObject->Max.z);
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
							#ifndef DIS_SHADER
								glMultiTexCoord2f(GL_TEXTURE0,pObject->pTexVerts[ coordIndex ].x, pObject->pTexVerts[ coordIndex ].y);
								glMultiTexCoord4f(GL_TEXTURE1,pObject->pTang[vertIndex].x,pObject->pTang[vertIndex].y,pObject->pTang[vertIndex].z,pObject->pTang[vertIndex].w);
							#endif
						}
					} 
					glVertex3f(pObject->pVerts[ vertIndex ].x, pObject->pVerts[ vertIndex ].y, pObject->pVerts[ vertIndex ].z);
				}
			}
		glEnd();
	}/*else{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		if(!isColliding(camera.box,*pObject))
			glColor3ub(255, 255, 255);
		else
			glColor3ub(255, 0, 0);
		glBegin(GL_LINES);
		for(int j = 0; j < pObject->numOfVerts; j++)
		{	
			for(int i = 0; i < pObject->numOfVerts; i++)
			{
				if(i==j) continue;
					glVertex3f(pObject->pVerts[ j ].x, pObject->pVerts[ j ].y, pObject->pVerts[ j ].z);
					glVertex3f(pObject->pVerts[ i ].x, pObject->pVerts[ i ].y, pObject->pVerts[ i ].z);
			
			}
		}
	
		glEnd();
		
		/*glColor3ub(255, 0, 0);
		CVector3 v1=pObject->min;
		CVector3 v2=pObject->max;
		glBegin(GL_LINES);

			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v2.x, v2.y, v2.z);


		glEnd();* /
	}*/
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
	
	// Camara
	sky->CreateSkyBox(0, 0, 0, 4096, 4096, 4096);
	

	// Calcular Posicion de heightMap
	CVector3 vPos		= camera.center;
	CVector3 vNewPos    = vPos;
	float dist=100000.0f;
	float ht=100000.0f;
	int c=g_3DModel.pObject[piso1].numOfVerts;
	for(int i=0;i<c;++i){
		CVector3  ver=g_3DModel.pObject[piso1].pVerts[i];
		float dT=sqrt( (ver.x-vPos.x)*(ver.x-vPos.x) + (ver.z-vPos.z)*(ver.z-vPos.z)  + (ver.y-vPos.y)*(ver.y-vPos.y) );
		if(dT<dist){
			ht=ver.y;
			dist=dT;
		}
	}

	float h=ht;//hp.Height2(vPos.x, vPos.z );
	//float hEscalera=escalera.Height2(vPos.x, vPos.z);	
	//float h;
	//if(fabs(vPos.y-hPiso)<fabs(vPos.y-hEscalera))
	//	h=hPiso;
	//else 
	//	h=hEscalera;

	if(vPos.y < h + 60 || vPos.y > h + 60)
	{
		
		blur_s[pas]=h;
		pas=(pas+1)%BLUR_STEP;
		prom=0.0f;
		for(int i=0;i<BLUR_STEP;++i) prom+=blur_s[i];
		prom/=float(BLUR_STEP);
		h=prom;
		vNewPos.y = h + 60;
		float temp = vNewPos.y - vPos.y;
		CVector3 vView = camera.eye;
		vView.y += temp;
		camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z, vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	repaint();



	//Panel On Screen
	#ifndef DIS_SHADER
		unapplyShader();
	#endif

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	orthogonalStart();
	int tU=0;
	for(int i=width()/3;i<=(width()*2)/3;i+=width()/15){

		//if(objetos[tU]){
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE);
			glDepthMask(false);
			glPushMatrix();
			//glColor4ub(0,0,0,255);
			glBindTexture(GL_TEXTURE_2D, panel[tU]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2f(i,height()-5);
				glTexCoord2f(0,1);
				glVertex2f(i, height()-width()/15 -10);
				glTexCoord2f(1,1);
				glVertex2f(i+width()/15-5, height()-width()/15 -10);
				glTexCoord2f(1,0);
				glVertex2f(i+width()/15-5, height()-5);
			glEnd();
		//}
		tU++;	
		glPopMatrix();
		glDepthMask(true); // Put the Z-buffer back into it's normal "Z-read and Z-write" state
		glDisable(GL_BLEND);
	}
	orthogonalEnd();
	

	glColor3f(1.0f,1.0f,1.0f);
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
		
	//Abrir cofre
	
	

	debugDisplay+=QString(" dist: ")+QString::number(CVector3::Distance(g_3DModel.pObject[cofre].center,camera.center))+QString(" ")+QString::number((double)g_3DModel.pObject[cofre].center.y)+QString(" ")+QString::number((double)g_3DModel.pObject[cofre].center.x)+QString(" ")+QString::number((double)g_3DModel.pObject[cofre].center.z);
	renderText(10,10,debugDisplay);

}

void Window::repaint()
{

	

	#ifndef DIS_SHADER
		applyShader(p);
		glUniform1i(getUniLoc(p, "activeLight[1]"),1);
	#endif

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);	
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


	#ifndef DIS_SHADER
		glUniform1i(getUniLoc(p, "text"), 1);
		glUniform1i(getUniLoc(p, "texture"), 0);
		glUniform1f(getUniLoc(p, "time"), GAMETIME/1000.0f);
		glActiveTexture(GL_TEXTURE0);
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	

	//Draw OBJ
	for(int i = piso1; i < g_3DModel.numOfObjects; i++)
	{
		glPushMatrix();
			if(i>=1 && i<=6)
			{
				if(!objetos[i-1]) drawObj(i);
			}
			else
			{
				//break;
				if(i==15){
					
					#ifndef DIS_SHADER
						unapplyShader();
						applyShader(normalMap);
					#endif

				//	glTranslatef(-220,22,6);
				}

				drawObj(i);
			}
		glPopMatrix();
	}

	#ifndef DIS_SHADER
		unapplyShader();
		//applyShader(p2);
		//glUniform1f(getUniLoc(p2, "time"), GAMETIME/1000.0f);
	#endif


	// Camara
	//sky->CreateSkyBox(0, 0, 0, 4096, 4096, 4096);
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
	if(event->key()==Qt::Key_1){
		sonidos[0]->Play();
	}
	if(event->key()==Qt::Key_2){
		sonidos[1]->Play();
	}
	if(event->key()==Qt::Key_3){
		sonidos[2]->Play();
	}

	
	if(kL)	{ dk=1.8; dk2=-2.0f; }
	else	{ dk=0.0f; dk2=0.0f; }//camera.StrafeCamera(1.8f);

	if(kD)	{ dl=1.8f; dl2=-2.0f; }
	else	{ dl=0.0f; dl2=0.0f;  } //camera.MoveCamera(1.8f);

	if(kR)	{ dk=-1.8f; dk2=2.0f; }
	else if(!kL)	{ dk=0.0f;  dk2=0.0f; }//camera.StrafeCamera(-1.8f);

	if(kU)	{ dl=-1.8f; dl2=2.0f; }//camera.MoveCamera(-1.8f);
	else if(!kD)	{ dl=0.0f;  dl2=0.0f; }

	

	if(dl!=0.0f || dk!=0.0f){
		camera.StrafeCamera(dk);
		camera.MoveCamera(dl);
		for(int i=initCol;i<g_3DModel.numOfObjects; i++)
			if(isColliding(camera.box,g_3DModel.pObject[i])){
				camera.StrafeCamera(dk2);
				camera.MoveCamera(dl2);
				//break;
			}
	}

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

void Window::OrthoMode(int left, int top, int right, int bottom)
{
	// Switch to our projection matrix so that we can change it to ortho mode, not perspective.
	glMatrixMode(GL_PROJECTION);						

	// Push on a new matrix so that we can just pop it off to go back to perspective mode
	glPushMatrix();									
	
	// Reset the current matrix to our identify matrix
	glLoadIdentity();								

	//Pass in our 2D ortho screen coordinates like so (left, right, bottom, top).  The last
	// 2 parameters are the near and far planes.
	glOrtho( left, right, bottom, top, 0, 1 );	
	
	// Switch to model view so that we can render the scope image
	glMatrixMode(GL_MODELVIEW);								

	// Initialize the current model view matrix with the identity matrix
	glLoadIdentity();										
}


///////////////////////////////// PERSPECTIVE MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function changes our returns our projection mode from 2D to 3D
/////
///////////////////////////////// PERSPECTIVE MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Window::PerspectiveMode()										// Set Up A Perspective View
{
	// Enter into our projection matrix mode
	glMatrixMode( GL_PROJECTION );							

	// Pop off the last matrix pushed on when in projection mode (Get rid of ortho mode)
	glPopMatrix();											

	// Go back to our model view matrix like normal
	glMatrixMode( GL_MODELVIEW );							

	// We should be in the normal 3D perspective mode now
}

void Window::CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID)										
{
	// Create a pointer to store the blank image data
	unsigned int *pTexture = NULL;											

	// Allocate and init memory for the image array and point to it from pTexture
	pTexture = new unsigned int [size * size * channels];
	memset(pTexture, 0, size * size * channels * sizeof(unsigned int));	

	// Register the texture with OpenGL and bind it to the texture ID
	glGenTextures(1, &textureArray[textureID]);								
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);					
	
	// Create the texture and store it on the video card
	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_INT, pTexture);						
	
	// Set the texture quality
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Since we stored the texture space with OpenGL, we can delete the image data
	delete [] pTexture;																					
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns true only the "desiredFrameRate" times a second
/////
///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool Window::AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = 0.0f;
	float elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
	float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
	if( elapsedTime > (1.0f / desiredFrameRate) )
	{
		// Reset the last time
		lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// RENDER MOTION BLUR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function takes a texture ID to blend over the screen for motion blur
/////
///////////////////////////////// RENDER MOTION BLUR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	
void Window::RenderMotionBlur(int textureID2)
{
	// This function was created to blend the rendered texture over top of the screen
	// to create the recursive blur effect.  What happens is we start out by turning
	// off depth testing, setting our blending mode, then binding the texture of our 
	// rendered textured over the QUAD that is about to be created.  Next, we set our 
	// alpha level to %90 of full strength.  This makes it so it will slowly disappear.
	// Before rendering the QUAD, we want to go into ortho mode.  This makes it easier
	// to render a QUAD over the full screen without having to deal with vertices, but
	// 2D coordinates.  Once we render the QUAD, we want to go back to perspective mode.
	// We can then turn depth testing back on and turn off texturing.

	// Push on a new stack so that we do not interfere with the current matrix
	glPushMatrix();

		// Turn off depth testing so that we can blend over the screen
		glDisable(GL_DEPTH_TEST);			

		// Set our blend method and enable blending
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	
		glEnable(GL_BLEND);					

		// Bind the rendered texture to our QUAD
		glBindTexture(GL_TEXTURE_2D, g_Texture2[textureID2]);			

		// Decrease the alpha value of the blend by %10 so it will fade nicely
		glColor4f(1, 1, 1, 0.9f);

		// Switch to 2D mode (Ortho mode)
		OrthoMode(0, 0, width(), height());

		// Display a 2D quad with our blended texture
		glBegin(GL_QUADS);

			// Display the top left point of the 2D image
			glTexCoord2f(0.0f, 1.0f);	glVertex2f(0, 0);

			// Display the bottom left point of the 2D image
			glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, height());

			// Display the bottom right point of the 2D image
			glTexCoord2f(1.0f, 0.0f);	glVertex2f(width(), height());

			// Display the top right point of the 2D image
			glTexCoord2f(1.0f, 1.0f);	glVertex2f(width(), 0);

		// Stop drawing 
		glEnd();
		
		// Let's set our mode back to perspective 3D mode.
		PerspectiveMode();

		// Turn depth testing back on and texturing off.  If you do NOT do these 2 lines of 
		// code it produces a cool flash effect that might come in handy somewhere down the road.
		glEnable(GL_DEPTH_TEST);						
		glDisable(GL_BLEND);							

	// Go back to our original matrix
	glPopMatrix();
}
