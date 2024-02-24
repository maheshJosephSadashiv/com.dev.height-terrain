/*
  CSCI 420 Computer Graphics, Computer Science, USC
  Assignment 1: Height Fields with Shaders.
  C/C++ starter code
  Student name: Mahesh Joseph Sadashiv
  Student username: sadashiv
*/

#include "openGLHeader.h"
#include "glutHeader.h"
#include "openGLMatrix.h"
#include "imageIO.h"
#include "pipelineProgram.h"
#include "vbo.h"
#include "vao.h"
#include <fstream>

#include <iostream>
#include <cstring>
#include <iomanip>
#include <sstream>

#if defined(WIN32) || defined(_WIN32)
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#if defined(WIN32) || defined(_WIN32)
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper";
#endif

using namespace std;

int mousePos[2]; // x,y screen coordinates of the current mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

typedef enum { MODE_1, MODE_2, MODE_3, MODE_4} MODE_STATE;
MODE_STATE modeState = MODE_1;
// Transformations of the terrain.
float terrainRotate[3] = { 0.0f, 0.0f, 0.0f }; 
// terrainRotate[0] gives the rotation around x-axis (in degrees)
// terrainRotate[1] gives the rotation around y-axis (in degrees)
// terrainRotate[2] gives the rotation around z-axis (in degrees)
float terrainTranslate[3] = { 0.0f, 0.0f, 0.0f };
float terrainScale[3] = { 1.0f, 1.0f, 1.0f };

// Width and height of the OpenGL window, in pixels.
int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 Homework 1";

// Stores the image loaded from disk.
ImageIO * heightmapImage;

// Number of vertices in the single triangle (starter code).
int numVertices;
// Number of lines.
int numLines;
// Number of triangles verticies
int numTriangleVertices;
//width and height of the image
int width, height;

// CSCI 420 helper classes.
OpenGLMatrix matrix;
PipelineProgram * pipelineProgram = nullptr;
//VBO for points
VBO * vboVertices = nullptr;
VBO * vboColors = nullptr;
VAO * vao = nullptr;
// VBO for lines
VBO * vboLines = nullptr;
VBO * vboColorsLines = nullptr;
VAO * vaoLines = nullptr;
// VBO for triangles
VBO * vboTriangles = nullptr;
VBO * vboColorsTriangles = nullptr;
VAO * vaoTriangles = nullptr;
// VBO for SmoothTriangles model 4
VBO * vboSmoothTrianglesC = nullptr;
VBO * vboSmoothTrianglesU = nullptr;
VBO * vboSmoothTrianglesD = nullptr;
VBO * vboSmoothTrianglesL = nullptr;
VBO * vboSmoothTrianglesR = nullptr;
VBO * vboColorsSmoothTriangles = nullptr;
VAO * vaoSmoothTriangles = nullptr;
// positions for the neighbours 
float * _up = nullptr;
float * _down = nullptr;
float * _left = nullptr;
float * _right = nullptr;

//camera init
float eye[3] = {0.0f, 1.0f, 1.0f};
//scale and exponent for mode 4
float scale = 1;
float exponent = 1;
int numImages = 0;

void setNeighbours(int k, int i, int j){
  _up[k] = i == 0 ? (1.0 * (i - width/2))/(width-1) : (1.0 * (i - 1 - width/2))/(width-1);
  _up[k+1] = i == 0 ? heightmapImage->getPixel(i, j, 0)/255.0f : heightmapImage->getPixel(i - 1, j, 0)/255.0f;
  _up[k+2] = (-1.0 * (height/2 - j))/(height-1);

  _down[k] = i == width - 1 ? (1.0 * (i - width/2))/(width-1) : (1.0 * (i + 1 - width/2))/(width-1);
  _down[k+1] = i == width - 1 ? heightmapImage->getPixel(i, j, 0)/255.0f : heightmapImage->getPixel(i + 1, j, 0)/255.0f;
  _down[k+2] = (-1.0 * (height/2 - j))/(height-1);

  _left[k] = (1.0 * (i - width/2))/(width-1);
  _left[k+1] = j == 0 ? heightmapImage->getPixel(i, j, 0)/255.0f : heightmapImage->getPixel(i, j - 1, 0)/255.0f;
  _left[k+2] = j == 0 ? (-1.0 * (height/2 - j))/(height-1) : (-1.0 * (height/2 - j + 1))/(height-1);

  _right[k] = (1.0 * (i - width/2))/(width-1);
  _right[k+1] = j == height - 1 ? heightmapImage->getPixel(i, j, 0)/255.0f : heightmapImage->getPixel(i, j + 1, 0)/255.0f;
  _right[k+2] = j == height - 1 ? (-1.0 * (height/2 - j))/(height-1) : (-1.0 * (height/2 - j - 1))/(height-1);
}

