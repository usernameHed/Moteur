#include "world.h"

NYWorld::NYWorld()
{
	_FacteurGeneration = 1.0;

	//On crée les chunks
	for (int x = 0; x<MAT_SIZE; x++)
		for (int y = 0; y<MAT_SIZE; y++)
			for (int z = 0; z<MAT_HEIGHT; z++)
				_Chunks[x][y][z] = new NYChunk();

	for (int x = 0; x<MAT_SIZE; x++)
		for (int y = 0; y<MAT_SIZE; y++)
			for (int z = 0; z<MAT_HEIGHT; z++)
			{
				NYChunk * cxPrev = NULL;
				if (x > 0)
					cxPrev = _Chunks[x - 1][y][z];
				NYChunk * cxNext = NULL;
				if (x < MAT_SIZE - 1)
					cxNext = _Chunks[x + 1][y][z];

				NYChunk * cyPrev = NULL;
				if (y > 0)
					cyPrev = _Chunks[x][y - 1][z];
				NYChunk * cyNext = NULL;
				if (y < MAT_SIZE - 1)
					cyNext = _Chunks[x][y + 1][z];

				NYChunk * czPrev = NULL;
				if (z > 0)
					czPrev = _Chunks[x][y][z - 1];
				NYChunk * czNext = NULL;
				if (z < MAT_HEIGHT - 1)
					czNext = _Chunks[x][y][z + 1];

				_Chunks[x][y][z]->setVoisins(cxPrev, cxNext, cyPrev, cyNext, czPrev, czNext);
			}

}

inline NYCube * NYWorld::getCube(int x, int y, int z, bool loop)
{
	if (!loop)
	{
		if (x < 0)x = 0;
		if (y < 0)y = 0;
		if (z < 0)z = 0;
		if (x >= MAT_SIZE * NYChunk::CHUNK_SIZE) x = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (y >= MAT_SIZE * NYChunk::CHUNK_SIZE) y = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE) z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE) - 1;
	}
	else
	{
		if (x < 0)x = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (y < 0)y = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (z < 0)z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE) - 1;
		if (x >= MAT_SIZE * NYChunk::CHUNK_SIZE) x = 0;
		if (y >= MAT_SIZE * NYChunk::CHUNK_SIZE) y = 0;
		if (z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE) z = 0;
	}

	return &(_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->_Cubes[x % NYChunk::CHUNK_SIZE][y % NYChunk::CHUNK_SIZE][z % NYChunk::CHUNK_SIZE]);
}

void NYWorld::updateCube(int x, int y, int z)
{
	if (x < 0)x = 0;
	if (y < 0)y = 0;
	if (z < 0)z = 0;
	if (x >= MAT_SIZE * NYChunk::CHUNK_SIZE)x = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
	if (y >= MAT_SIZE * NYChunk::CHUNK_SIZE)y = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
	if (z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE)z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE) - 1;
	_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->toVbo();
}

void NYWorld::deleteCube(int x, int y, int z)
{
	NYCube * cube = getCube(x, y, z);
	cube->_Draw = false;
	cube = getCube(x - 1, y, z);
	updateCube(x, y, z);
}

//Création d'une pile de cubes
//only if zero permet de ne générer la  pile que si sa hauteur actuelle est de 0 (et ainsi de ne pas regénérer de piles existantes)
void NYWorld::load_pile(int x, int y, int height, bool onlyIfZero = true)
{
	if (height < 1)
		height = 1;
	if (height >= MAT_HEIGHT_CUBES)
		height = MAT_HEIGHT_CUBES - 1;

	if (_MatriceHeights[x][y] != 0 && onlyIfZero)
		return;

	for (int z = 0; z<height; z++)
	{
		getCube(x, y, z)->_Draw = true;
		if (z>0)
			getCube(x, y, z)->_Type = CUBE_TERRE;
		else
			getCube(x, y, z)->_Type = CUBE_EAU;
	}

	if (height - 1>0)
	{
		getCube(x, y, height - 1)->_Draw = true;
		getCube(x, y, height - 1)->_Type = CUBE_HERBE;
	}

	for (int z = height; z<MAT_HEIGHT_CUBES; z++)
	{
		getCube(x, y, z)->_Draw = true;
		getCube(x, y, z)->_Type = CUBE_AIR;
	}

	_MatriceHeights[x][y] = height;
}

