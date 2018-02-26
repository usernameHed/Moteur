#include "RenderWorld.h"

//g�n�re le monde au d�but
void initTheWorld(void)
{
	g_world = new NYWorld();
	g_world->_FacteurGeneration = 5;
	g_world->regenerateFullWorld();

	g_saveMap = new SaveMap(g_world);
}

//ici g�n�re le monde en jeu...
void renderTheWorld(void)
{
	glPushMatrix();
	//g_world->render_world_old_school();
	g_world->render_world_vbo();
	glPopMatrix();
}