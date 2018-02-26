#pragma once

#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>
#include <stdio.h>      /* printf */
#include <math.h>       /* sin */
//#include "_minecraft/src/Render.h"
#include "Render.h"
#include "saveMap.h"

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"

//Pour avoir le monde
#include "world.h"
#include "RenderWorld.h"

extern NYRenderer * g_renderer;

extern NYTimer * g_timer;
extern int g_nb_frames;
extern float g_elapsed_fps;
extern int g_main_window_id;
extern int g_mouse_btn_gui_state;
extern bool g_fullscreen;

//GUI 
extern GUIScreenManager * g_screen_manager;
extern GUIBouton * BtnParams;
extern GUIBouton * BtnClose;
extern GUILabel * LabelFps;
extern GUILabel * LabelCam;
extern GUIScreen * g_screen_params;
extern GUIScreen * g_screen_jeu;
extern GLuint g_program;

extern GUISlider * g_slider;

//cor
//Soleil
extern NYVert3Df g_sun_dir;
extern NYColor g_sun_color;
extern float g_mn_lever;
extern float g_mn_coucher;
extern float g_tweak_time;
extern bool g_fast_time;

//world
extern NYWorld * g_world;
extern SaveMap *g_saveMap;
extern int wire;

struct PosMouse
{
	int x;
	int y;

	PosMouse(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
};

void moveCameraPlayer(int x, int y);
void movePlayer(void);
void light(void);

void update(void);
void render2d(void);
void renderObjects(void);
void setLights(void);
void resizeFunction(int width, int height);
void specialDownFunction(int key, int p1, int p2);
void specialUpFunction(int key, int p1, int p2);
void keyboardDownFunction(unsigned char key, int p1, int p2);
void keyboardUpFunction(unsigned char key, int p1, int p2);
void mouseWheelFunction(int wheel, int dir, int x, int y);
void mouseFunction(int button, int state, int x, int y);
void mouseMoveFunction(int x, int y, bool pressed);
void mouseMoveActiveFunction(int x, int y);
void mouseMovePassiveFunction(int x, int y);
void clickBtnParams(GUIBouton * bouton);
void clickBtnCloseParam(GUIBouton * bouton);
int main(int argc, char* argv[]);