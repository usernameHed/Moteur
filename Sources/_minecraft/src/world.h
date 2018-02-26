#ifndef __WORLD_H__
#define __WORLD_H__

#include "gl/glew.h"
#include "gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"
#include "Hole.h"

typedef uint8 NYAxis;
#define NY_AXIS_X 0x01
#define NY_AXIS_Y 0x02
#define NY_AXIS_Z 0x04

#define MAT_SIZE 8 //en nombre de chunks
#define MAT_HEIGHT 8 //en nombre de chunks
#define MAT_SIZE_CUBES (MAT_SIZE * NYChunk::CHUNK_SIZE)
#define MAT_HEIGHT_CUBES (MAT_HEIGHT * NYChunk::CHUNK_SIZE)
#define BETWEEN_CUBE_DEBUG_SIZE 0.5f //espace entre les chunk

extern int wire;

class NYWorld
{
public :
	NYChunk * _Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];
	int _MatriceHeights[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	int _MatriceHeightsTmp[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	float _FacteurGeneration;
	int nbHole = 100;

	int alive = 2;
	int dead = 3;
	int poid = 305;

	NYWorld();

	inline NYCube * getCube(int x, int y, int z, bool loop = false);
	void updateCube(int x, int y, int z);
	void deleteCube(int x, int y, int z);
	void load_pile(int x, int y, int height, bool onlyIfZero);
	void generate_piles(int x1, int y1,
		int x2, int y2,
		int x3, int y3,
		int x4, int y4, int prof, int profMax);
	void lisse(void);
	void init_world(int profmax = -1);
	NYCube * pick(NYVert3Df  pos, NYVert3Df  dir, NYPoint3D * point);
	NYAxis getMinCol(NYVert3Df pos, float width, float height, float & valueColMin, int i);
	void render_world_vbo(void);
	void add_world_to_vbo(void);

	void generateHole(void);
	void doAutomate();
	void generateGrass(void);
	void disableCubsWhenHidden(void);
	

	/*void drawCoin(void);


	void renderChunk(int x, int y, int z);
	void renderCube(NYChunk *chunk, int x, int y, int z);
	*/
	void randomCube(void);
	void noiseGround(int profmax);
	void render_world_old_school(void);
	void regenerateFullWorld(void);

	void changeAutomate(void);
	int doSome(int x, int y, int z);
	int getCubeNeighber(int neighber, int x, int y, int z);
	int getNumber(int number);
};



#endif