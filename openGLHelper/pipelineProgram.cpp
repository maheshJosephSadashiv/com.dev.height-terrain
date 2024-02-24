#include "pipelineProgram.h"

#include <cstring>
#include <cstdio>
#include <iostream>

using namespace std;

PipelineProgram::PipelineProgram() : programHandle(0) {}

PipelineProgram::~PipelineProgram()
{
  glDeleteProgram(programHandle);
}

int PipelineProgram::BuildShadersFromFiles(const char * filenameBasePath,
                                           const char * vertexShaderFilename,
                                           const char * fragmentShaderFilename,
                                           const char * geometryShaderFilename,
                                           const char * tessellationControlShaderFilename,
                                           const char * tessellationEvaluationShaderFilename)
{
  char * shaderCodes[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
  const char * filenames[5] = { vertexShaderFilename, fragmentShaderFilename, geometryShaderFilename, tessellationControlShaderFilename, tessellationEvaluationShaderFilename };

  for (int i = 0; i < 5; i++) 
  {
    // if filename not provided, skip that shader
    if (filenames[i] == nullptr) 
    {
      shaderCodes[i] = nullptr;
      continue;
    }

    // allocate space for shader code (128 Kb -- should be enough)
    shaderCodes[i] = new char[128 * 1024];
    
    // create the complete file path
    char * filepath = new char[strlen(filenames[i]) + strlen(filenameBasePath) + 5];
    strcpy(filepath, filenameBasePath);
    strcat(filepath, "/");
    strcat(filepath, filenames[i]);

    // load the shader into the shaderCodes string
    if (LoadShader(filepath, shaderCodes[i], 128 * 1024) != 0) 
    {
      cout << "Shader " << filepath << " file not found." << endl;
      delete[] filepath;
      return 1;
    }
    delete[] filepath;
  }

  int exitCode = BuildShadersFromStrings(shaderCodes[0], shaderCodes[1], shaderCodes[2], shaderCodes[3], shaderCodes[4]);
  for (int i = 0; i < 5; i++) 
  {
    delete [] (shaderCodes[i]);
    shaderCodes[i] = nullptr;
  }

  return exitCode;
}

int PipelineProgram::BuildShadersFromStrings(const char * vertexShaderCode,
                                             const char * fragmentShaderCode,
                                             const char * geometryShaderCode,
                                             const char * tessellationControlShaderCode,
                                             const char * tessellationEvaluationShaderCode)
{
  // create a program handle
  programHandle = glCreateProgram();
  if (programHandle == 0) 
  {
    cout << "Shader initialization failed." << endl;
    return -1;
  }

  // store the codes into one array
  const char * shaderCode[5] = { vertexShaderCode, fragmentShaderCode, geometryShaderCode, tessellationControlShaderCode, tessellationEvaluationShaderCode };

  GLuint h_shaders[5] = { 0, 0, 0, 0, 0 }; // shader handles to-be-created
  // OpenGL shader flags (macros are used to prevent a compile error in case the OpenGL version is too low and a symbolic constant is not defined)
  GLenum shaderFlags[5] = 
  {
    GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, 
    #if defined(GL_GEOMETRY_SHADER)
      GL_GEOMETRY_SHADER,
    #else
      0,
    #endif
    #if defined(GL_TESS_CONTROL_SHADER) && defined(GL_TESS_EVALUATION_SHADER)
       GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER
    #else
      0, 0
    #endif
  };

  // informative shader names
  char shaderName[5][64] = { "vertex shader", "fragment shader", "geometry shader", "tessellation control shader", "tessellation evaluation shader" };

  for (int i = 0; i < 5; i++)
  {
    // if code is not provided, skip this shader
    if (shaderCode[i] == nullptr)
      continue;

    cout << "Compiling " << shaderName[i] << "..." << endl;

    // compile the shader
    if (CompileShader(shaderCode[i], shaderFlags[i], h_shaders[i]) != 0)
    {
      cout << "Shader " << shaderName[i] << " compile error" << endl;
      return 1;
    }
    else 
    {
      // attach the shader to the pipeline program
      glAttachShader(programHandle, h_shaders[i]);
    }
  }

  if (PreLink() != 0) 
  {
    cout << "Pre-link shader code failed." << endl;
    return 1;
  }

  // link the program
  glLinkProgram(programHandle);
  
  int status;
  glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
  if (status == 0)
  {
    GLchar infoLog[512];
    glGetProgramInfoLog(programHandle, 512, nullptr, infoLog);
    cout << "Errors:\n" << infoLog << endl;
    return 1;
  }

  // the shaders are no longer needed after the program is linked
  for (int i = 0; i < 5; i++)
    glDeleteShader(h_shaders[i]);

  return 0;
}

int PipelineProgram::PreLink()
{
  return 0;
}

void PipelineProgram::Bind()
{
  glUseProgram(programHandle);
}

int PipelineProgram::LoadShader(const char * filename, char * code, int len)
{
  FILE * file = fopen(filename, "rb");
  if (file == nullptr)
    return 1;

  code[fread(code, 1, len, file)] = '\0';
  fclose(file);

  return 0;
}

int PipelineProgram::CompileShader(const char * shaderCode, GLenum shaderType, GLuint & shaderHandle)
{
  shaderHandle = glCreateShader(shaderType);

  if (shaderHandle == 0) 
  {
    cout << "Shader creation failed" << endl;
    return 1;
  }

  const int numShaderCodes = 1;
  const GLchar * shaderCodes[] = { shaderCode };
  GLint codeLength[] = { (GLint)strlen(shaderCode) };

  // compile the shader (the entire source is in one string)
  glShaderSource(shaderHandle, numShaderCodes, shaderCodes, codeLength);
  glCompileShader(shaderHandle);

  // check if compilation was successful
  GLint status;
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
  if (status == 0)
  {
    GLchar infoLog[512];
    glGetShaderInfoLog(shaderHandle, 512, nullptr, infoLog);
    cout << infoLog << endl;
    return 1;
  }

  return 0;
}

GLint PipelineProgram::GetUniformVariableHandle(const char * name)
{
  GLint handle = glGetUniformLocation(programHandle, name);

  if (handle == -1)
    cout << "Uniform variable not found: \'" << name << '\'' << "." << endl;

  return handle;
}

// Set the uniform variable of type "int". 
void PipelineProgram::SetUniformVariablei(const char * name, int value)
{
  glUniform1i(GetUniformVariableHandle(name), value);
}

// Set the uniform variable of type "float". 
void PipelineProgram::SetUniformVariablef(const char * name, float value)
{
  glUniform1f(GetUniformVariableHandle(name), value);
}

// Set the uniform variable of type "4x4 matrix of floats" (column-major).
void PipelineProgram::SetUniformVariableMatrix4fv(const char * name, GLboolean transpose, float value[16])
{
  glUniformMatrix4fv(GetUniformVariableHandle(name), 1, transpose, value);
}

