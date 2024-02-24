#ifndef _PIPELINE_PROGRAM_H_
#define _PIPELINE_PROGRAM_H_

#include "openGLHeader.h"

/*
  CSCI 420 Computer Graphics, University of Southern California
  Jernej Barbic, Bohan Wang, 2020

  Shader program helper class. 
  It loads shaders and properly initializes them in OpenGL.
*/

/* This class stores a set of shaders that serve as the stages of the rendering pipeline. 
   The stages are:
     (A) vertex shader (OpenGL 2.0 and higher),
     (B) fragment shader (OpenGL 2.0 and higher),
     (C) geometry shader (OpenGL 3.2 and higher),
     (D) tessellation control shader (OpenGL 4.0 and higher),
     (E) tessellation evaluation shader (OpenGL 4.0 and higher)

   Note: this is not the actual order of the stages in the pipeline. This order
   is used for historic reasons and to reflect the popularity of each stage,
   with (A) most popularly used, and (E) least popular.
   The actual pipeline order is:
   (A), (D), (E), (C), (B)
  
   The shaders are provided either by loading them from a file, or from a C string.
*/

class PipelineProgram
{
public:
  PipelineProgram();
  virtual ~PipelineProgram();

  // Loads the shaders, compiles them, and links them into a pipeline program
  // If a shader is not provided, the pointer should be set to nullptr on input.
  // The provided shaders must come from one of the following choices:
  // (A), (B)
  // (A), (B), (C)
  // (A), (B), (D), (E)
  // (A), (B), (C), (D), (E)
  //
  // Load shaders from a text file.
  // All shaders must reside in the same folder, denoted by "filenameBasePath".
  int BuildShadersFromFiles(const char * filenameBasePath,
                            const char * vertexShaderFilename,
                            const char * fragmentShaderFilename,
                            const char * geometryShaderFilename = nullptr,
                            const char * tessellationControlShaderFilename = nullptr,
                            const char * tessellationEvaluationShaderFilename = nullptr);
  // Load shaders from a C text string.
  int BuildShadersFromStrings(const char * vertexShaderCode,
                              const char * fragmentShaderCode,
                              const char * geometryShaderCode = nullptr,
                              const char * tessellationControlShaderCode = nullptr,
                              const char * tessellationEvaluationShaderCode = nullptr);

  // Binds (activates) this pipeline program. This means that all the shaders in this pipeline program become active.
  void Bind();

  // Get the handle to this pipeline program. This is used to disambiguate one pipeline program from another. 
  GLuint GetProgramHandle() { return programHandle; }

  // Get the handle to the uniform shader variable named "name".
  // Uniform variables are shader variables that the *same* for all the vertices, such as the modelview matrix or the projection matrix.
  // Note: this routine will malfunction if you attempt to obtain a handle to a non-uniform variable such as an attribute.
  GLint GetUniformVariableHandle(const char * name); 

  // Set the uniform variable of type "int". 
  void SetUniformVariablei(const char * name, int value);
  // Set the uniform variable of type "float". 
  void SetUniformVariablef(const char * name, float value);
  // Set the uniform variable of type "4x4 matrix of floats" (column-major).
  void SetUniformVariableMatrix4fv(const char * name, GLboolean transpose, float value[16]);

protected:
  GLuint programHandle; // the handle to the pipeline program

  // Load shader from a file into a string.
  int LoadShader(const char * filename, char * code, int len); 

  // Compile a shader.
  // input:
  //   code: the shader code
  //   shaderType: the type of shader: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER
  // output:
  //   shaderHandle: the handle to the compiled shader
  //   return value: 0=success, non-zero: failure
  int CompileShader(const char * code, GLenum shaderType, GLuint & shaderHandle); // compile a shader

  virtual int PreLink(); // Custom operations to perform before linking (nothing in most cases).
};

#endif