//create the height map
float * createHeightMap(MODE_STATE mode){
  width = heightmapImage->getWidth();
  height = heightmapImage->getHeight();
  float * positions = nullptr;
  int k =0;
  switch (mode)
  {
  case MODE_1:
      numVertices = width * height;
      positions = (float*) malloc (numVertices * 3 * sizeof(float));
      for( int i = 0; i<width;i++){
        for( int j = 0; j<height;j++){
          positions[k++] = (1.0 * (i - width/2))/(width-1);
          positions[k++] = 0.30 * heightmapImage->getPixel(i, j, 0)/255.0f;
          positions[k++] = (-1.0 * (height/2 - j))/(height-1);
        }
      }
      return positions;
    break;
  case MODE_2:
      numLines = (width - 1) * height + (height - 1) * width;
      positions = (float*) malloc ((2 * numLines * 3) * sizeof(float));
      for( int i = 0; i<width;i++){
        for( int j = 0; j<height;j++){
          if(i < width - 1){
            positions[k++] = (1.0 * (i - width/2))/(width-1);
            positions[k++] = 0.30 * heightmapImage->getPixel(i, j, 0)/255.0f;
            positions[k++] = (-1.0 * (height/2 - j))/(height-1);

            positions[k++] = (1.0 * (i - width/2 + 1))/(width-1);
            positions[k++] = 0.30 * heightmapImage->getPixel(i + 1, j, 0)/255.0f;
            positions[k++] = (-1.0 * (height/2 - j))/(height-1);
          }
          if(j < height - 1){
            positions[k++] = (1.0 * (i - width/2))/(width-1);
            positions[k++] = 0.30 * heightmapImage->getPixel(i, j, 0)/255.0f;
            positions[k++] = (-1.0 * (height/2 - j))/(height-1);
            
            positions[k++] = (1.0 * (i - width/2))/(width-1);
            positions[k++] = 0.30 * heightmapImage->getPixel(i, j  + 1, 0)/255.0f;
            positions[k++] = (-1.0 * (height/2 - (j  + 1)))/(height-1);
          }
        }
      }
      return positions;
    break;
   case MODE_3:
      numTriangleVertices = height * width  + (height - 2) * (width + 2);
      positions = (float *) malloc (numTriangleVertices * 3 * sizeof(float));
      for( int j = 0; j<height-1;j++){
        for( int i = 0; i<width;i++){
          positions[k++] = (1.0 * (i - width/2))/(width-1);
          positions[k++] = 0.30 * heightmapImage->getPixel(i, j, 0)/255.0f;
          positions[k++] = (-1.0 * (height/2 - j))/(height-1);

          positions[k++] = (1.0 * (i - width/2))/(width-1);
          positions[k++] = 0.30 * heightmapImage->getPixel(i, (j + 1), 0)/255.0f;
          positions[k++] = (-1.0 * (height/2 - (j + 1)))/(height-1);
        }
          //degenerate triangles
          positions[k++] = (1.0 * (width - 1 - width/2))/(width-1);
          positions[k++] = 0.30 * heightmapImage->getPixel(width - 1, (j+1), 0)/255.0f;
          positions[k++] = (-1.0 * (height/2 - (j + 1)))/(height-1);

          positions[k++] = (1.0 * (0 - width/2))/(width-1);
          positions[k++] = 0.30 * heightmapImage->getPixel(0, j + 1, 0)/255.0f;
          positions[k++] = (-1.0 * (height/2 - (j + 1)))/(height-1);
      }
      return positions;
   break;
   case MODE_4:
      //numTriangleVertices = height * width  + (height - 2) * (width + 2);
      positions = (float *) malloc (numTriangleVertices * 3 * sizeof(float));
      _up = (float *) malloc (numTriangleVertices * 3 * sizeof(float));
      _down = (float *) malloc (numTriangleVertices * 3 * sizeof(float));
      _left = (float *) malloc (numTriangleVertices * 3 * sizeof(float));
      _right = (float *) malloc (numTriangleVertices * 3 * sizeof(float));
      for( int j = 0; j<height-1;j++){
        for( int i = 0; i<width;i++){
          positions[k] = (1.0 * (i - width/2))/(width-1);
          positions[k+1] = heightmapImage->getPixel(i, j, 0)/255.0f;
          positions[k+2] = (-1.0 * (height/2 - j))/(height-1);
          setNeighbours(k, i, j);
          k += 3;
          positions[k] = (1.0 * (i - width/2))/(width-1);
          positions[k+1] =  heightmapImage->getPixel(i, (j + 1), 0)/255.0f;
          positions[k+2] = (-1.0 * (height/2 - (j + 1)))/(height-1);
          setNeighbours(k, i, j + 1);
          k += 3;
        }
          //degenerate triangles
          positions[k] = (1.0 * (width - 1 - width/2))/(width-1);
          positions[k+1] = heightmapImage->getPixel(width - 1, (j+1), 0)/255.0f;
          positions[k+2] = (-1.0 * (height/2 - (j + 1)))/(height-1);
          setNeighbours(k, width - 1, j + 1);
          k += 3;
          positions[k] = (1.0 * (0 - width/2))/(width-1);
          positions[k+1] = heightmapImage->getPixel(0, j + 1, 0)/255.0f;
          positions[k+2] = (-1.0 * (height/2 - (j + 1)))/(height-1);
          setNeighbours(k, 0, j + 1);
          k += 3;
      }
      return positions;
   break;
  default:
    break;
  }
  return positions;
}

