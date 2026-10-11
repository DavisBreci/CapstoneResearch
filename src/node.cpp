#include "node.hpp"
#include<cmath>

void Sprite::draw(ScreenBuffer* screen, Transform absolute) {
	int drawn_width = absolute.x + w > screen->width ? screen->width - absolute.x : w;
	int drawn_height = absolute.y + h > screen->height ? screen->height - absolute.y : h;
	for(int row = 0; row < drawn_height; ++row)
		for(int col = 0; col < drawn_width; ++col)
			screen->draw_buffer[(absolute.y+row)*screen->width + (absolute.x+col)] = texture->pixel_array[(row+v)*texture->width + (col+u)];
}

void AnimatedSprite::draw(ScreenBuffer* screen, Transform absolute) {
	if(state.play_on_this_frame) {
		milliseconds end = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
		float diff = static_cast<float>((end - state.current_animation_start_time).count())/1000;
		bool carry = false;
		int int_div = (int)(diff / state.current_animation_total_time);
		if(int_div > last_int_div) {
			carry = true;
			last_int_div = int_div;
		}
		diff = std::fmod(diff,state.current_animation_total_time);
		if(carry || diff > animation_table[state.current_animation].at(state.current_frame_index).frame_end_time) {
			state.current_frame_index = carry ? 0 : state.current_frame_index + 1;
		}
		int index = state.current_frame_index;
		int frame_index = animation_table[state.current_animation].at(index).frame_index;
		int x = frame_index % frames_x;
		int y = frame_index / frames_x;
		u = x * w;
		v = y * h;
	}
	Sprite::draw(screen, absolute);
	state.play_on_this_frame = false;
}