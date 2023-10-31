

#ifndef __FEWINDOW_H__
#define __FEWINDOW_H__ 1

#include <memory>
#include <glm/vec4.hpp>
struct GLFWwindow;

struct GLFWindowDestroy {
	void operator()(GLFWwindow* ptr);
};

class FEWindow {
public:
	

	FEWindow(glm::vec4 background_color);

	~FEWindow();

	int init();

	bool isDone();

	void swap();

	void clear();

	void changeBackgroundColor(glm::vec4 background_color);

	


	private :
	friend void resizeWindow(GLFWwindow* window, int width, int height);
	//the window of the engine
	std::unique_ptr<GLFWwindow,GLFWindowDestroy> window_;
	glm::vec4 background_color_;
	
	friend class FEWorld;
};


#endif //__FEWINDOW_H__