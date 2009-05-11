#ifndef WINDOW_H
#define WINDOW_H

#include "headers.h"
#include "audio.h"
#include <QtGui>
#include <QGLWidget>
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );

class Window : public QGLWidget
{
public:

	unsigned int g_Texture[MAX_TEXTURES];						// This holds the texture info, referenced by an ID
	CLoadObj g_LoadObj;										// This is OBJ class.  This should go in a good model class.
	t3DModel g_3DModel;										// This holds the 3D Model info that we load in
	float g_RotateX;							// This is the current value at which the model is rotated
	float g_RotationSpeed ;							// This is the speed that our model rotates.  (-speed rotates left)

	Audio audio;

	Window(QWidget *parent);
	~Window();
    QTime m_time;
	QCursor *mouse;
	void resizeGL(int width, int height);

	// VSync desable...
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;
	void setVSync(int interval=1)
	{
	  const char *extensions = (const char *)glGetString( GL_EXTENSIONS );
	  if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 ){
		  printf("Failed WGL_EXT_swap_control not Found");
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	  }else
	  {
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
		if( wglSwapIntervalEXT )
		  wglSwapIntervalEXT(interval);
	  }
	}

protected:
    QTimer *m_timer;
	QString debugDisplay;
    int sec;
	int fps;
	double ratio;
	int mX,mY;
	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent (QKeyEvent * event );
	 
};

#endif // WINDOW_H
