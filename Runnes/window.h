#ifndef WINDOW_H
#define WINDOW_H

#include "headers.h"
#include <QtGui>
#include <QGLWidget>

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );


class Window : public QGLWidget
{
	Q_OBJECT

public:
	Window(QWidget *parent);
	~Window();
    QTime m_time;
	void resizeGL(int width, int height);
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
    int sec;
	int fps;
	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent (QKeyEvent * event );
};

#endif // WINDOW_H
