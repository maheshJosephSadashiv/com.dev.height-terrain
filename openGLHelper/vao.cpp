#include "vao.h"

#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

VAO::VAO()
{
  // Create the VAO handle 
  glGenVertexArrays(1, &handle);
}

VAO::~VAO()
{
  // Delete the VAO.
  glDeleteVertexArrays(1, &handle);
}

void VAO::Bind()
{
  glBindVertexArray(handle);
}

void VAO::ConnectPipelineProgramAndVBOAndShaderVariable(PipelineProgram * pipelineProgram, VBO * vbo, const char * shaderVariable)
{
  Bind(); // Bind this VAO.
  pipelineProgram->Bind(); // Bind the pipeline program. Needed so that "glVertexAttribPointer" knows in which pipeline program to look for the shader variables.
  vbo->Bind(); // The VBO that we bind here will be used in the glVertexAttribPointer calls below. If we forget to bind the VBO here, the program will malfunction.

  // Set up the relationship between the shader variable and the VAO.
  const GLuint locationOfShaderVariable = glGetAttribLocation(pipelineProgram->GetProgramHandle(), shaderVariable); // Obtain a handle to the shader variable.
  glEnableVertexAttribArray(locationOfShaderVariable); // Must always enable the vertex attribute. By default, it is disabled.
  const int stride = 0; // Stride is 0, i.e., data is tightly packed in the VBO.
  const GLboolean normalized = GL_FALSE; // Normalization is off.
  glVertexAttribPointer(locationOfShaderVariable, vbo->GetNumFloatsPerVertex(), GL_FLOAT, normalized, stride, (const void *)0); // The shader variable receives its data from the currently bound VBO (i.e., vbo), starting from offset 0 in the VBO. The number of float entries per vertex in the VBO is read from the VBO via "GetNumFloatsPerVertex".
}