//create the color for the terrain
float * createColorMapMode(MODE_STATE mode){
    float * colors = nullptr;
    switch (mode)
    {
    case MODE_1:
      colors = (float*) malloc (numVertices * 4 * sizeof(float)); // 4 floats per vertex, i.e., r,g,b,a
        for(int i = 0; i < numVertices*4;i+=4){
            colors[i] = 1;
            colors[i + 1] = 1;
            colors[i + 2] = 1;
            colors[i + 3] = 1;
      }
      return colors;
      break;
    case MODE_2:
        colors = (float*) malloc (2 * numLines * 4 * sizeof(float)); // 4 floats per line, i.e., r,g,b,a
        for(int i = 0; i < numLines*8; i += 8){
            colors[i] = 1;
            colors[i + 1] = 1;
            colors[i + 2] = 1;
            colors[i + 3] = 1;

            colors[i + 4] = 1;
            colors[i + 5] = 1;
            colors[i + 6] = 1;
            colors[i + 7] = 1;
      }
      return colors;
    break;
    case MODE_3:
        colors = (float*) malloc (numTriangleVertices * 4 * sizeof(float)); // 4 floats per vertex, i.e., r,g,b,a
        for(int i = 0; i < numTriangleVertices*4;i+=4){
            colors[i] = 1;
            colors[i + 1] = 1;
            colors[i + 2] = 1;
            colors[i + 3] = 1;
      }
      return colors;
    break;
    default:
      break;
    }
  return colors;
}

// Write a screenshot to the specified filename.
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}

void idleFunc()
{
  // Do some stuff... 
  // For example, here, you can save the screenshots to disk (to make the animation).

  // Notify GLUT that it should call displayFunc.
  glutPostRedisplay();
}

