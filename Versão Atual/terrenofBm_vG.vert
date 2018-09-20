#version 430 core

in vec2 position;

uniform float amp;
uniform mat4 MVP;

out vec3 vNormal;
out vec4 vColor;

void main(){
    float h 			= 0;//amp * sin((position.x)) * cos((position.y))+1;
    vNormal.x	= -sin(position.x)*sin(position.y);
    vNormal.z	= cos(position.x)*cos(position.y);
    vNormal.y   = h;

    vColor.r 	= h / 4.0;
    vColor.g 	= 0.0;
    vColor.b 	= 1.0 - h / 4.0;
    vColor.a 	= 1.0f;

    gl_Position = MVP * vec4(vec3(position.x, h, position.y),1.0);

}
