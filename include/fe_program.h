// Author : jauregiaz
#ifndef __FEPROGRAM_H__
#define __FEPROGRAM_H__ 1

#include <glad/gl.h>
#include <vector>
#include "fe_shader.h"

class FEProgram {
public :
	
	FEProgram(const std::vector<FEShader>& list);

	GLint getId();
private :
	GLint id_;
};


#endif //__FEPROGRAM_H__


