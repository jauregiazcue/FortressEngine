#include "fe_program.h"
#include <cassert>


FEProgram::FEProgram(const std::vector<FEShader>& list) {
  id_ = glCreateProgram();
	assert(glGetError() == GL_NO_ERROR);
  for (int i = 0; i < list.size(); i++) {
    glAttachShader(id_, list[i].shader_);
  }
	glLinkProgram(id_);
	assert(glGetError() == GL_NO_ERROR);
	GLint isLinked = 0;
	glGetProgramiv(id_, GL_LINK_STATUS, &isLinked);
	assert(glGetError() == GL_NO_ERROR);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &maxLength);
		assert(glGetError() == GL_NO_ERROR);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(id_, maxLength, &maxLength, &infoLog[0]);
		assert(glGetError() == GL_NO_ERROR);

		// The program is useless now. So delete it.
		glDeleteProgram(id_);
		assert(glGetError() == GL_NO_ERROR);
	}
}

GLint FEProgram::getId() {
  return id_;
}
