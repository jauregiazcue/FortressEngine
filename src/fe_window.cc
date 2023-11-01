
// Author : jauregiaz


#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "fe_window.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "fe_input.h"

#include "fe_constants.h"


void GLFWindowDestroy::operator()(GLFWwindow* ptr) { glfwDestroyWindow(ptr); }

FEWindow::FEWindow() {

}

FEWindow::~FEWindow() {
  glfwTerminate();
}

int FEWindow::init() {
  
  if(!glfwInit()) {
    return -1;
  }
  // Create a windowed mode window and its OpenGL context 
  window_.reset(glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowName, NULL, NULL));
  if(!window_.get()) {
    glfwTerminate();
    return -1;
  }

  // Make the window's context current 
  glfwMakeContextCurrent( window_.get() );

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  gladLoadGL(glfwGetProcAddress);

  glfwSetFramebufferSizeCallback(window_.get(), resizeWindow);


  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  FEInput::init();

  // setup platform/renderer bindings
  if (!ImGui_ImplGlfw_InitForOpenGL(window_.get(), true)) { return -1; }
  if (!ImGui_ImplOpenGL3_Init()) { return -1; }
  return 0;
}



bool FEWindow::isDone() {
  if(glfwWindowShouldClose(window_.get())) {
    return true;
  } else {
    return false;
  }
}


void FEWindow::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(kClearColor.x, kClearColor.y, kClearColor.z, kClearColor.w);

  FEInput::SetCallback(window_.get());
}

void FEWindow::swap() {

  // Swap front and back buffers 
  glfwSwapBuffers(window_.get());

  // Poll for and process events 
  glfwPollEvents();
  FEInput::nextFrame();
}

void resizeWindow(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);

}







