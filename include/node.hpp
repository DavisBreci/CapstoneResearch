#pragma once

#include<string>
using std::string;
#include<vector>
using std::vector;
#include<unordered_map>
using std::unordered_map;
#include<chrono>
using namespace std::chrono;
#include "transform.hpp"
#include "screenbuffer.hpp"
#include "utils.hpp"

#ifndef NODE_HPP
#define NODE_HPP

struct DrawableNode {
	virtual void draw(ScreenBuffer*, Transform) = 0;
	virtual ~DrawableNode() = default;
};

/*
Nodes are the fundamental data type. The data stored is very minimal, so the base Node type is
not very useful for development, but all of its data cascades down via inheritance.
*/
struct Node {
	string name;
	int id;
	bool visibility;
	vector<Node*> children;
	Node(string _name) {
		name = _name;
//		id = utils::new_id()
		id = 0;
		visibility = 1;
		children = vector<Node*>();
	}

/* All Nodes are capable of runtime polymorphism. For example, a Sprite can be considered a Node for
   most of it's lifetime, but can be dynamically casted down to a Sprite whenever it needs to draw. */
	virtual ~Node() = default;
};

/*
SpatialNodes are Nodes that have transform data to be used in the spatial plane.
*/
struct SpatialNode : Node {
	Transform transform;
	SpatialNode(string _name) : Node(_name) {
		transform = Transform();
	}
};

/*
A Sprite is an image that can be moved anywhere on the screen. Sprite's texture is loaded on startup,
either as it's own image
*/
struct Sprite : SpatialNode, DrawableNode {
	int u, v, w, h;
	Texture* texture;
// this constructor can be used when the sprite should be the entire texture
	Sprite(string _name, string filename) : SpatialNode(_name) {
		texture = utils::load_texture(filename);
		u = 0;
		v = 0;
		w = texture->width;
		h = texture->height;
	}
// this constructor must be used when using a sprite sheet
	Sprite(string _name, string filename, int _u, int _v, int _w, int _h) : SpatialNode(_name) {
		set_texture(filename);
		set_uvwh(_u, _v, _w, _h);
	}
	void set_texture(string filename) {
		texture = utils::load_texture(filename);
	}
	void set_uvwh(int _u, int _v, int _w, int _h) {
		if(texture == nullptr)
			return;
		u = _u < 0 ? 0 : _u > texture->width ? texture->width : _u;
		v = _v < 0 ? 0 : _v > texture->height ? texture->height : _v;
		w = (u + _w > texture->width) ? texture->width - u : _w;
		h = (v + _h > texture->height) ? texture->height - v : _h;
	}
	~Sprite() override {
		free(texture);
	}
	void draw(ScreenBuffer*, Transform) override;
};

typedef struct {
	int frame_index;
	float frame_end_time = 0.0f;  // relative to the start of the animation
} Frame;

struct AnimatedSprite : Sprite {
	typedef struct {
		milliseconds current_animation_start_time;
		string current_animation = "default";
		float current_animation_total_time;
		int current_animation_total_frames;
		int current_frame_index = 0;
		bool play_on_this_frame = false;
	} AnimationState;

	AnimationState state;
	unordered_map<string, vector<Frame>> animation_table;
	int frames_x, frames_y;

	AnimatedSprite(string _name, string spritesheet_filename, int _w, int _h) : Sprite(_name, spritesheet_filename) {
		if(texture->width % _w == 0 && texture->height % _h == 0) {
			w = _w;
			h = _h;
		}
		frames_x = texture->width/w;
		frames_y = texture->height/h;
	}
	void set_frames(string animation, vector<Frame> frames) {
		animation_table[animation] = frames;
	}
/*
	void reset_frames(string animation) {
		animation_table[animation].clear()
	}
	void append_frames(string animation, vector<Frame> frames) {
		animation_table[animation].insert(animation_table[animation].end(), frames.begin(), frames.end());
	}
*/
	void play(string animation) {
		if(animation != state.current_animation) {
			state.current_animation = animation;
			state.current_animation_total_frames = animation_table[state.current_animation].size();
			state.current_animation_total_time = animation_table[state.current_animation].at(state.current_animation_total_frames-1).frame_end_time;
			state.current_frame_index = 0;
			state.current_animation_start_time = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
		}
		state.play_on_this_frame = true;
	}
	void stop() {
		state.current_animation = "";
		state.play_on_this_frame = false;
	}
	int last_int_div = 0;
	void draw(ScreenBuffer*, Transform) override;
};

#endif