//Creation du monde entier, en utilisant le mouvement brownien fractionnaire
void NYWorld::generate_piles(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	int x4, int y4, int prof, int profMax = -1)
{
	//return;

	if ((x3 - x1) <= 1 && (y3 - y1) <= 1)
		return;

	int largeurRandom = (int)(MAT_HEIGHT_CUBES / (prof*_FacteurGeneration));
	if (largeurRandom == 0)
		largeurRandom = 1;

	if (profMax >= 0 && prof >= profMax)
	{
		Log::log(Log::ENGINE_INFO, ("End of generation at prof " + toString(prof)).c_str());
		return;
	}

	//On se met au milieu des deux coins du haut
	int xa = (x1 + x2) / 2;
	int ya = (y1 + y2) / 2;
	int heighta = (_MatriceHeights[x1][y1] + _MatriceHeights[x2][y2]) / 2;
	if ((x2 - x1)>1)
	{
		heighta += (rand() % largeurRandom) - (largeurRandom / 2);
		load_pile(xa, ya, heighta);
	}
	else
		heighta = _MatriceHeights[xa][ya];

	//Au milieu des deux coins de droite
	int xb = (x2 + x3) / 2;
	int yb = (y2 + y3) / 2;
	int heightb = (_MatriceHeights[x2][y2] + _MatriceHeights[x3][y3]) / 2;
	if ((y3 - y2)>1)
	{
		heightb += (rand() % largeurRandom) - (largeurRandom / 2);
		load_pile(xb, yb, heightb);
	}
	else
		heightb = _MatriceHeights[xb][yb];

	//Au milieu des deux coins du bas
	int xc = (x3 + x4) / 2;
	int yc = (y3 + y4) / 2;
	int heightc = (_MatriceHeights[x3][y3] + _MatriceHeights[x4][y4]) / 2;
	heightc += (rand() % largeurRandom) - (largeurRandom / 2);
	if ((x3 - x4)>1)
	{
		load_pile(xc, yc, heightc);
	}
	else
		heightc = _MatriceHeights[xc][yc];

	//Au milieu des deux coins de gauche
	int xd = (x4 + x1) / 2;
	int yd = (y4 + y1) / 2;
	int heightd = (_MatriceHeights[x4][y4] + _MatriceHeights[x1][y1]) / 2;
	heightd += (rand() % largeurRandom) - (largeurRandom / 2);
	if ((y3 - y1)>1)
	{
		load_pile(xd, yd, heightd);
	}
	else
		heightd = _MatriceHeights[xd][yd];

	//Au milieu milieu
	int xe = xa;
	int ye = yb;
	if ((x3 - x1)>1 && (y3 - y1)>1)
	{
		int heighte = (heighta + heightb + heightc + heightd) / 4;
		heighte += (rand() % largeurRandom) - (largeurRandom / 2);
		load_pile(xe, ye, heighte);
	}

	//On genere les 4 nouveaux quads
	generate_piles(x1, y1, xa, ya, xe, ye, xd, yd, prof + 1, profMax);
	generate_piles(xa, ya, x2, y2, xb, yb, xe, ye, prof + 1, profMax);
	generate_piles(xe, ye, xb, yb, x3, y3, xc, yc, prof + 1, profMax);
	generate_piles(xd, yd, xe, ye, xc, yc, x4, y4, prof + 1, profMax);
}


void NYWorld::lisse(void)
{
	int sizeWidow = 4;
	memset(_MatriceHeightsTmp, 0x00, sizeof(int)*MAT_SIZE_CUBES*MAT_SIZE_CUBES);
	for (int x = 0; x<MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y<MAT_SIZE_CUBES; y++)
		{
			//on moyenne sur une distance
			int nb = 0;
			for (int i = (x - sizeWidow < 0 ? 0 : x - sizeWidow);
				i < (x + sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : x + sizeWidow); i++)
			{
				for (int j = (y - sizeWidow < 0 ? 0 : y - sizeWidow);
					j <(y + sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : y + sizeWidow); j++)
				{
					_MatriceHeightsTmp[x][y] += _MatriceHeights[i][j];
					nb++;
				}
			}
			if (nb)
				_MatriceHeightsTmp[x][y] /= nb;
		}
	}

	//On reset les piles
	for (int x = 0; x<MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y<MAT_SIZE_CUBES; y++)
		{
			load_pile(x, y, _MatriceHeightsTmp[x][y], false);
		}
	}
	

}

