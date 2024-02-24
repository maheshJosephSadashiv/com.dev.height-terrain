#version 150

in vec3 position;
in vec3 upPosition;
in vec3 downPosition;
in vec3 leftPosition;
in vec3 rightPosition;
in vec4 color;
out vec4 col;
out vec3 pos;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform float scale;
uniform float exponent;
uniform float mode;

void main()
{
  // compute the transformed and projected vertex position (into gl_Position) 
  // compute the vertex color (into col)
  if(mode == 4){
    float x = 1.0f * (upPosition[0] + downPosition[0] + leftPosition[0] + rightPosition[0] + position[0])/ 5;
    float y = 1.0f * (upPosition[1] + downPosition[1] + leftPosition[1] + rightPosition[1] + position[1])/ 5;
    y = scale * pow(y, exponent);
    float z = 1.0f * (upPosition[2] + downPosition[2] + leftPosition[2] + rightPosition[2] + position[2])/ 5;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(x, y, z, 1.0f);
    pos = vec3(x, y, z);
  }else{
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0f);
    //passing the coordinates to the fragment shader to calculate the the color
    pos = position;
  }
  col = color;  
}