#version 420 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 100) out;

//in vec3 vPosition;
in gl_PerVertex {
    vec3 gl_Position;
};
in vec3 vNormal;
in vec4 vColor;

uniform float px;
uniform float py;
uniform float pz;
uniform mat4 MVP;

out vec3 vNormal;
out vec4 vColor;

typedef struct{
    vec3 v0;
    vec3 v1;
    vec3 v2;
    vec3 cA;
    vec3 cO;
    vec3 cH;
    float distCA;
    float distCO;
    float distH;
} Triangulo;

float dist(vec3 x1, vec3 x2){
    return sqrt((x2.x - x1.x)*(x2.x - x1.x) +
                (x2.y - x1.y)*(x2.y - x1.y) +
                (x2.z - x1.z)*(x2.z - x1.z));
}

void main(){
    Triangulo p0;
    vec3 cam = (px, py, pz);

    if( dist(gl_in[0].gl_Position, gl_in[1].gl_Position) >= dist(gl_in[1].gl_Position, gl_in[2].gl_Position) &&
        dist(gl_in[0].gl_Position, gl_in[1].gl_Position) >= dist(gl_in[2].gl_Position, gl_in[0].gl_Position)){
        p0.v0 = gl_in[0].gl_Position;
        p0.v1 = gl_in[1].gl_Position;
        p0.v2 = gl_in[2].gl_Position;
    }
    else if( dist(gl_in[1].gl_Position, gl_in[2].gl_Position) >= dist(gl_in[2].gl_Position, gl_in[0].gl_Position) &&
             dist(gl_in[1].gl_Position, gl_in[2].gl_Position) >= dist(gl_in[0].gl_Position, gl_in[1].gl_Position)){
        p0.v0 = gl_in[1].gl_Position;
        p0.v1 = gl_in[2].gl_Position;
        p0.v2 = gl_in[0].gl_Position;
    }
    else if( dist(gl_in[2].gl_Position, gl_in[0].gl_Position) >= dist(gl_in[0].gl_Position, gl_in[1].gl_Position) &&
             dist(gl_in[2].gl_Position, gl_in[0].gl_Position) >= dist(gl_in[1].gl_Position, gl_in[2].gl_Position)){
        p0.v0 = gl_in[2].gl_Position;
        p0.v1 = gl_in[0].gl_Position;
        p0.v2 = gl_in[1].gl_Position;
    }

    p0.cH = (p0.v0 + p0.v1) / 2;
    p0.cO = (p0.v1 + p0.v2) / 2;
    p0.cA = (p0.v2 + p0.v0) / 2;
    p0.distH = dist(p0.cH, cam);
    p0.distCO = dist(p0.cO, cam);
    p0.distCA = dist(p0.cA, cam);

    if((p0.distCA < 15) ||
       (p0.distCO < 15) ||
       (p0.distH < 15)){

        if((p0.distCA < 15) && (p0.distCO < 15) && (p0.distH < 15)){ //7
            Triangulo p1;
                gl_Position = MVP * vec4(MVP * vec4(p0.v0, 1.0);//;
                EmitVertex();

                gl_Position = MVP * vec4(MVP * vec4(p0.cH, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(MVP * vec4(p0.cA, 1.0);
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.cA;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, cam);
                p1.distCO = dist(p1.cO, cam);
                p1.distCA = dist(p1.cA, cam);

            Triangulo p2;
                gl_Position = MVP * vec4(MVP * vec4(p0.cA, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(MVP * vec4(p0.cH, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(MVP * vec4(p0.v2, 1.0);
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cA;
                p2.v1 = p0.cH;
                p2.v2 = p0.v2;

            Triangulo p3;
                gl_Position = MVP * vec4(MVP * vec4(p0.cH, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(MVP * vec4(p0.cO, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(MVP * vec4(p0.v2, 1.0);
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cH;
                p3.v1 = p0.cO;
                p3.v2 = p0.v2;

            Triangulo p4;
                gl_Position = MVP * vec4(MVP * vec4(p0.cH;
                EmitVertex();

                gl_Position = MVP * vec4(MVP * vec4(p0.v1;
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO;
                EmitVertex();
                EndPrimitive();

                p4.v0 = p0.cH;
                p4.v1 = p0.v1;
                p4.v2 = p0.cO;
        }
         else if((distH < 15) && !(distC1 < 15) && (distC2 < 15)){ //6
            Triangulo p1;
                gl_Position = MVP * vec4(p0.v0;
                EmitVertex();

                gl_Position = MVP * vec4(p0.cH;
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.v2;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, cam);
                p1.distCO = dist(p1.cO, cam);
                p1.distCA = dist(p1.cA, cam);

            Triangulo p2;
                gl_Position = MVP * vec4(p0.cH;
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO;
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cH;
                p2.v1 = p0.cO;
                p2.v2 = p0.v2;

            Triangulo p3;
                gl_Position = MVP * vec4(p0.cH;
                EmitVertex();

                gl_Position = MVP * vec4(p0.v1;
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO;
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cH;
                p3.v1 = p0.v1;
                p3.v2 = p0.cO;
        }
        else if((distH < 15) && (distC1 < 15) && !(distC2 < 15)){ //5
            Triangulo p1;
                gl_Position = MVP * vec4(p0.v0;
                EmitVertex();

                gl_Position = MVP * vec4(p0.cH;
                EmitVertex();

                gl_Position = MVP * vec4(p0.cA;
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.cA;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, cam);
                p1.distCO = dist(p1.cO, cam);
                p1.distCA = dist(p1.cA, cam);

            Triangulo p2;
                gl_Position = MVP * vec4(p0.cA;
                EmitVertex();

                gl_Position = MVP * vec4(p0.cH;
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2;
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cA;
                p2.v1 = p0.cH;
                p2.v2 = p0.v2;

            Triangulo p3;
                gl_Position = MVP * vec4(p0.cH;
                EmitVertex();

                gl_Position = MVP * vec4(p0.v1;
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2;
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cH;
                p3.v1 = p0.v1;
                p3.v2 = p0.v2;
        }
        else if(!(distH < 15) && (distC1 < 15) && (distC2 < 15)){ //4
            Triangulo p1;
                gl_Position = MVP * vec4(p0.v0, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v1, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO, 1.0);
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.v1;
                p1.v2 = p0.cO;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, cam);
                p1.distCO = dist(p1.cO, cam);
                p1.distCA = dist(p1.cA, cam);

            Triangulo p2;
                gl_Position = MVP * vec4(p0.v0, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cA, 1.0);
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.v0;
                p2.v1 = p0.cO;
                p2.v2 = p0.cA;

            Triangulo p3;
                gl_Position = MVP * vec4(p0.cA, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2, 1.0);
                EmitVertex();
                EndPrimitive();

                p3.v0 = p0.cA;
                p3.v1 = p0.cO;
                p3.v2 = p0.v2;
        }
        else if((distH < 15) && !(distC1 < 15) && !(distC2 < 15)){ //3
            Triangulo p1;
                gl_Position = MVP * vec4(p0.v0, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cH, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2, 1.0);
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.cH;
                p1.v2 = p0.v2;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, cam);
                p1.distCO = dist(p1.cO, cam);
                p1.distCA = dist(p1.cA, cam);

            Triangulo p2;
                gl_Position = MVP * vec4(p0.cH, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v1, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2, 1.0);
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cH;
                p2.v1 = p0.v1;
                p2.v2 = p0.v2;
        }
        else if(!(distH < 15) && (distC1 < 15) && !(distC2 < 15)){//2
            Triangulo p1;
                gl_Position = MVP * vec4(p0.v0, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v1, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cA, 1.0);
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.v1;
                p1.v2 = p0.cA;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, cam);
                p1.distCO = dist(p1.cO, cam);
                p1.distCA = dist(p1.cA, cam);

            Triangulo p2;
                gl_Position = MVP * vec4(p0.cA, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v1, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2, 1.0);
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.cA;
                p2.v1 = p0.v1;
                p2.v2 = p0.v2;
        }
        else if(!(distH < 15) && !(distC1 < 15) && (distC2 < 15)){ //1
            Triangulo p1;
                gl_Position = MVP * vec4(p0.v0, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v1, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO, 1.0);
                EmitVertex();
                EndPrimitive();

                p1.v0 = p0.v0;
                p1.v1 = p0.v1;
                p1.v2 = p0.cO;

                p1.cH = (p1.v0 + p1.v1) / 2;
                p1.cO = (p1.v1 + p1.v2) / 2;
                p1.cA = (p1.v2 + p1.v0) / 2;
                p1.distH = dist(p1.cH, cam);
                p1.distCO = dist(p1.cO, cam);
                p1.distCA = dist(p1.cA, cam);

            Triangulo p2;
                gl_Position = MVP * vec4(p0.v0, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.cO, 1.0);
                EmitVertex();

                gl_Position = MVP * vec4(p0.v2, 1.0);
                EmitVertex();
                EndPrimitive();

                p2.v0 = p0.v0;
                p2.v1 = p0.cO;
                p2.v2 = p0.v2;
        }
    }
