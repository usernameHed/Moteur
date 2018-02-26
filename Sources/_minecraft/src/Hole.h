#pragma once
#include <vector>
#include "Hole.h"
#include "engine/utils/types_3d.h"
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */



class Hole
{
public:
	enum TypeHole
	{
		FILL = 0,
		SUB = 1,
		WORLD = 2,
	};

private:
	NYVert3Df max;
	int minRadius = 2;
	int maxRadius;
	int maxWorld;

	std::vector<std::vector<int> > holeCoord;

	void findMaxRadius();


public:
	Hole(int xMax, int yMax, int zMax);
	~Hole();

	void addHoles(int nbHoles, TypeHole type);
	bool testIfBlockOnHoles(int x, int y, int z, TypeHole type);
	bool testIfBlockOnHolesWorld(int x, int y, int z);
	//void drawHole(unsigned int x1, unsigned int y1, unsigned int r, const Mapping &mapping, const TypeHole &typeHole);
};