void NYWorld::init_world(int profmax)
{
	_cprintf("Creation du monde %f \n", _FacteurGeneration);

	srand(6665);

	//Reset du monde
	for (int x = 0; x<MAT_SIZE; x++)
		for (int y = 0; y<MAT_SIZE; y++)
			for (int z = 0; z<MAT_HEIGHT; z++)
				_Chunks[x][y][z]->reset();
	memset(_MatriceHeights, 0x00, MAT_SIZE_CUBES*MAT_SIZE_CUBES * sizeof(int));

	//On charge les 4 coins
	load_pile(0, 0, MAT_HEIGHT_CUBES / 2);
	load_pile(MAT_SIZE_CUBES - 1, 0, MAT_HEIGHT_CUBES / 2);
	load_pile(MAT_SIZE_CUBES - 1, MAT_SIZE_CUBES - 1, MAT_HEIGHT_CUBES / 2);
	load_pile(0, MAT_SIZE_CUBES - 1, MAT_HEIGHT_CUBES / 2);

	//generateWorld();

	
		
}

NYCube * NYWorld::pick(NYVert3Df  pos, NYVert3Df  dir, NYPoint3D * point)
{
	return NULL;
}

//Boites de collisions plus petites que deux cubes
NYAxis NYWorld::getMinCol(NYVert3Df pos, float width, float height, float & valueColMin, int i)
{
	NYAxis axis = 0x00;
	return axis;
}


void NYWorld::render_world_vbo(void)
{
	for (int x = 0; x<MAT_SIZE; x++)
		for (int y = 0; y<MAT_SIZE; y++)
			for (int z = 0; z<MAT_HEIGHT; z++)
			{
				glPushMatrix();
				glTranslatef((float)(x*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE), (float)(y*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE), (float)(z*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE));
				_Chunks[x][y][z]->render();
				glPopMatrix();
			}
}

void NYWorld::add_world_to_vbo(void)
{
	int totalNbVertices = 0;

	for (int x = 0; x<MAT_SIZE; x++)
		for (int y = 0; y<MAT_SIZE; y++)
			for (int z = 0; z<MAT_HEIGHT; z++)
			{
				_Chunks[x][y][z]->toVbo();
				totalNbVertices += _Chunks[x][y][z]->_NbVertices;
			}

	Log::log(Log::ENGINE_INFO, (toString(totalNbVertices) + " vertices in VBO").c_str());
}

/*
void NYWorld::renderCube(NYChunk *chunk, int x, int y, int z)
{
	//chunk->_Cubes[x][y][z]._Draw = true;
	//chunk->_Cubes[x][y][z]._Type = CUBE_HERBE;

	switch (chunk->_Cubes[x][y][z]._Type)
	{
	case NYCubeType::CUBE_EAU:
		glColor3d(0, 0, 1);
		break;
	case NYCubeType::CUBE_TERRE:
		glColor3d(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f);
		break;
	case NYCubeType::CUBE_HERBE:
		glColor3d(0, 1, 0);
		break;

	default:
		return;
		break;
	}

	glTranslatef(x * NYCube::CUBE_SIZE, y * NYCube::CUBE_SIZE, z * NYCube::CUBE_SIZE);

	if (wire == -1)
		glutSolidCube(NYCube::CUBE_SIZE);
	else
		glutWireCube(NYCube::CUBE_SIZE);

	glTranslatef(-x * NYCube::CUBE_SIZE, -y * NYCube::CUBE_SIZE, -z * NYCube::CUBE_SIZE);
	
}

void NYWorld::renderChunk(int xChunk, int yChunk, int zChunk)
{
	for (int x = 0; x < NYChunk::CHUNK_SIZE; x++)
		for (int y = 0; y < NYChunk::CHUNK_SIZE; y++)
			for (int z = 0; z < NYChunk::CHUNK_SIZE; z++)
			{
				renderCube(_Chunks[xChunk][yChunk][zChunk], x, y, z);
			}
}
*/

