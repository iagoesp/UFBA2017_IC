#version 430 core

in vec2 position;

uniform float amp;

out float h;
out vec3 vNormal;
out vec4 vColor;

out vec3 vPosition;

out vertexData {
    //vec2 texCoord;
    vec3 vNg;
    vec4 vCg;
} vIn;

void main(){
    h 			= amp * cos((position.x)) * sin((position.y));
    vNormal.x	= -sin(position.x)*sin(position.y);
    vNormal.z	= cos(position.x)*cos(position.y);
    vNormal.y   = h;
    vIn.vNg = (vNormal);

    vColor.r 	= h / 4.0;
    vColor.g 	= 0.0;
    vColor.b 	= 1.0 - h / 4.0;
    vColor.a 	= 1.0f;
    vIn.vCg = (vColor);

    vPosition 	= vec3(position.x, h, position.y);
}
