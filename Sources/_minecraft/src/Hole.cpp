#include "Hole.h"


//défini la size du monde
Hole::Hole(int xMax, int yMax, int zMax)
{
	
	holeCoord.clear();
	max.X = xMax;
	max.Y = yMax;
	max.Z = zMax;
	findMaxRadius();
}


Hole::~Hole()
{

}

//calcule le max radius
void Hole::findMaxRadius()
{
	maxWorld = min(min(max.X, max.Y), max.Z);
	maxRadius = maxWorld / 3;
	if (maxRadius <= minRadius)
	{
		cout << "error hole 28" << endl;
		exit(0);
		return;
	}
}

//ajoute un hole à la list
void Hole::addHoles(int nbHoles, TypeHole type)
{
	for (int i = 0; i < nbHoles; i++)
	{
		std::vector<int> littleHole;

		int radius = minRadius + rand() % (maxRadius - minRadius);
		//radius = radius * radius;
		if (type == WORLD)
			radius = maxWorld;

		littleHole.push_back(radius + rand() % ((int)max.X - (radius * 2)) ); //(max.X - (radius * 2)));
		littleHole.push_back(radius + rand() % ((int)max.Y - (radius * 2))); //(max.X - (radius * 2)));
		littleHole.push_back(radius + rand() % ((int)max.Z - (radius * 2))); //(max.X - (radius * 2)));

		littleHole.push_back(radius);

		littleHole.push_back((int)type);

		//cout << "new hole: (" << littleHole[0] << "," << littleHole[1] << "," << littleHole[2] << "," << littleHole[3] << endl;

		holeCoord.push_back(littleHole);
	}
}

bool Hole::testIfBlockOnHoles(int x, int y, int z, TypeHole type)
{
	for (int i = 0; i < holeCoord.size(); i++)
	{
		if (holeCoord[i][4] != (int)type)
			continue;
		int rxSin = holeCoord[i][0];
		int rySin = holeCoord[i][1];
		int rzSin = holeCoord[i][2];

		int powX = pow(x - (holeCoord[i][0]), 2.0);
		int powY = pow(y - (holeCoord[i][1]), 2.0);
		int powZ = pow(z - (holeCoord[i][2]), 2.0);

		//+ (rr * 2 * sin(x * 0.9)) - (rr * 0.05 * sin(y * 0.02)
		int distance = sqrt(powX + powY + powZ);
		//int distance = pow(x - holeCoord[i][0], 2.0) + pow(y - holeCoord[i][1], 2.0) + pow(z - holeCoord[i][2], 2.0);
		//if (distance < holeCoord[i][3] + ((holeCoord[i][3] * 2 * sin(x * 0.9)) - (holeCoord[i][3] * 0.05 * sin(y * 0.02))))
		if (distance < holeCoord[i][3])
		{
			return (true);
		}
			
	}
	return (false);
}

bool Hole::testIfBlockOnHolesWorld(int x, int y, int z)
{
	for (int i = 0; i < holeCoord.size(); i++)
	{
		if (holeCoord[i][4] != WORLD)
			continue;
		int rxSin = holeCoord[i][0];
		int rySin = holeCoord[i][1];
		int rzSin = holeCoord[i][2];

		int powX = pow(x - (holeCoord[i][0]), 2.0);
		int powY = pow(y - (holeCoord[i][1]), 2.0);
		int powZ = pow(z - (holeCoord[i][2]), 2.0);

		//+ (rr * 2 * sin(x * 0.9)) - (rr * 0.05 * sin(y * 0.02)
		int distance = sqrt(powX + powY + powZ);
		//int distance = pow(x - holeCoord[i][0], 2.0) + pow(y - holeCoord[i][1], 2.0) + pow(z - holeCoord[i][2], 2.0);
		//if (distance < holeCoord[i][3] + ((holeCoord[i][3] * 2 * sin(x * 0.9)) - (holeCoord[i][3] * 0.05 * sin(y * 0.02))))
		if (distance < holeCoord[i][3])
		{
			return (true);
		}
			
	}
	return (false);
}