void reshapeFunc(int w, int h)
{
  glViewport(0, 0, w, h);

  // When the window has been resized, we need to re-set our projection matrix.
  matrix.SetMatrixMode(OpenGLMatrix::Projection);
  matrix.LoadIdentity();
  // You need to be careful about setting the zNear and zFar. 
  // Anything closer than zNear, or further than zFar, will be culled.
  const float zNear = 0.1f;
  const float zFar = 10000.0f;
  const float humanFieldOfView = 60.0f;
  matrix.Perspective(humanFieldOfView, 1.0f * w / h, zNear, zFar);
}

void mouseMotionDragFunc(int x, int y)
{
  // Mouse has moved, and one of the mouse buttons is pressed (dragging).

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the terrain
    case TRANSLATE:
      if (rightMouseButton)
      {
        // control x,y translation via the left mouse button
        terrainTranslate[0] += mousePosDelta[0] * 0.01f;
        terrainTranslate[1] -= mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        terrainTranslate[2] += mousePosDelta[1] * 0.01f;
      }
      break;

    // rotate the terrain
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        terrainRotate[0] += mousePosDelta[1];
        terrainRotate[1] += mousePosDelta[0];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        terrainRotate[2] += mousePosDelta[1];
      }
      break;

    // scale the terrain
    case SCALE:
      if (leftMouseButton)
      {
        // control x,y scaling via the left mouse button
        terrainScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
        terrainScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z scaling via the middle mouse button
        terrainScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // Mouse has moved.
  // Store the new mouse position.
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // A mouse button has has been pressed or depressed.

  // Keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables.
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // Keep track of whether CTRL and SHIFT keys are pressed.
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = SCALE;
    break;

    // If CTRL and SHIFT are not pressed, we are in rotate mode.
    default:
      controlState = ROTATE;
    break;
  }

  // Store the new mouse position.
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{ 
  ostringstream oss;
  string with3digits;
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
      cout << "You pressed the spacebar." << endl;
    break;

    case 'x':
      // Take a screenshot.
      oss << std::setw(3) << std::setfill('0') << numImages;
      with3digits = "./outputImages/"+oss.str()+".jpg";
      cout << with3digits << endl;
      numImages++;
      saveScreenshot(with3digits.c_str());
      break;
    case '1':
      modeState = MODE_1;
      pipelineProgram->SetUniformVariablef("mode", 1);
      break;
    case '2':
      modeState = MODE_2;
      pipelineProgram->SetUniformVariablef("mode", 2);
      break;
    case '3':
      modeState = MODE_3;
      pipelineProgram->SetUniformVariablef("mode", 3);
      break;
    case '4':
      modeState = MODE_4;
      pipelineProgram->SetUniformVariablef("scale", scale);
      pipelineProgram->SetUniformVariablef("exponent", exponent);
      pipelineProgram->SetUniformVariablef("mode", 4);
      break;
    case '=':
      if(modeState == MODE_4)
      {
        scale *= 2;
        pipelineProgram->SetUniformVariablef("scale", scale);
        break;
      }
    case '-':
      if (modeState == MODE_4)
      {
        scale /= 2;
        pipelineProgram->SetUniformVariablef("scale", scale);
        break;
      }
    case '9':
      if (modeState == MODE_4)
      {
        exponent *= 2;
        pipelineProgram->SetUniformVariablef("exponent", exponent);
        break;
      }
    case '0':
      if (modeState == MODE_4)
      {
        exponent /= 2;
        pipelineProgram->SetUniformVariablef("exponent", exponent);
        break;
      }
  }
}

void displayModeFunc(){
  switch (modeState)
  {
  case MODE_1:
    pipelineProgram->SetUniformVariablei("currentVAO", 0);
    vao->Bind();
    glDrawArrays(GL_POINTS, 0, numVertices);
    break;
  case MODE_2:
    pipelineProgram->SetUniformVariablei("currentVAO", 0);
    vaoLines->Bind();
    glDrawArrays(GL_LINES, 0, numLines * 2);
    break;
  case MODE_3:
    pipelineProgram->SetUniformVariablei("currentVAO", 0);
    vaoTriangles->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numTriangleVertices);
    vaoLines -> Bind();
    pipelineProgram->SetUniformVariablei("currentVAO", 1);
    glDrawArrays(GL_LINES, 0, numLines * 2);
    break;
  case MODE_4:
    pipelineProgram->SetUniformVariablei("currentVAO", 0);
    vaoSmoothTriangles->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numTriangleVertices);
    break;
  default:
    break;
  }
}

