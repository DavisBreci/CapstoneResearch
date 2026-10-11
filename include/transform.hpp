#pragma once



#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP
struct Transform {
	int x;
	int y;
	int z;
	float rotation;
	float scale;
	Transform() {
		x = 0;
		y = 0;
		z = 0;
		rotation = 0.0f;
		scale = 0.0f;
	}
};
#endif