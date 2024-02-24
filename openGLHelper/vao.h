#ifndef _VAO_H_
#define _VAO_H_

#include "openGLHeader.h"
#include "pipelineProgram.h"
#include "vbo.h"

/*
  CSCI 420 Computer Graphics, University of Southern California
  Jernej Barbic, 2024

  "Vertex Array Object" (VAO) helper class. It creates and manages VAOs.
*/

class VAO
{
public:

  VAO();
  virtual ~VAO();

  // Binds (activates) the VAO.
  // At any given moment of time, exactly one VAO is bound. The purpose of binding a VAO is to specify *which* VAO
  // out of potentially many VAOs we want to operate on, and which one we want to use for rendering.
  void Bind();

  // Set up a shader variable in a pipeline program, by connecting it to a VBO.
  // Without this call, data won't correctly flow to the shaders. It must be repeated for every shader variable that is an attribute (e.g., vertex positions, colors, texture coordinates, etc.).
  // Note: this function has a side-effect: after calling it, "pipelineProgram" and "vbo" will be bound.
  void ConnectPipelineProgramAndVBOAndShaderVariable(PipelineProgram * pipelineProgram, VBO * vbo, const char * shaderVariable);

  // get handle
  GLuint GetHandle() { return handle; }

protected:
  GLuint handle; // the handle to the VAO
};

#endif

