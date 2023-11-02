#ifndef __FEFRAMEBUFFER_H__
#define __FEFRAMEBUFFER_H__ 1

#include <glad/gl.h>

class FEFramebuffer {
public:
	

	FEFramebuffer();

	~FEFramebuffer();

	GLuint id_;
	GLuint texture_id_;
	GLuint depth_texture_id;
};


#endif //__FEFRAMEBUFFER_H__