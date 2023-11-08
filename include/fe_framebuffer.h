#ifndef __FEFRAMEBUFFER_H__
#define __FEFRAMEBUFFER_H__ 1

#include <glad/gl.h>
#include <fe_window.h>

class FEFramebuffer {
public:
	

	FEFramebuffer();

	~FEFramebuffer();

	bool isResizeNeeded( float newWidth, float newHeight );
	void resizeTextures(float newWidth, float newHeight);

	GLuint id_;
	GLuint texture_id_;
	GLuint depth_texture_id;
	float texture_width_;
	float texture_height_;
};


#endif //__FEFRAMEBUFFER_H__