void displayFunc()
{
  // This function performs the actual rendering.

  // First, clear the screen.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up the camera position, focus point, and the up vector.
  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.LoadIdentity();
  matrix.LookAt(eye[0], eye[1], eye[2],
                0, 0, 0,
                0, 1.0, 0);

  matrix.Rotate(terrainRotate[0], 1, 0, 0);
  matrix.Rotate(terrainRotate[1], 0, 1, 0);
  matrix.Rotate(terrainRotate[2], 0, 0, 1);
  matrix.Translate(terrainTranslate[0], terrainTranslate[1], terrainTranslate[2]);
  matrix.Scale(terrainScale[0], terrainScale[1], terrainScale[2]);

  

  // In here, you can do additional modeling on the object, such as performing translations, rotations and scales.
  // ...

  // Read the current modelview and projection matrices from our helper class.
  // The matrices are only read here; nothing is actually communicated to OpenGL yet.
  float modelViewMatrix[16];
  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.GetMatrix(modelViewMatrix);

  float projectionMatrix[16];
  matrix.SetMatrixMode(OpenGLMatrix::Projection);
  matrix.GetMatrix(projectionMatrix);

  // Upload the modelview and projection matrices to the GPU. Note that these are "uniform" variables.
  // Important: these matrices must be uploaded to *all* pipeline programs used.
  // In hw1, there is only one pipeline program, but in hw2 there will be several of them.
  // In such a case, you must separately upload to *each* pipeline program.
  // Important: do not make a typo in the variable name below; otherwise, the program will malfunction.
  pipelineProgram->SetUniformVariableMatrix4fv("modelViewMatrix", GL_FALSE, modelViewMatrix);
  pipelineProgram->SetUniformVariableMatrix4fv("projectionMatrix", GL_FALSE, projectionMatrix);

  // Execute the rendering.
  // Bind the VAO that we want to render. Remember, one object = one VAO. 
  displayModeFunc();

  // Swap the double-buffers.
  glutSwapBuffers();
}

void initScene(int argc, char *argv[])
{
  // Load the image from a jpeg disk file into main memory.
  heightmapImage = new ImageIO();
  if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  {
    cout << "Error reading image " << argv[1] << "." << endl;
    exit(EXIT_FAILURE);
  }

  // Set the background color.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black color.

  // Enable z-buffering (i.e., hidden surface removal using the z-buffer algorithm).
  glEnable(GL_DEPTH_TEST);
  // Enable polygon offset
  glEnable(GL_POLYGON_OFFSET_FILL);
  // Set the scale factor and units for the offset
  glPolygonOffset(1.0f, 1.0f);
  // Enable blending
  glEnable(GL_BLEND);
  // Set the blend function
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Create a pipeline program. This operation must be performed BEFORE we initialize any VAOs.
  // A pipeline program contains our shaders. Different pipeline programs may contain different shaders.
  // In this homework, we only have one set of shaders, and therefore, there is only one pipeline program.
  // In hw2, we will need to shade different objects with different shaders, and therefore, we will have
  // several pipeline programs (e.g., one for the rails, one for the ground/sky, etc.).
  pipelineProgram = new PipelineProgram(); // Load and set up the pipeline program, including its shaders.
  // Load and set up the pipeline program, including its shaders.
  if (pipelineProgram->BuildShadersFromFiles(shaderBasePath, "vertexShader.glsl", "fragmentShader.glsl") != 0)
  {
    cout << "Failed to build the pipeline program." << endl;
    throw 1;
  } 
  cout << "Successfully built the pipeline program." << endl;
  // Bind the pipeline program that we just created. 
  // The purpose of binding a pipeline program is to activate the shaders that it contains, i.e.,
  // any object rendered from that point on, will use those shaders.
  // When the application starts, no pipeline program is bound, which means that rendering is not set up.
  // So, at some point (such as below), we need to bind a pipeline program.
  // From that point on, exactly one pipeline program is bound at any moment of time.
  pipelineProgram->Bind();


// create the vbo and vao for points
  float * positions = createHeightMap(MODE_1);
  float * colors = createColorMapMode(MODE_1);
  vboVertices = new VBO(numVertices, 3, positions, GL_STATIC_DRAW); // 3 values per position
  vboColors = new VBO(numVertices, 4, colors, GL_STATIC_DRAW); // 4 values per color
  vao = new VAO();
  vao->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboVertices, "position");
  vao->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboColors, "color");
  free(positions);
  free(colors);
