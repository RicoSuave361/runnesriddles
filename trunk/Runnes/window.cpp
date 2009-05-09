#include "window.h"

Window::Window(QWidget *parent)
	: QGLWidget(parent)
{
	setMinimumSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	setGeometry(50,50,SCREEN_WIDTH, SCREEN_HEIGHT);
	m_timer = new QTimer(this);
    m_timer->setInterval(15);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    m_timer->start();
	m_time.start();
}

Window::~Window()
{

}


void Window::initializeGL()
{

}

void Window::paintGL()
{       
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	

}
void Window::resizeGL(int width, int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.33, 0.0001f, 10.0f);
}

void Window::mousePressEvent(QMouseEvent *event)
{
	
}
void Window::mouseMoveEvent(QMouseEvent *event)
{

}

void Window::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_Escape){
		exit(0);
	}
}

