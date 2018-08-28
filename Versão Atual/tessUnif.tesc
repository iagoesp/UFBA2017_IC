#version 430 core

layout(vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec4 vColor[];

out vec3 tcPosition[];
out vec3 tcNormal[];
out vec4 tcColor[];

uniform float TessLevelInner;
uniform float TessLevelOuter;
uniform float px;
uniform float py;
uniform float pz;

float TessLevelnner;
float TessLeveluter;

#define ID gl_InvocationID

void main(){

    tcPosition[ID]  = vPosition[ID];
    tcNormal[ID]    = vNormal[ID];
    tcColor[ID]     = vColor[ID];

    if (ID == 0) {
        vec3 vPos = vPosition[0];
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }
}

