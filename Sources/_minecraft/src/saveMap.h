#pragma once
#include "world.h"
#include <iostream>
#include <fstream>  
#include <sstream>
#include <string>

//écrit un int dans un binaire
template<typename T>
std::ostream& binary_write(std::ostream& stream, const T& value)
{
	return stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

//lit un int dans un binaire
template<typename T>
std::istream & binary_read(std::istream& stream, T& value) {
	return stream.read(reinterpret_cast<char*>(&value), sizeof(T));
}

class SaveMap
{
public:
	NYWorld * World;


	SaveMap(NYWorld * World);
	~SaveMap();

	void saveNewMap();
	void loadLastMap();
};

