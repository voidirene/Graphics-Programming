#pragma once
#include <GL\glew.h>
#include <string>

class Texturing
{
public:

	Texturing();
	~Texturing();

	void InitializeTexture(const std::string& file);
	void UseTexture(unsigned int number); //uses up to 32 textures

private:

	GLuint textureHandler[3];
	unsigned int amountOfTextures;
};

