#version 430 core

in vec3 vcNormal;
in vec4 vcColor;
in float p;

out vec4 fragColor;

void main(){
    fragColor = vcColor;
}
