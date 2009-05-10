#ifndef _MY_HEADER
#define _MY_HEADER

#include <iostream>						// Include our c++ standard header
#include <fstream>						// Include this to use ifstream
#include <string>						// Include this to use the string class
#include <vector>	
#include <math.h>
#include <algorithm>

#pragma warning(disable:4996)

using namespace std;					// Start off with the standard namespace

#include "vector.h"
#include "3d.h"
#include "obj.h"

typedef unsigned char byte;
#define SCREEN_WIDTH  640				// We want our screen width 640 pixels
#define SCREEN_HEIGHT 480				// We want our screen height 480 pixels
#define SCREEN_DEPTH  16					// We want 16 bits per pixel
#define MAX_PATH      260
#define MAX_TEXTURES  100				// The maximum amount of textures to load
#define SCREEN_DEPTH 16	

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