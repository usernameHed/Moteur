#include "saveMap.h"



SaveMap::SaveMap(NYWorld * worldToSave)
	:	World(worldToSave)
{

}


SaveMap::~SaveMap()
{

}

void SaveMap::saveNewMap()
{
	/*std::ofstream stream("map.bin", std::ios::binary);
	if (!stream)
	{
		cout << "can't open file" << endl;
		return;
	}

	binary_write(stream, MAT_SIZE_CUBES);
	binary_write(stream, MAT_HEIGHT_CUBES);

	string datas = "";
	*/
	std::ofstream outfile("map.txt");
	if (!outfile)
	{
		cout << "can't open file" << endl;
		return;
	}

	outfile << MAT_SIZE_CUBES << std::endl;
	outfile << MAT_HEIGHT_CUBES << std::endl;
	for (int x = 0; x < MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y < MAT_SIZE_CUBES; y++)
		{
			for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
			{
				NYCube * cube = World->getCube(x, y, z);
				outfile << (int)cube->_Type;
				//datas += std::to_string((int)cube->_Type);
			}
			//outfile << endl;
		}
	}
	outfile.close();

	/*int sizeDatas = (datas.size());
	stream.write(reinterpret_cast<char *>(&sizeDatas), sizeof(int));
	stream.write(datas.c_str(), sizeDatas);

	//outfile.close();
	stream.flush();
	stream.close();
	*/

	cout << "saved !" << endl;
}

void SaveMap::loadLastMap()
{
	/*ifstream input("map.bin", ios::in | ios::binary);
	if (!input)
	{
		cout << "can't open file" << endl;
		return;
	}*/

	ifstream file("map.txt"); // pass file name as argment
	if (!file)
	{
		cout << "can't open file" << endl;
		return;
	}

	string linebuffer;
	int index = 0;

	while (file && getline(file, linebuffer))
	{
		if (linebuffer.length() == 0)
			continue;

		if (index == 0)
		{
			if (std::atoi(linebuffer.c_str()) != (int)MAT_SIZE_CUBES)
				return;
			index++;
			continue;
		}
		else if (index == 1)
		{
			if (std::atoi(linebuffer.c_str()) != (int)MAT_HEIGHT_CUBES)
				return;
			index++;
			continue;
		}

		std::cout << "same size, londing..." << std::endl;

		const char * pt = linebuffer.c_str();

		for (int x = 0; x < MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y < MAT_SIZE_CUBES; y++)
			{
				for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
				{
					NYCube * cube = World->getCube(x, y, z);

					int typeInt = (*pt - '0') % 48;// (linebuffer[(x * MAT_SIZE_CUBES) + (y * MAT_SIZE_CUBES) + (z * MAT_HEIGHT_CUBES)] - '0') % 48;
					

					cube->_Draw = true;
					cube->_Type = (NYCubeType)typeInt;

					pt++;

					/*for (int i = 0; i < linebuffer.size(); i++)
					{
						NYCube * cube = World->getCube(x, y, z);

						int typeInt = (linebuffer[i] - '0') % 48;

						cube->_Draw = true;
						cube->_Type = (NYCubeType)typeInt;
					}*/

					
				}
				//outfile << endl;
			}
		}
		std::cout << "ok parsing, load..." << std::endl;
		World->add_world_to_vbo();
		break;
	}
	std::cout << "load" << std::endl;
}