void NYWorld::render_world_old_school(void)
{
	//Flemme des materials
	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();

	//Pour tous les cubes de notre matrice
	for (int x = 0; x <MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y < MAT_SIZE_CUBES; y++)
		{
			for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
			{
				//On recup le cube (se charge d'aller chercher dans le bon chunk)
				NYCube * cube = getCube(x, y, z);

				//On le dessine en fonction de son type
				if (cube->_Draw && cube->_Type != CUBE_AIR)
				{
					switch (cube->_Type)
					{
					case CUBE_TERRE:
						glColor3f(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f);
						break;
					case CUBE_HERBE:
						glColor3f(1.0f / 255.0f, 112.0f / 255.0f, 12.0f / 255.0f);
						break;
					case CUBE_EAU:
						glColor3f(0.0f / 255.0f, 48.0f / 255.0f, 255.0f / 255.0f);
						break;
					}

					//On se deplace pour chaque cube... bcp de push et pop
					glPushMatrix();
					glTranslated(x*NYCube::CUBE_SIZE, y*NYCube::CUBE_SIZE, z*NYCube::CUBE_SIZE);

					if (wire == -1)
						glutSolidCube(NYCube::CUBE_SIZE);
					else
						glutWireCube(NYCube::CUBE_SIZE);

					//glutSolidCube(NYCube::CUBE_SIZE);
					glPopMatrix();
				}
			}
		}
	}
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);

	return;
	/*
	for (int x = 0; x < MAT_SIZE; x++)
		for (int y = 0; y < MAT_SIZE; y++)
			for (int z = 0; z < MAT_HEIGHT; z++)
			{
				glTranslatef(x * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE, y * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE, z * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE);

				renderChunk(x, y, z);

				glTranslatef(-x * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE, -y * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE, -z * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE);
				
			}*/
}
/*
void NYWorld::drawCoin()
{
	for (int xChunk = 0; xChunk < MAT_SIZE; xChunk++)
		for (int yChunk = 0; yChunk < MAT_SIZE; yChunk++)
			for (int zChunk = 0; zChunk < MAT_HEIGHT; zChunk++)
			{
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][0][0]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][0][0]._Type = CUBE_EAU;

				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][0][0]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][0][0]._Type = CUBE_EAU;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1][0]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1][0]._Type = CUBE_EAU;

				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][NYChunk::CHUNK_SIZE - 1][0]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][NYChunk::CHUNK_SIZE - 1][0]._Type = CUBE_EAU;

				
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][0][NYChunk::CHUNK_SIZE - 1]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][0][NYChunk::CHUNK_SIZE - 1]._Type = CUBE_EAU;

				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][0][NYChunk::CHUNK_SIZE - 1]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][0][NYChunk::CHUNK_SIZE - 1]._Type = CUBE_EAU;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1]._Type = CUBE_EAU;

				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1]._Draw = true;
				_Chunks[xChunk][yChunk][zChunk]->_Cubes[0][NYChunk::CHUNK_SIZE - 1][NYChunk::CHUNK_SIZE - 1]._Type = CUBE_EAU;
			}
}*/



void NYWorld::noiseGround(int profmax)
{
	//On génère a partir des 4 coins
	generate_piles(0, 0,
		MAT_SIZE_CUBES - 1, 0,
		MAT_SIZE_CUBES - 1, MAT_SIZE_CUBES - 1,
		0, MAT_SIZE_CUBES - 1, 1, profmax);
}

void NYWorld::disableCubsWhenHidden()
{
	for (int x = 0; x<MAT_SIZE; x++)
		for (int y = 0; y<MAT_SIZE; y++)
			for (int z = 0; z<MAT_HEIGHT; z++)
				_Chunks[x][y][z]->disableHiddenCubes();
}


