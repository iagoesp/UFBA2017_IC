#version 330 core

in vec2 position; // receberá a posição x e y da malha. contudo, a variável position.y vai se tornar a coordenada z e a coordenada y ficará em função da função seno*cos.

//uniform mat4 MVP;
uniform float amp;

out float y;
out vec3 vPos;

void main(){
  //float pi = 3.14;
  y = cos((position.x)) * sin((position.y));
  y = y*amp;
  //y=0.0;
//  gl_Position = MVP * vec4(position.x, y, position.y, 1.0);
  vPos = (position.x, y, position.y);
}
