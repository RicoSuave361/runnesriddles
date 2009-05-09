// Include C++ Library
#include<stdio.h>

// Include QT Library
#include <QtGui>

// Include Other Library
#include "headers.h"

// Include Widget
#include "window.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	printf("Iniciando...");
	Window w(NULL);
	w.show();
	//while(true) w.update();
	//w.showFullScreen();
	return a.exec();
}
