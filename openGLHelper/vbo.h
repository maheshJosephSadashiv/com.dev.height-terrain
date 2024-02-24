#ifndef _VBO_H_
#define _VBO_H_

#include "openGLHeader.h"

/*
  CSCI 420 Computer Graphics, University of Southern California
  Jernej Barbic, 2024

  "Vertex Buffer Object" (VBO) helper class. 
  It creates and manages VBOs.
*/

class VBO
{
public:

  // Initialize the VBO.
  // "numVertices": the number of vertices
  // "numFloatsPerVertex": the number of floating point values per vertex; e.g. 3 for vertex positions, 4 for colors, and 2 for texture coordinates.
  // "usage" must be either GL_STATIC_DRAW or GL_DYNAMIC_DRAW
  VBO(int numVertices, int numFloatsPerVertex, const float * data, const GLenum usage = GL_STATIC_DRAW);
  virtual ~VBO();

  // Binds (activates) this VBO.
  void Bind();

  // Get handle to this VBO.
  GLuint GetHandle() { return handle; }
  // Get the number of vertices in this VBO.
  int GetNumVertices() { return numVertices; }
  // Get the number of floating point values per vertex in this VBO.
  int GetNumFloatsPerVertex() { return numFloatsPerVertex; }

protected:
  GLuint handle; // the handle to the VBO
  int numVertices;
  int numFloatsPerVertex;
};

#endif