void NYWorld::generateHole()
{
	//return;
	//drawCoin();
	

	//setup les trou en définissant les size MAx
	Hole hole(MAT_SIZE_CUBES, MAT_SIZE_CUBES, MAT_SIZE_CUBES);

	//créé X holes (stup les positions random + radius)
	srand(time(NULL));
	hole.addHoles(5 + rand() % 10, Hole::TypeHole::FILL);
	hole.addHoles(5 + rand() % 10, Hole::TypeHole::SUB);
	hole.addHoles(1, Hole::TypeHole::WORLD);

	//Pour tous les cubes de notre matrice
	for (int x = 0; x <MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y < MAT_SIZE_CUBES; y++)
		{
			for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
			{
				//On recup le cube (se charge d'aller chercher dans le bon chunk)
				NYCube * cube = getCube(x, y, z);

				if (hole.testIfBlockOnHoles(x, y, z, Hole::TypeHole::FILL))
				{
					//load_pile(x, y, z);
					//load_pile(MAT_SIZE_CUBES - 1, 0, MAT_HEIGHT_CUBES / 2);
					//load_pile(MAT_SIZE_CUBES - 1, MAT_SIZE_CUBES - 1, MAT_HEIGHT_CUBES / 2);
					//load_pile(0, MAT_SIZE_CUBES - 1, MAT_HEIGHT_CUBES / 2);

					cube->_Draw = true;
					cube->_Type = CUBE_TERRE;

					//renderCube(_Chunks[xChunk][yChunk][zChunk], x, y, z);
				}
				if (hole.testIfBlockOnHoles(x, y, z, Hole::TypeHole::SUB))
				{
					cube->_Draw = true;
					cube->_Type = CUBE_AIR;
				}
				/*if (!hole.testIfBlockOnHolesWorld(x, y, z))
				{
					cube->_Draw = true;
					cube->_Type = CUBE_AIR;
				}*/

			}
		}
	}
}

void NYWorld::generateGrass()
{
	//Pour tous les cubes de notre matrice
	for (int x = 0; x <MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y < MAT_SIZE_CUBES; y++)
		{
			for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
			{
				NYCube * cube = getCube(x, y, z);	//get le cube

				//ici si on est pas TOUT en haut, test si on peut rajouter de la grass
				if (z < (MAT_HEIGHT * NYChunk::CHUNK_SIZE) - 1)
				{
					//ici rajoute de la grass si le cube le plus haut n'en ai pas.
					NYCube * Upercube = getCube(x, y, z + 1);
					if ((Upercube->_Type == CUBE_AIR || Upercube->_Draw == false) && (cube->_Type == CUBE_TERRE && cube->_Draw == true))
						cube->_Type = CUBE_HERBE;
				}
				continue;	//break l'algo pour l'instant...

				if (z <= 0)	//si il n'y a pas plus bas
					continue;
				
				//ici test le bas
				int zToFill = z - 1;
				NYCube * lowerCube = getCube(x, y, zToFill);

				if (!(lowerCube->_Type == CUBE_AIR || lowerCube->_Draw == false))
				{
					//if (lowerCube->_Type != CUBE_AIR || lowerCube->_Draw == true)
					continue;
				}

				for (int i = zToFill; i > 0; i--)	//du haut (le lower cube AIR trouvé vers le 0 (ou l'eau trouvé)
				{
					NYCube * moreLowerCube = getCube(x, y, i);
					if (moreLowerCube->_Type == CUBE_AIR || lowerCube->_Draw == false)	//si c'est toujours de l'air, on continue de descendre...
						continue;
					if ((moreLowerCube->_Type == CUBE_TERRE || moreLowerCube->_Type == CUBE_HERBE) && lowerCube->_Draw == true)	//si c'est terre / herbe, on arrete, c'est ok !
						break;
					if (moreLowerCube->_Type == CUBE_EAU && lowerCube->_Draw == true)	//si c'est de l'eau, on a enfin trouvé la source !
					{
						//ici on à l'eau, on doit remonter jusqu'a zToFill
						for (int zTemp = i + 1; zTemp <= zToFill; zTemp++)
						{
							NYCube * lessLowerCube = getCube(x, y, zTemp);
							lessLowerCube->_Type = CUBE_TERRE;
							lessLowerCube->_Draw = true;
						}

						break;
					}
					
				}

				//cube->_Draw = true;
				//cube->_Type = CUBE_AIR;
			}
		}
	}
}


void NYWorld::randomCube()
{
	//Pour tous les cubes de notre matrice
	for (int x = 0; x <MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y < MAT_SIZE_CUBES; y++)
		{
			for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
			{
				NYCube * cube = getCube(x, y, z);	//get le cube

				cube->_Draw = true;

				int randValue = alive + (rand() % static_cast<int>(/*max*/(dead + poid) - alive + 1));
				if (randValue > dead)
					randValue = dead;
				cube->_Type = (NYCubeType)randValue;
			}
		}
	}
}