// create the vbo and vao for lines
  positions = createHeightMap(MODE_2);
  colors = createColorMapMode(MODE_2);
  vboLines = new VBO(numLines * 2, 3, positions, GL_STATIC_DRAW);
  vboColorsLines = new VBO(numLines * 2, 4, colors, GL_STATIC_DRAW); 
  vaoLines = new VAO();
  vaoLines->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboLines, "position");
  vaoLines->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboColorsLines, "color");
  free(positions);
  free(colors);
  //Trianlges
  positions = createHeightMap(MODE_3);
  colors = createColorMapMode(MODE_3);
  vboTriangles = new VBO(numTriangleVertices, 3, positions, GL_STATIC_DRAW);
  vboColorsTriangles = new VBO(numTriangleVertices, 4, colors, GL_STATIC_DRAW); 
  vaoTriangles = new VAO();
  vaoTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboTriangles, "position");
  vaoTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboColorsTriangles, "color");
  free(positions);
  free(colors);
  //Triangles with smoothing
  positions = createHeightMap(MODE_4);
  colors = createColorMapMode(MODE_3);
  vboSmoothTrianglesC = new VBO(numTriangleVertices, 3, positions, GL_STATIC_DRAW);
  vboSmoothTrianglesU = new VBO(numTriangleVertices, 3, _up, GL_STATIC_DRAW);
  vboSmoothTrianglesD = new VBO(numTriangleVertices, 3, _down, GL_STATIC_DRAW);
  vboSmoothTrianglesL = new VBO(numTriangleVertices, 3, _left, GL_STATIC_DRAW);
  vboSmoothTrianglesR = new VBO(numTriangleVertices, 3, _right, GL_STATIC_DRAW);
  vboColorsSmoothTriangles = new VBO(numTriangleVertices, 4, colors, GL_STATIC_DRAW); 
  vaoSmoothTriangles = new VAO();
  vaoSmoothTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboSmoothTrianglesC, "position");
  vaoSmoothTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboSmoothTrianglesU, "upPosition");
  vaoSmoothTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboSmoothTrianglesD, "downPosition");
  vaoSmoothTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboSmoothTrianglesL, "leftPosition");
  vaoSmoothTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboSmoothTrianglesR, "rightPosition");
  vaoSmoothTriangles->ConnectPipelineProgramAndVBOAndShaderVariable(pipelineProgram, vboColorsSmoothTriangles, "color");
  free(positions);
  free(colors);
  free(_up);
  free(_down);
  free(_left);
  free(_right);

  // Check for any OpenGL errors.
  std::cout << "GL error status is: " << glGetError() << std::endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cout << "The arguments are incorrect." << endl;
    cout << "usage: ./hw1 <heightmap file>" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  #ifdef __APPLE__
    // This is needed on recent Mac OS X versions to correctly display the window.
    glutReshapeWindow(windowWidth - 1, windowHeight - 1);
  #endif

  // Tells GLUT to use a particular display function to redraw.
  glutDisplayFunc(displayFunc);
  // Perform animation inside idleFunc.
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);

  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif

  // Perform the initialization.
  initScene(argc, argv);

  // Sink forever into the GLUT loop.
  glutMainLoop();
}