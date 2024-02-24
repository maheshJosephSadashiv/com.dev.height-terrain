#version 150

in vec4 col;
in vec3 pos;
out vec4 c;
uniform float mode;
uniform float scale;
uniform int currentVAO;

void main()
{
  // compute the final pixel color
  if (currentVAO == 1){
    //Extra credit applying the mesh over the triangle mode
    c = vec4(0.5f, 0.5f, 0.5f, 0.2f);
  } 
  else {
    //Extra credit creating a terrain effect by coloring high altitudes with white, middle ranged altitudes with green and lower altitudes with brown or blue 
    float height;
    if (mode == 4){
    height = pos[1]/scale;
    }else{
      height = pos[1]/0.3;
    }
    if (height < 0.3) {
      c = vec4(0.8*pos[1]/0.3, 0.5*pos[1]/0.3, 0.1, 1.0);
    } else if (0.3 <= height && height < 0.75) {
      c = vec4(0.0, 0.8*pos[1]/0.3, 0.0, 1.0);
    } else {
      c = vec4(1.0*pos[1]/0.3, 1.0*pos[1]/0.3, 1.0*pos[1]/0.3, 1.0);
    }
  }
}