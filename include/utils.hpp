#pragma once

#include<string>
#include<unordered_map>
#include "texture.hpp"
#include "lodepng.h"

namespace utils {
	static std::unordered_map<std::string, Texture*> loaded_textures;
	Texture* load_texture(std::string filename);
}