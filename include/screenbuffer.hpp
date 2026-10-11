#pragma once

#include<cstdlib>

#ifndef SCREENBUFFER_HPP
#define SCREENBUFFER_HPP
struct ScreenBuffer {
	int width;
	int height;
	unsigned int* draw_buffer;
	unsigned int* display_buffer;
	ScreenBuffer(int _width, int _height) {
		width = _width;
		height = _height;
		draw_buffer = (unsigned int*)malloc(sizeof(unsigned int)*width*height);
		display_buffer = (unsigned int*)malloc(sizeof(unsigned int)*width*height);
		for(int i = 0; i < width*height; ++i) {
			draw_buffer[i] = 0x000000FF;
			display_buffer[i] = 0x000000FF;
		}
	}
	~ScreenBuffer() {
		free(draw_buffer);
		free(display_buffer);
	}
	void swap_buffers() {
		unsigned int* temp = draw_buffer;
		draw_buffer = display_buffer;
		display_buffer = temp;
	}
};
#endif