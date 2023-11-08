#include <fe_framebuffer.h>
#include <cassert>
#include <fe_constants.h>

FEFramebuffer::FEFramebuffer() {
	glGenFramebuffers(1, &id_);
	glBindFramebuffer(GL_FRAMEBUFFER, id_);

  //Need to safe the depth else the rendering looks from back to front instead of front to back
  glGenTextures(1, &depth_texture_id);
  glBindTexture(GL_TEXTURE_2D, depth_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, kWindowWidth, kWindowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);
  
  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, kWindowWidth, kWindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);


	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
	"Error : FEFramebuffer : framebuffer is not complete");


  
  

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FEFramebuffer::~FEFramebuffer() {
}

bool FEFramebuffer::isResizeNeeded( float newWidth, float newHeight ) {
  if( newWidth == texture_width_ ) return false;
  if( newHeight == texture_height_ ) return false;
  return true;
}

void FEFramebuffer::resizeTextures( float newWidth, float newHeight ) {
  

  glBindTexture( GL_TEXTURE_2D, depth_texture_id );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
    newWidth, newHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
  glBindTexture( GL_TEXTURE_2D, 0 );

  glBindTexture( GL_TEXTURE_2D, texture_id_ );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, 
    newWidth, newHeight, 0, GL_RGBA, GL_FLOAT, NULL );
  glBindTexture( GL_TEXTURE_2D, 0 );
}
