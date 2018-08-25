#version 430 core

in vec3 gNormal;
in vec4 gColor;
in float p;

out vec4 fragColor;

void main(){

    fragColor = gColor;
}

