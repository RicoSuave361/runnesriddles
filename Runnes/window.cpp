#include "window.h"

Window::Window(QWidget *parent)
	: QGLWidget(parent),wglSwapIntervalEXT(0)
{
	setMinimumSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	setGeometry(50,50,SCREEN_WIDTH, SCREEN_HEIGHT);
	m_timer = new QTimer(this);
    m_timer->setInterval(1);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    m_timer->start();
	m_time.start();
	sec=0;
}

Window::~Window()
{

}


void Window::initializeGL()
{
	setVSync(0);
}

void Window::paintGL()
{       
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	++fps;
	if(m_time.currentTime().second()!=sec){
		double ratio=double(fps)/double(abs(m_time.currentTime().second()-sec));
		sec=m_time.currentTime().second();
		setWindowTitle(QString("FPS: ")+QString::number(ratio));
		fps=0;
	}

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

