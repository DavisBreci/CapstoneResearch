#include "utils.hpp"
#include<vector>
using std::vector;
#include<iostream>
using std::cout;
using std::endl;

namespace utils {
	Texture* load_texture(std::string filename) {
		if(loaded_textures[filename] != nullptr)
			return loaded_textures[filename];
		vector<unsigned char> image8;
		unsigned w, h;
		unsigned error = lodepng::decode(image8, w, h, filename);
		if(error) {
			cout << "Decoder Error " << error << ": " << lodepng_error_text(error) << endl;
			return nullptr;
		}
		int width = static_cast<int>(w);
		int height = static_cast<int>(h);
		unsigned int* image32 = (unsigned int*)malloc(sizeof(int)*width*height);
		for(int i = 0; i < width*height; ++i) {
			unsigned int r = static_cast<unsigned int>(image8.at(4*i)) << 24;
			unsigned int g = static_cast<unsigned int>(image8.at(4*i+1)) << 16;
			unsigned int b = static_cast<unsigned int>(image8.at(4*i+2)) << 8;
			unsigned int a = static_cast<unsigned int>(image8.at(4*i+3));
			image32[i] = (r + g + b + a);
		}
		Texture* txt = new Texture(width, height);
		txt->pixel_array = image32;
		loaded_textures[filename] = txt;
		return txt;
	}
}