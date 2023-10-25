

#ifndef __FEINPUT_H__
#define __FEINPUT_H__ 1

#include <GLFW/glfw3.h>
#include <unordered_map>

struct GLFWwindow;

enum class Key {
	KEY_W = GLFW_KEY_W,
	KEY_A = GLFW_KEY_A,
	KEY_S = GLFW_KEY_S,
	KEY_D = GLFW_KEY_D,
	KEY_Q = GLFW_KEY_Q,

	KEY_0 = GLFW_KEY_0,
	KEY_1 = GLFW_KEY_1,
	KEY_2 = GLFW_KEY_2,
	KEY_3 = GLFW_KEY_3,
	KEY_4 = GLFW_KEY_4,
	KEY_5 = GLFW_KEY_5,
	KEY_6 = GLFW_KEY_6,
	KEY_7 = GLFW_KEY_7,
	KEY_8 = GLFW_KEY_8,
	KEY_9 = GLFW_KEY_9,

	KEY_UP = GLFW_KEY_UP,
	KEY_DOWN = GLFW_KEY_DOWN,
	KEY_LEFT = GLFW_KEY_LEFT,
	KEY_RIGHT = GLFW_KEY_RIGHT,

	KEY_ENTER = GLFW_KEY_ENTER

};

struct key_state {
	bool previous_down;
	bool down;
	bool change;
	
};

class FEInput {
public:
	
	static bool keyUp( const Key k);

	static bool keyDown( const Key k );

	static bool keyPress( const Key k );

	static bool keyRelease( const Key k );

	
	private :

	static std::unordered_map<uint16_t, key_state> keymap_;


	static void init();

	friend void keyCallback( GLFWwindow* window, const int key, const int scancode,const int action, const int mods );

	static void SetCallback( GLFWwindow* window );

	static void nextFrame();

	friend class FEWindow;

};

#endif //__FEINPUT_H__