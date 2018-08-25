#version 430 core

layout(triangles, equal_spacing, cw) in;

//uniform vec4 planes[6];

uniform mat4 P;
uniform mat4 V;
uniform mat4 MVP;

//in vec3 tcPosition[];
in gl_PerVertex {
    vec3 gl_Position;
} gl_in[];
in vec3 tcNormal[];
in vec4 tcColor[];

out float p;
out vec3 vcNormal;
out vec4 vcColor;

out vec3 tePosition;

void main(){

    vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position;
    vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position;
    vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position;
    tePosition = (p0 + p1 + p2);

    vec3 n0 = gl_TessCoord.x * tcNormal[0];
    vec3 n1 = gl_TessCoord.y * tcNormal[1];
    vec3 n2 = gl_TessCoord.z * tcNormal[2];
    vcNormal = (n0 + n1 + n2);

    vec4 c0 = gl_TessCoord.x * tcColor[0];
    vec4 c1 = gl_TessCoord.y * tcColor[1];
    vec4 c2 = gl_TessCoord.z * tcColor[2];
    vcColor = (c0 + c1 + c2);

    p = tePosition.y;
    gl_Position = MVP * vec4(tePosition, 1.0);
}