void NYWorld::doAutomate()
{
	//Pour tous les cubes de notre matrice
	for (int x = 0; x <MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y < MAT_SIZE_CUBES; y++)
		{
			for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
			{
			//int z = MAT_HEIGHT_CUBES - 1;

				NYCube * cube = getCube(x, y, z);	//get le cube

				cube->_Draw = true;
				int sum = doSome(x, y, z);

				//2 = terre
				//3 = rien

				//cout << sum << endl;
				/*if (sum == 13)
				{
					int randValue = 0 + (rand() % static_cast<int>(1 - 0 + 1));
					if (randValue == 0)
						cube->_Type = (NYCubeType)3;
					else
						cube->_Type = (NYCubeType)2;
				}
				else
				{*/

				/*
				if (cube->_Type == (NYCubeType)alive &&
					(getCubeNeighber(13, x, y, z)
						&& getCubeNeighber(14, x, y, z)
						&& getCubeNeighber(16, x, y, z)
						&& getCubeNeighber(17, x, y, z)
						&& getCubeNeighber(18, x, y, z)
						&& getCubeNeighber(19, x, y, z)
						&& getCubeNeighber(20, x, y, z)
						&& getCubeNeighber(21, x, y, z)
						&& getCubeNeighber(22, x, y, z)
						&& getCubeNeighber(23, x, y, z)
						&& getCubeNeighber(24, x, y, z)
						&& getCubeNeighber(25, x, y, z)
						&& getCubeNeighber(26, x, y, z)

						))
				{
					cube->_Type = (NYCubeType)alive;
				}
					
				else if (cube->_Type == (NYCubeType)dead && (getCubeNeighber(17, x, y, z) || getCubeNeighber(18, x, y, z) || getCubeNeighber(19, x, y, z)))
				{
					cube->_Type = (NYCubeType)alive;
				}

				else
				{
					cube->_Type = (NYCubeType)dead;
				}
					

				//}

				//return;
				//int m = SommeMoore(i, j);

				//NYCube * cube = getCube(x, y, z);	//get le cube
				//NYCube * cube1 = getCube(x, y, z);	//get le cube
				*/



				//
			}
		}
	}
}


int NYWorld::getCubeNeighber(int neighber, int x, int y, int z)
{
	if (neighber == 1)	return getNumber((int)getCube(x - 1, y + 1, z)->_Type);
	if (neighber == 2)	return getNumber((int)getCube(x, y + 1, z)->_Type);	//get le cube
	if (neighber == 3)	return getNumber((int)getCube(x + 1, y + 1, z)->_Type);	//get le cube

	if (neighber == 4)	return getNumber((int)getCube(x, y - 1, z)->_Type);	//get le cube
	if (neighber == 5)	return getNumber((int)getCube(x, y + 1, z)->_Type);	//get le cube

	if (neighber == 6)	return getNumber((int)getCube(x - 1, y + 1, z)->_Type);	//get le cube
	if (neighber == 7)	return getNumber((int)getCube(x, y + 1, z)->_Type);	//get le cube
	if (neighber == 8)	return getNumber((int)getCube(x + 1, y + 1, z)->_Type);	//get le cube

	//face 2
	if (neighber == 9)	return getNumber((int)getCube(x - 1, y + 1, z - 1)->_Type);	//get le cube
	if (neighber == 10)	return getNumber((int)getCube(x, y + 1, z - 1)->_Type);	//get le cube
	if (neighber == 11)	return getNumber((int)getCube(x + 1, y + 1, z - 1)->_Type);	//get le cube

	if (neighber == 12)	return getNumber((int)getCube(x, y - 1, z - 1)->_Type);	//get le cube
	if (neighber == 13)	return getNumber((int)getCube(x, y + 1, z - 1)->_Type);	//get le cube
	if (neighber == 14)	return getNumber((int)getCube(x, y, z - 1)->_Type);	//get le cube

	if (neighber == 15)	return getNumber((int)getCube(x - 1, y + 1, z - 1)->_Type);	//get le cube
	if (neighber == 16)	return getNumber((int)getCube(x, y + 1, z - 1)->_Type);	//get le cube
	if (neighber == 17)	return getNumber((int)getCube(x + 1, y + 1, z - 1)->_Type);	//get le cube

	//face 3
	if (neighber == 18)	return getNumber((int)getCube(x - 1, y + 1, z + 1)->_Type);	//get le cube
	if (neighber == 19)	return getNumber((int)getCube(x, y + 1, z + 1)->_Type);	//get le cube
	if (neighber == 20)	return getNumber((int)getCube(x + 1, y + 1, z + 1)->_Type);	//get le cube

	if (neighber == 21)	return getNumber((int)getCube(x, y - 1, z + 1)->_Type);	//get le cube
	if (neighber == 22)	return getNumber((int)getCube(x, y + 1, z + 1)->_Type);	//get le cube
	if (neighber == 23)	return getNumber((int)getCube(x, y, z + 1)->_Type);	//get le cube

	if (neighber == 24)	return getNumber((int)getCube(x - 1, y + 1, z + 1)->_Type);	//get le cube
	if (neighber == 25)	return getNumber((int)getCube(x, y + 1, z + 1)->_Type);	//get le cube
	if (neighber == 26)	return getNumber((int)getCube(x + 1, y + 1, z + 1)->_Type);	//get le cube
	return (-1);
}


