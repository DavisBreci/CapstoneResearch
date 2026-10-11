#include "transform.hpp"
#include "screenbuffer.hpp"
#include "node.hpp"
#include<MiniFB.h>
#include<string>
#include<vector>
#include<iostream>
using std::string;
using std::vector;
using std::cout;
using std::endl;
#include<cstdlib>
using std::free;
#include<chrono>
#include<thread>

// MiniFB setup
#define kUnused(var) (void) var
#define TEST_TAG "fullscreen"
#define WIDTH 250
#define HEIGHT 200
static bool g_active = true;

struct Scene {
	string name;
	int scene_id;
	Node* root;
	Scene(string _name) {
		name = _name;
//		scene_id = new_scene_id();
		scene_id = 0;
		root = NULL;
	}
};
/*
void printNode(Node* n, int layer) {
	if(n == NULL) {
		return;
	}
	for(int j = 0; j < layer; ++j)
			cout << "-";
	cout << n->name << endl;
	for(int i = 0; i < n->children.size(); ++i) {
		printNode(n->children.at(i), layer+1);
	}
}

void printScene(Scene* s) {
	cout << s->name << ": " << endl;
	printNode(s->root, 0);
}
*/
void printScreen(ScreenBuffer* screen) {
	for(int row = 0; row < screen->height; ++row) {
		for(int col = 0; col < screen->width; ++col) {
			cout << ((screen->draw_buffer[row*screen->width+col] == 255) ? ' ' : '1');
		}
		cout << endl;
	}
}
// converts RRGGBBAA format to 00RRGGBB for use with MiniGB
void convertPixelFormat(ScreenBuffer* screen) {
	for(int i = 0; i < screen->width*screen->height; ++i)
		screen->display_buffer[i] = screen->display_buffer[i] >> 8;
}

struct OrderingTable {
	struct NodeTransformPair {
		Node* node;
		Transform absolute;
	};
  	const int Z = 4;
	vector<vector<Node*>> nodes;
	vector<vector<Transform>> transforms;
	NodeTransformPair drawing_data;
	OrderingTable() {
		nodes = vector<vector<Node*>>(Z, vector<Node*>());
		transforms = vector<vector<Transform>>(Z, vector<Transform>());
		drawing_data.node = NULL;
		drawing_data.absolute = Transform();
	}
	void push_back(Node* n, Transform absolute) {
		if(absolute.z>=Z)
			absolute.z = Z-1;
		else if(absolute.z < 0)
			absolute.z = 0;
		nodes.at(absolute.z).push_back(n);
		transforms.at(absolute.z).push_back(absolute);
	}
	bool pop() {
		int i = 0;
		while(i < Z && nodes.at(i).size() == 0)
			i++;
		if(i >= Z) {
			drawing_data.node = NULL;
			drawing_data.absolute = Transform();
			return 0;
		}
		drawing_data.node = nodes.at(i).at(nodes.at(i).size()-1);
		nodes.at(i).pop_back();
		drawing_data.absolute = transforms.at(i).at(transforms.at(i).size()-1);
		transforms.at(i).pop_back();
		return 1;
	}
};

void calculateNodes(int width, int height, OrderingTable* ot, Transform absolute, Node* n) {
	if(dynamic_cast<SpatialNode*>(n) != nullptr) {
		SpatialNode* n_ = dynamic_cast<SpatialNode*>(n);
		absolute.x += n_->transform.x;
		absolute.y += n_->transform.y;
		absolute.z += n_->transform.z;
		absolute.rotation += n_->transform.rotation;
		absolute.scale *= n_->transform.scale;
	}  // else absolute doesn't change
	for(int i = 0; i < (int)(n->children.size()); ++i)
		calculateNodes(width, height, ot, absolute, n->children.at(i));
	if(dynamic_cast<DrawableNode*>(n) != nullptr && absolute.x < width && absolute.y < height)
		ot->push_back(n, absolute);
}

void generateDisplayBuffer(ScreenBuffer* screen, OrderingTable* ot, Scene* scene) {
	Transform absolute = Transform();
	if(scene->root != NULL) {
		calculateNodes(screen->width, screen->height, ot, absolute, scene->root);
	}
	while(ot->pop()) {
		DrawableNode* drawable = dynamic_cast<DrawableNode*>(ot->drawing_data.node);
		drawable->draw(screen, ot->drawing_data.absolute);
	}
	screen->swap_buffers();
//	printScreen(screen);
}

int main()  {
	ScreenBuffer screen(WIDTH, HEIGHT);
	mfb_update_state state;
	OrderingTable ot = OrderingTable();
	Scene s = Scene("scene");

	Node* n = new Node("root");
	SpatialNode* o = new SpatialNode("child1");
		o->transform.x = 120;
		o->transform.y = 40;
		o->transform.z = 3;
	AnimatedSprite* q = new AnimatedSprite("grandchild1", "spritesheet.png", 5, 5);
		q->transform.x = -20;
		q->transform.y = -20;
		q->transform.z = -1;
		string animation = "my_animation";
		vector<Frame> frames;
		Frame f1; f1.frame_index = 0; f1.frame_end_time = 0.1; frames.push_back(f1);
		Frame f2; f2.frame_index = 1; f2.frame_end_time = 0.2; frames.push_back(f2);
		Frame f3; f3.frame_index = 2; f3.frame_end_time = 0.3; frames.push_back(f3);
		Frame f4; f4.frame_index = 3; f4.frame_end_time = 0.4; frames.push_back(f4);
		q->set_frames(animation, frames);

	s.root = n;
	n->children.push_back((Node*)o);
	o->children.push_back((Node*)q);

	MFB_LOGI(TEST_TAG, "Full Screen Window");
	struct mfb_window *window = mfb_open("full screen auto", WIDTH, HEIGHT);
	if(!window) {
		MFB_LOGE(TEST_TAG, "Cannot create window");
		return 0;
	}
	mfb_set_viewport_best_fit(window, WIDTH, HEIGHT);

	do {
		if(g_active) {
			q->play(animation);
			generateDisplayBuffer(&screen, &ot, &s);
			convertPixelFormat(&screen);
			mfb_update_ex(window, screen.display_buffer, WIDTH, HEIGHT);
		} else {
			state = mfb_update_events(window);
		}
		if(state != MFB_STATE_OK) {
			window = nullptr;
			break;
		}
	} while (mfb_wait_sync(window));
	delete q;
	return 0;
}