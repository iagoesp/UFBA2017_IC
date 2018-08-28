#version 430 core

in vec2 position;

uniform float amp;

out float h;
out vec3 vNormal;
out vec4 vColor;

out vec3 vPosition;

void main(){
    h 			= amp * cos((position.x)) * sin((position.y))+1;
    vNormal.x	= -sin(position.x)*sin(position.y);
    vNormal.z	= cos(position.x)*cos(position.y);
    vNormal.y   = h;

    vColor.r 	= h / 3.0;
    vColor.g 	= 0.0;
    vColor.b 	= 1.0 - h / 4.0;
    vColor.a 	= 1.0f;

    vPosition 	= vec3(position.x, h, position.y);
}
