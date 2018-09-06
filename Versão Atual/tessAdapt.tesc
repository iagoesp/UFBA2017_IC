#version 430 core

layout(vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec4 vColor[];

out vec3 tcPosition[];
out vec3 tcNormal[];
out vec4 tcColor[];

uniform float px;
uniform float py;
uniform float pz;

float TessLevelInner;
float TessLevelOuter;

#define ID gl_InvocationID

float LOD(vec3 posV, float posCX, float posCY, float posCZ){
  float dist = sqrt((posV.x - posCX)*(posV.x - posCX)
                  + (posV.y - posCY)*(posV.y - posCY)
                  + (posV.z - posCZ)*(posV.z - posCZ));
  if(dist<=5) return 64.0;
  else if(dist>5 && dist<=10) return 32.0;
  else if(dist>10 && dist<=15) return 16.0;
  else if(dist>15 && dist<=20) return 8.0;
  else if(dist>20 && dist<=30) return 4.0;
  else if(dist>30 && dist<=40) return 2.0;
  else if(dist>40) return 1.0;
}

void main(){

    tcPosition[ID]  = vPosition[ID];
    tcNormal[ID]    = vNormal[ID];
    tcColor[ID]     = vColor[ID];

    if (ID == 0) {
        vec3 vPos = vPosition[0];
        TessLevelInner = LOD(vPos, px, py, pz);
        TessLevelOuter = LOD(vPos, px, py, pz);
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }

    /*else if (ID == 10) {
        TessLevelnner=TessLevelInner+32.0;
        TessLeveluter=TessLevelnner;
        gl_TessLevelInner[0] = TessLevelnner;
        gl_TessLevelOuter[0] = TessLeveluter;
        gl_TessLevelOuter[1] = TessLeveluter;
        gl_TessLevelOuter[2] = TessLeveluter;
    }*/

}

