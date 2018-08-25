#version 430 core

layout(vertices = 3) out;

//in vec3 vPosition[];
in gl_PerVertex {
    vec3 gl_Position;
} gl_in[];

in vec3 vNormal[];
in vec4 vColor[];

//out vec3 tcPosition[];
out gl_PerVertex {
    vec3 gl_Position;
} gl_out[];
out vec3 tcNormal[];
out vec4 tcColor[];

uniform float px;
uniform float py;
uniform float pz;

float TessLevelnner;
float TessLeveluter;

#define ID gl_InvocationID

float LOD(vec3 posV, float posCX, float posCY, float posCZ){
  float dist = sqrt((posCX - posV.x)*(posCX - posV.x)
                  + (posCY - posV.y)*(posCY - posV.y)
                  + (posCZ - posV.z)*(posCZ - posV.z));
  if(dist<=4) return 32.0;
  else if(dist>4 && dist<=8) return 16.0;
  else if(dist>8 && dist<=16) return 8.0;
  else if(dist>16 && dist<=32) return 4.0;
  else if(dist>32 && dist<=64) return 2.0;
  else if(dist>64) return 1.0;

}

void main(){

    gl_out[ID].gl_Position = gl_in[ID].gl_Position;
    //tcPosition[ID]  = vPosition[ID];
    tcNormal[ID]    = vNormal[ID];
    tcColor[ID]     = vColor[ID];

    if (ID == 0) {
        vec3 vPos = gl_in[0].gl_Position;
        TessLevelnner = LOD(vPos, px, py, pz);
        TessLeveluter = LOD(vPos, px, py, pz);
        gl_TessLevelInner[0] = TessLevelnner;
        gl_TessLevelOuter[0] = TessLeveluter;
        gl_TessLevelOuter[1] = TessLeveluter;
        gl_TessLevelOuter[2] = TessLeveluter;
    }
}


