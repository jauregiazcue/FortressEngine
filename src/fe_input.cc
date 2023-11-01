
// Author : jauregiaz
#include "fe_input.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

std::unordered_map<uint16_t, key_state> FEInput::keymap_;
std::unordered_map<uint16_t, key_state> FEInput::mousekeymap_;

void keyCallback( GLFWwindow* window, const int key, const int scancode, const int action, const int mods ) {
  auto iterator = FEInput::keymap_.find( key );
  if(iterator != FEInput::keymap_.end()) {
    
    if(action == GLFW_PRESS) {
      iterator->second.down = true;
    }
    if(action == GLFW_RELEASE) {
      iterator->second.down = false;
    }
    
  }
 

}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window,button,action,mods);

  auto iterator = FEInput::mousekeymap_.find(button);
  if (iterator != FEInput::mousekeymap_.end()) {

    if (action == GLFW_PRESS) {
      iterator->second.down = true;
    }
    if (action == GLFW_RELEASE) {
      iterator->second.down = false;
    }

  }
}



void FEInput::init() {
  key_state state;
  state.down = false;
  state.change = false;
  state.previous_down = false;
  keymap_[GLFW_KEY_W] = state;
  keymap_[GLFW_KEY_A] = state;
  keymap_[GLFW_KEY_S] = state;
  keymap_[GLFW_KEY_D] = state;
  keymap_[GLFW_KEY_Q] = state;

  keymap_[GLFW_KEY_0] = state;
  keymap_[GLFW_KEY_1] = state;
  keymap_[GLFW_KEY_2] = state;
  keymap_[GLFW_KEY_3] = state;
  keymap_[GLFW_KEY_4] = state;
  keymap_[GLFW_KEY_5] = state;
  keymap_[GLFW_KEY_6] = state;
  keymap_[GLFW_KEY_7] = state;
  keymap_[GLFW_KEY_8] = state;
  keymap_[GLFW_KEY_9] = state;

  keymap_[GLFW_KEY_UP] = state;
  keymap_[GLFW_KEY_DOWN] = state;
  keymap_[GLFW_KEY_LEFT] = state;
  keymap_[GLFW_KEY_RIGHT] = state;

  keymap_[GLFW_KEY_ENTER] = state;

  mousekeymap_[GLFW_MOUSE_BUTTON_LEFT] = state;
  mousekeymap_[GLFW_MOUSE_BUTTON_RIGHT] = state;
  mousekeymap_[GLFW_MOUSE_BUTTON_MIDDLE] = state;
}

bool FEInput::keyUp( const Key k ) {
  key_state* km = &keymap_[(int)k];
  return !km->previous_down;
}

bool FEInput::keyDown( const Key k ) {
  key_state* km = &keymap_[(int)k];
  return km->previous_down;
}

bool FEInput::keyPress( const Key k ) {
  key_state* km = &keymap_[(int)k];
  return  km->previous_down && km->change;
}

bool FEInput::keyRelease( const Key k ) {
  key_state* km = &keymap_[(int)k];
  return !km->previous_down && km->change;
}

bool FEInput::mouseKeyUp(const Mouse k) {
  key_state* km = &mousekeymap_[(int)k];
  return !km->previous_down;
}

bool FEInput::mouseKeyDown(const Mouse k) {
  key_state* km = &mousekeymap_[(int)k];
  return km->previous_down;
}

bool FEInput::mouseKeyPress(const Mouse k) {
  key_state* km = &mousekeymap_[(int)k];
  return  km->previous_down && km->change;
}

bool FEInput::mouseKeyRelease(const Mouse k) {
  key_state* km = &mousekeymap_[(int)k];
  return !km->previous_down && km->change;
}



void FEInput::SetCallback( GLFWwindow* window ) {
  glfwSetKeyCallback( window, keyCallback );
  glfwSetMouseButtonCallback(window, mouseCallback);
}

void FEInput::nextFrame() {
  for (int i = 0; i < keymap_.size(); i++) {
    key_state* key = &keymap_[(int)i];
    key->change = key->down != key->previous_down;
    key->previous_down = key->down;

    key = &mousekeymap_[(int)i];
    key->change = key->down != key->previous_down;
    key->previous_down = key->down;
  }
}








