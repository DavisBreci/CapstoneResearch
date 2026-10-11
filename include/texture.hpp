#pragma once

#include<cstdlib>
using std::calloc;
using std::free;

#ifndef TEXTURE_HPP
#define TEXTURE_HPP
struct Texture {
	unsigned int* pixel_array;
	int width;
	int height;
	Texture(int _width, int _height) : width(_width), height(_height) {
		pixel_array = (unsigned int*)calloc(width*height, sizeof(unsigned int));
	}
	~Texture() {
		free(pixel_array);
	}
};
#endif