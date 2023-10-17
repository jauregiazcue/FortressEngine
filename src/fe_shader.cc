
// Author : jauregiaz
#include "fe_shader.h"

#include <glad/gl.h>
#include <fstream>
#include <vector>



std::string FEShader::ReadFile(std::string file_source) {

  //Open file, put pointer in reading at the end (ate)
  std::ifstream file(file_source, std::ios::binary | std::ios::ate);

  //Copy the position of the pointer of reading, and it tells us the size
  //due to being in the end
  std::streamsize size = file.tellg();

  //Create a buffer with the necesary size for the file
  auto buffer = std::make_unique<char[]>(size);

  //Move the pointer to the start of the file
  file.seekg(0, std::ios::beg);

  // Read the file to the buffer
  if (!file.read(buffer.get(), size)) {
    //Something went wrong
    correctly_made_ = false;

  }
  std::string s(buffer.get(), buffer.get() + size);

  return s;
}

bool FEShader::isCompiled() {
  return correctly_made_;
}

FEShader::FEShader(std::string shader_source, ShaderType type) {
  correctly_made_ = true;
  switch (type) {
  case ShaderType::Vertex:
    shader_ = glCreateShader(GL_VERTEX_SHADER);

    break;
  case ShaderType::Fragment:
    shader_ = glCreateShader(GL_FRAGMENT_SHADER);

    break;

  default:
    break;
  }

  auto source = ReadFile(shader_source);
  const char* sourceptr = source.c_str();

  if (correctly_made_) {
    glShaderSource(shader_, 1, &sourceptr, NULL);

    glCompileShader(shader_);

  }

  GLint isCompiled = 0;
  glGetShaderiv(shader_, GL_COMPILE_STATUS, &isCompiled);

  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &maxLength);


    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader_, maxLength, &maxLength, &errorLog[0]);


    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(shader_); // Don't leak the shader.


  }

}




FEShader::~FEShader() {

}


