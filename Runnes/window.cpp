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

	// Inicializar carga de OBJs
	memset(g_Texture,0,sizeof(g_Texture));
	g_RotateX=0.0f;
	g_RotationSpeed=0.8f;
	g_3DModel.numOfMaterials=0;
	g_3DModel.numOfObjects=0;
	g_LoadObj.m_bObjectHasUV=false;
	g_LoadObj.m_bJustReadAFace=false;
}

Window::~Window()
{

}

void Window::resizeGL(int width, int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.33, 0.0001f, 30.0f);
}


void Window::initializeGL()
{
	setVSync(0);
	bool load=g_LoadObj.ImportObj(&g_3DModel, "Models/Box.obj");
	load=g_LoadObj.ImportObj(&g_3DModel, "Models/oct2.obj");
	g_LoadObj.AddMaterial(&g_3DModel, "bone", "Textures/bone.bmp", 255, 255, 255);
	g_LoadObj.AddMaterial(&g_3DModel, "text2", "Textures/text2.jpg", 255, 255, 255);
	g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);
	g_LoadObj.SetObjectMaterial(&g_3DModel, 1, 1);
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
	audio.Play("Footsteps.wav");	
}

void Window::paintGL()
{       
	
	
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	debugDisplay=QString("FPS: ")+QString::number(ratio);

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


	gluLookAt(0, 1.5f, 12,0, 0.5f, 0,0, 1, 0);	
	glRotatef(g_RotateX, 0, 1.0f, 0);						// Rotate the object around the Y-Axis
	g_RotateX = (m_time.msecsTo(QTime::currentTime())/10)*g_RotationSpeed;							// Increase the speed of rotation

    //glScalef(5.3f,5.3f,5.3f);

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
}

void Window::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_Escape){
		close();
	}
	if(event->key()==Qt::Key_0){
		if(this->isFullScreen()){
			showNormal();
			setCursor(Qt::CrossCursor);
		}else{
			showFullScreen();
			setCursor(QCursor(QPixmap("textures/transparent.png"))); //Mouse transparente
		}
	}
}

