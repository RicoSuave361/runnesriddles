#ifndef _MY_HEADER
#define _MY_HEADER

#include <iostream>	
#include <fstream>
#include <string>
#include <vector>	
#include <math.h>
#include <algorithm>

#pragma warning(disable:4996)
#pragma warning(disable:4244)

using namespace std;

#include "vector.h"
#include "3d.h"
#include "obj.h"
#include "camera.h"

typedef unsigned char byte;
#define SCREEN_WIDTH  1024				//Window width
#define SCREEN_HEIGHT 768				//Window height
#define SCREEN_DEPTH  32				//Color Depth
#define MAX_PATH      260
#define MAX_TEXTURES  100				// The maximum amount of textures to load

// Include OpenGL Library
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/al.h"
#include "GL/alc.h"
#include "GL/efx.h"
#include "GL/efx-creative.h"
#include "GL/EFX-Util.h"
#include "GL/xram.h"
#include "Framework/Framework.h"
#include "Framework/Vorbis/vorbisfile.h"

//#include "GL/glut.h"
//#pragma comment(lib, "lib/glut32.lib")

#pragma comment(lib, "lib/opengl32.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/OpenAL32.lib")
#pragma comment(lib, "lib/EFX-Util.lib")

#endif