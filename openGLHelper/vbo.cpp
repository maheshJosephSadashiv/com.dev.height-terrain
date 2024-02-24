#include "vbo.h"

#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

VBO::VBO(int numVertices_, int numFloatsPerVertex_, const float * data,  const GLenum usage) : numVertices(numVertices_), numFloatsPerVertex(numFloatsPerVertex_)
{
  // Create the VBO handle 
  glGenBuffers(1, &handle);

  // Initialize the VBO.
  glBindBuffer(GL_ARRAY_BUFFER, handle);
  const int numBytes = numVertices * numFloatsPerVertex * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, numBytes, data, usage);
}

VBO::~VBO()
{
  // Delete the VBO.
  glDeleteBuffers(1, &handle);
}

void VBO::Bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, handle);
}

