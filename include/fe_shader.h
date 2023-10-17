
// Author : jauregiaz
#ifndef __FESHADER_H__
#define __FESHADER_H__ 1

#include <glad/gl.h>
#include <string>
#include <memory>

enum class ShaderType {
	Vertex = 0,
	Fragment = 1,

};

class FEShader{
public :

	FEShader(std::string shader_source,ShaderType type);

	~FEShader();

	std::string ReadFile(std::string file_source);

	bool isCompiled();

private :
	GLint shader_;
	bool correctly_made_;
	friend class FEProgram;


};


#endif //__FESHADER_H__


