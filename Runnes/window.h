#ifndef WINDOW_H
#define WINDOW_H

#include "headers.h"
#include "audio.h"
#include "skybox.h"
#include "HeightMap.h"
#include "SystemParticle.h"
#include <QtGui>
#include <QGLWidget>

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );

class Window : public QGLWidget
{
public:
	unsigned int g_Texture[MAX_TEXTURES];			// This holds the texture info, referenced by an ID
	CLoadObj g_LoadObj;								// This is OBJ class.  This should go in a good model class.
	t3DModel g_3DModel;								// This holds the 3D Model info that we load in
	float g_RotateX;								// This is the current value at which the model is rotated
	float g_RotationSpeed ;							// This is the speed that our model rotates.  (-speed rotates left)

    GLuint primitiveList;

	Audio audio;									//Audio Initialize
	SkyBox *sky;									//SkyBox Initialize
	CXBOXController playerController;

	Window(QWidget *parent);						//Window Handler
	~Window();

	HeightMap hp;	
	HeightMap escalera;									

	CFrustum g_Frustum;								// esta es nuestra piramide truncada
	bool g_bIgnoreFrustum;							// boleano que me la activa o no

    QTime m_time;									//Gametime
	QCursor *mouse;									//Mouse instance

	void resizeGL(int width, int height);			//Window resize
	SystemParticle *sp;
	bool kL,kD,kR,kU;
	int nrObjectDraw;
	void drawObj(int ID);
	float dead,dt;
	//VSync handler
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;
	void setVSync(int interval=1)
	{
	  const char *extensions = (const char *)glGetString( GL_EXTENSIONS );
	  if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
	  {
		  printf("Failed WGL_EXT_swap_control not Found");
			return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	  }
	  else
	  {
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
			
			if( wglSwapIntervalEXT )
				wglSwapIntervalEXT(interval);
	  }
	}

	char *textFileRead(char *fn) 
		{
			FILE *fp;
			char *content = NULL;

			int count=0;

			if (fn != NULL) 
			{
				fp = fopen(fn,"r");

				if (fp != NULL) 
				{
					fseek(fp, 0, SEEK_END);
					count = ftell(fp);
					rewind(fp);

					if (count > 0) 
					{
						content = (char *)malloc(sizeof(char) * (count+1));
						count = fread(content,sizeof(char),count,fp);
						content[count] = '\0';
					}
					fclose(fp);
				}
			}
			return content;
		}

		int textFileWrite(char *fn, char *s) 
		{
			FILE *fp;
			int status = 0;

			if (fn != NULL) 
			{
				fp = fopen(fn,"w");

				if (fp != NULL) 
				{
					if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
						status = 1;
					fclose(fp);
				}else printf("filed open %s\n",fn);
			}
			return(status);
		}

		GLint getUniLoc(GLuint program, const GLchar *name)
		{
			GLint loc;
			loc = glGetUniformLocation(program, name);

			return loc;
		}

		void initShader(void)
		{
			char *vs = NULL,*fs = NULL,*fs2 = NULL;
			
			//creacion del shader
			v = glCreateShader(GL_VERTEX_SHADER);
			f = glCreateShader(GL_FRAGMENT_SHADER);
			
			char cwd[]="./glsl/phong.vert";
				//=_getcwd(NULL,0);
			//strcat( cwd, "./glsl/phong.vert");

			char cwd2[]="./glsl/phong.frag";
				//_getcwd(NULL,0);
			//strcat( cwd2, "./glsl/phong.frag");

			vs =  textFileRead(cwd);
			fs = textFileRead(cwd2);

			const char * ff = fs;	
			const char * vv = vs;

			//carga el codigo al shader creado
			glShaderSource(v, 1, &vv,NULL);
			glShaderSource(f, 1, &ff,NULL);
			
			free(vs);
			free(fs);
			
			//compila el shader
			glCompileShader(v);
			glCompileShader(f);

			//crea el programa
			p = glCreateProgram();

			//adjunta el shader al programa creado
			glAttachShader(p,f);
			glAttachShader(p,v);

			//se enlaza el programa
			glLinkProgram(p);
		}

		void applyShader(void)
		{
			glUseProgram(p);
		}

		void unapplyShader(void)
		{
			glUseProgram(0);
		}

	//void DrawBox(CVector3 Maximo,CVector3 Minimo);	

protected:
    QTimer *m_timer;
	QString debugDisplay;
    int sec;
	int fps;
	double ratio;
	int mX,mY, lastX, lastY;
	float posX, posZ;
	float viewX, viewY;
	CCamera camera;
	GLuint v,f,f2,p;
	void initializeGL();
	void paintGL();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent (QKeyEvent * event );
	void keyReleaseEvent(QKeyEvent * event );
};

#endif // WINDOW_H