int NYWorld::getNumber(int number)
{
	if (number == dead)
		return (0);
	else if (number == alive)
		return (1);
	cout << "error" << endl;
	return (1);
}

int NYWorld::doSome(int x, int y, int z)
{
	int sum =  getNumber((int)getCube(x, y, z)->_Type);

	//sum += (int)getCube(x, y, z)->_Type;
	//face 1
	sum +=getNumber( (int)getCube(x - 1, y + 1, z)->_Type);	//get le cube
	sum +=getNumber( (int)getCube(x, y + 1, z)->_Type);	//get le cube
	sum +=getNumber( (int)getCube(x + 1, y + 1, z)->_Type);	//get le cube

	sum +=getNumber( (int)getCube(x, y - 1, z)->_Type);	//get le cube
	sum +=getNumber( (int)getCube(x, y + 1, z)->_Type);	//get le cube

	sum +=getNumber( (int)getCube(x - 1, y + 1, z)->_Type);	//get le cube
	sum +=getNumber( (int)getCube(x, y + 1, z)->_Type);	//get le cube
	sum +=getNumber( (int)getCube(x + 1, y + 1, z)->_Type);	//get le cube
	
	//face 2
	sum +=getNumber(  (int)getCube(x - 1, y + 1, z - 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y + 1, z - 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x + 1, y + 1, z - 1)->_Type);	//get le cube

	sum +=getNumber(  (int)getCube(x, y - 1, z - 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y + 1, z - 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y, z - 1)->_Type);	//get le cube

	sum +=getNumber(  (int)getCube(x - 1, y + 1, z - 1)	->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y + 1, z - 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x + 1, y + 1, z - 1)->_Type);	//get le cube

	//face 3
	sum +=getNumber(  (int)getCube(x - 1, y + 1, z + 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y + 1, z + 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x + 1, y + 1, z + 1)->_Type);	//get le cube

	sum +=getNumber(  (int)getCube(x, y - 1, z + 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y + 1, z + 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y, z + 1)->_Type);	//get le cube

	sum +=getNumber(  (int)getCube(x - 1, y + 1, z + 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x, y + 1, z + 1)->_Type);	//get le cube
	sum +=getNumber(  (int)getCube(x + 1, y + 1, z + 1)->_Type);	//get le cube
	


	return (sum);
}

void NYWorld::regenerateFullWorld()
{
	init_world();	//ici le noise ground est créé

	randomCube();
	//doAutomate(3);

	//noiseGround(5);	//créé les sols
	//disableCubsWhenHidden();	//cache les cubes ?

	//lisse();		//on lisse

	//generateHole();	//créé des trous
	//generateWorld();	//on créé les noises automates
	//generateGrass();	//on génère la grass !
	

	add_world_to_vbo();	//ajoute aux vbo pour la carte graphique
}

void NYWorld::changeAutomate()
{
	doAutomate();



	add_world_to_vbo();
}