#ifndef WINDOW_H
#define WINDOW_H

#include "headers.h"
#include <QtGui>
#include <QGLWidget>


class Window : public QGLWidget
{
	Q_OBJECT

public:
	Window(QWidget *parent);
	~Window();
    QTime m_time;
	void resizeGL(int width, int height);

protected:
    QTimer *m_timer;
	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent (QKeyEvent * event );
};

#endif // WINDOW_H
