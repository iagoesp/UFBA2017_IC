#version 330 core

in vec2 position;

uniform mat4 M;
uniform mat4 V;
uniform mat4 MVP;
uniform float wave;
uniform float freq;
uniform float amp;

out float y;

void main(){
  //float pi = 3.14;
	y = sin((amp*position.x+wave)) * cos((amp*position.y+wave));
  y = y*freq;
  gl_Position = MVP * vec4(position.x, y, position.y, 1.0);

}
