#include "Texturing.h"
#include "stb_image.h"
#include <cassert>
#include <iostream>

Texturing::Texturing()
{
	amountOfTextures = 0;
}

Texturing::~Texturing()
{
	for (unsigned int i = 0; i < amountOfTextures; i++)
	{
		glDeleteTextures(1, &textureHandler[i]); //number of and address of textures
	}
}

void Texturing::InitializeTexture(const std::string& file)
{
	int width;
	int height;
	int numberOfComponents;

	//load the image from file
	unsigned char* textureData = stbi_load((file).c_str(), &width, &height, &numberOfComponents, 4); //4 is the required components (not important to us)

	//check if it loaded
	if (textureData == NULL)
	{
		std::cerr << "Failed to load texture" << file << '\n';
	}

	//generate the buffer to store the texture in openGL
	glGenTextures(1, &textureHandler[amountOfTextures]); //number of and address of textures
	glBindTexture(GL_TEXTURE_2D, textureHandler[amountOfTextures]); //bind texture - define type & specify the texture it should work with

	//set the parameters to control texture wrapping and linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //wrap texture outside width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //wrap texture outside height
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //linear filtering for minification(texture is smaller than area)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //linear filtering for magnification(texture is larger)

	//send the texture to the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData); //Target, Mipmapping Level, Pixel Format, Width, Height, Border Size, Input Format, Data Type of Texture, Texture Data

	stbi_image_free(textureData); //delete the data from CPU

	amountOfTextures += 1;
}

void Texturing::UseTexture(unsigned int number)
{
	assert(number >= 0 && number <= 31); //check that we are working with one of the 32 textures

	glBindTexture(GL_TEXTURE_2D, textureHandler[number]); //type of and texture to bind to unit
}