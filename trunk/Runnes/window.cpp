#include "window.h"


Window::Window(QWidget *parent)
	: QGLWidget(parent),wglSwapIntervalEXT(0)
{
	// Inicializar Widget
	setCursor(Qt::CrossCursor);
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
	gluPerspective(45.0f, (float)width/(float)height, 0.1f, 300.0f);
}


void Window::initializeGL()
{
	
		//	showFullScreen();
	setVSync(1);
	g_LoadObj.ImportObj(&g_3DModel, "Models/Box.obj");
	//g_LoadObj.ImportObj(&g_3DModel, "Models/mm.obj");
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/bone.bmp", 255, 255, 255);
	//g_LoadObj.AddMaterial(&g_3DModel, "text2", "Textures/t2.jpg", 255, 255, 255);
	g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);
	//g_LoadObj.SetObjectMaterial(&g_3DModel, 1, 1);
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// Check if the current material has a file name
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0) {
			QImage img(g_3DModel.pMaterials[i].strFile);
			g_Texture[i]=bindTexture(img, GL_TEXTURE_2D);
		}
		
		// Assign the material ID to the current material
		g_3DModel.pMaterials[i].texureId = i;
	}

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color	
	glEnable(GL_TEXTURE_2D);							// Enables Texture Mapping
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glEnable(GL_CULL_FACE);
	//glEnable(GL_NORMALIZE);
	glCullFace(GL_BACK);
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
	audio.Play("Footsteps.wav");	
}

void Window::paintGL()
{ 
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	debugDisplay=QString("FPS: ")+QString::number(ratio)+QString(" Eye: ")+QString::number((double)camera.eye.x)+QString(" ")+QString::number((double)camera.eye.y)+QString(" ")+QString::number((double)camera.eye.z)+QString(" Center: ")+QString::number((double)camera.center.x)+QString(" ")+QString::number((double)camera.center.y)+QString(" ")+QString::number((double)camera.center.z)+QString(" Up: ")+QString::number((double)camera.up.x)+QString(" ")+QString::number((double)camera.up.y)+QString(" ")+QString::number((double)camera.up.z);

	renderText(10,10,debugDisplay);
	/////////////// Calcular FPS //////////////
	++fps;
	if(m_time.currentTime().second()!=sec && fps>0){
		ratio=double(fps)/double(abs(m_time.currentTime().second()-sec));
		sec=m_time.currentTime().second();
		setWindowTitle(QString("FPS: ")+QString::number(ratio));
		fps=0;
	}
	//////////////////////////////////////////

	
	gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z, camera.center.x, camera.center.y, camera.center.z, camera.up.x, camera.up.y, camera.up.z);	
	glPushMatrix();
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

	glPopMatrix();

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
	
	if(lastX==0 && lastY==0) return; 

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


	if(event->key()==Qt::Key_0){
		if(this->isFullScreen()){
			showNormal();
		//	setCursor(Qt::CrossCursor);
		}else{
			showFullScreen();
			setCursor(QCursor(QPixmap("textures/transparent.png"))); //Mouse transparente
		}
	}
}

