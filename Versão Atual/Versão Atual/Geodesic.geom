#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 100) out;

//in vec3 vPosition;

in vec3 vNormal[];
in vec4 vColor[];

//out vec4 gl_Position;
out vec3 gNormal;
out vec4 gColor;

uniform float px;
uniform float py;
uniform float pz;
uniform mat4 MVP;

struct Triangulo{
    vec4 v0;
    vec4 v1;
    vec4 v2;
    vec4 cA;
    vec4 cO;
    vec4 cH;
    vec3 normal;
    float distCA;
    float distCO;
    float distH;
};

float dist(vec4 x1, vec4 x2){
    return sqrt((x2.x - x1.x)*(x2.x - x1.x) +
                (x2.y - x1.y)*(x2.y - x1.y) +
                (x2.z - x1.z)*(x2.z - x1.z));
}

void main(){
    vec4 camV;
    camV.x = px;
    camV.y = py;
    camV.z = pz;
    camV.w = 1.0;

    Triangulo p0;
    p0.v0 = gl_in[0].gl_Position;
    p0.v1 = gl_in[1].gl_Position;
    p0.v2 = gl_in[2].gl_Position;

    p0.cH = (p0.v0 + p0.v1) / 2;
    p0.cO = (p0.v1 + p0.v2) / 2;
    p0.cA = (p0.v2 + p0.v0) / 2;
    p0.distH = dist(p0.cH, camV);
    p0.distCO = dist(p0.cO, camV);
    p0.distCA = dist(p0.cA, camV);

    if((p0.distCA < 45.f) ||
       (p0.distCO < 45.f) ||
       (p0.distH < 45.f)){

        if((p0.distCA < 45) && (p0.distCO < 45) && (p0.distH < 45)){ //7
            Triangulo p1;
//                gColor[0].rgba vec4(p0.v0.y / 4, 0.0f, 1.0f - p0.v0.y / 4, 1.0f);
                gl_Position = p0.v0;
                EmitVertex();

//                gColor[1].r = p0.cH.y / 4;
//                gColor[1].g = 0.0f;
//                gColor[1].b = 1.0f - p0.cH.y / 4;
//                gColor[1].a = 1.0f;
                gl_Position = p0.cH;
                EmitVertex();

//                gColor[2].r = p0.cA.y / 4;
//                gColor[2].g = 0.0f;
//                gColor[2].b = 1.0f - p0.cA.y / 4;
//                gColor[2].a = 1.0f;
                gl_Position = p0.cA;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.cA;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, camV);
                p1.distCO = dist(p1.cO, camV);
                p1.distCA = dist(p1.cA, camV);


            Triangulo p2;
                gl_Position = p0.cA;
                EmitVertex();

                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cA;
                p2.v1 = p0.cH;
                p2.v2 = p0.v2;

            Triangulo p3;
                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cH;
                p3.v1 = p0.cO;
                p3.v2 = p0.v2;

            Triangulo p4;
                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();
                EndPrimitive();

                p4.v0 = p0.cH;
                p4.v1 = p0.v1;
                p4.v2 = p0.cO;
        }
        else if((p0.distH < 45) && !(p0.distCA < 45) && (p0.distCO < 45)){ //6
            Triangulo p1;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.v2;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, camV);
                p1.distCO = dist(p1.cO, camV);
                p1.distCA = dist(p1.cA, camV);

            Triangulo p2;
                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cH;
                p2.v1 = p0.cO;
                p2.v2 = p0.v2;

            Triangulo p3;
                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cH;
                p3.v1 = p0.v1;
                p3.v2 = p0.cO;
        }
        else if((p0.distH < 45) && (p0.distCA < 45) && !(p0.distCO < 45)){ //5
            Triangulo p1;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.cA;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.cA;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, camV);
                p1.distCO = dist(p1.cO, camV);
                p1.distCA = dist(p1.cA, camV);

            Triangulo p2;
                gl_Position = p0.cA;
                EmitVertex();

                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cA;
                p2.v1 = p0.cH;
                p2.v2 = p0.v2;

            Triangulo p3;
                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cH;
                p3.v1 = p0.v1;
                p3.v2 = p0.v2;
        }
        else if(!(p0.distH < 45) && (p0.distCA < 45) && (p0.distCO < 45)){ //4
            Triangulo p1;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.v1;
                p1.v2 = p0.cO;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, camV);
                p1.distCO = dist(p1.cO, camV);
                p1.distCA = dist(p1.cA, camV);

            Triangulo p2;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();

                gl_Position = p0.cA;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.v0;
                p2.v1 = p0.cO;
                p2.v2 = p0.cA;

            Triangulo p3;
                gl_Position = p0.cA;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cA;
                p3.v1 = p0.cO;
                p3.v2 = p0.v2;
        }
        else if((p0.distH < 45) && !(p0.distCA < 45) && !(p0.distCO < 45)){ //3
            Triangulo p1;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.v2;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, camV);
                p1.distCO = dist(p1.cO, camV);
                p1.distCA = dist(p1.cA, camV);

            Triangulo p2;
                gl_Position = p0.cH;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cH;
                p2.v1 = p0.v1;
                p2.v2 = p0.v2;
        }
        else if(!(p0.distH < 45) && (p0.distCA < 45) && !(p0.distCO < 45)){//2
            Triangulo p1;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.cA;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.v1;
                p1.v2 = p0.cA;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, camV);
                p1.distCO = dist(p1.cO, camV);
                p1.distCA = dist(p1.cA, camV);

            Triangulo p2;
                gl_Position = p0.cA;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cA;
                p2.v1 = p0.v1;
                p2.v2 = p0.v2;
        }
        else if(!(p0.distH < 45) && !(p0.distCA < 45) && (p0.distCO < 45)){ //1
            Triangulo p1;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.v1;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.v1;
                p1.v2 = p0.cO;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, camV);
                p1.distCO = dist(p1.cO, camV);
                p1.distCA = dist(p1.cA, camV);

            Triangulo p2;
                gl_Position = p0.v0;
                EmitVertex();

                gl_Position = p0.cO;
                EmitVertex();

                gl_Position = p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.v0;
                p2.v1 = p0.cO;
                p2.v2 = p0.v2;
        }
    }
}
