#include <fe_framebuffer.h>
#include <cassert>
#include <fe_constants.h>

FEFramebuffer::FEFramebuffer() {
	glGenFramebuffers(1, &id_);
	glBindFramebuffer(GL_FRAMEBUFFER, id_);


  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kWindowWidth, kWindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);


	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
	"Error : FEFramebuffer : framebuffer is not complete");




	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FEFramebuffer::~FEFramebuffer() {
}
