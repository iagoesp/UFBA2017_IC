#version 420

layout(triangles) in;
layout (triangle_strip, max_vertices=4) out;

uniform mat4 N;
uniform mat4 MVP;
uniform float px;
uniform float py;
uniform float pz;

in vertexData {
    //vec2 texCoord;
    vec3 vNg;
    vec4 vCg;
} gs[];

out VertexData {
    vec2 texCoord;
    vec3 normal;
    vec4 color;
} vOut;

void returnGl_0(){
    gl_Position = projModelViewMatrix * (gl_in[0].gl_Position);
    vOut.normal = normalize(N * gs[0].vNg );
    //vOut.texCoord = VertexIn[0].texCoord;
    EmitVertex();
}

void returnGl_1(){
    gl_Position = projModelViewMatrix * (gl_in[1].gl_Position);
    vOut.normal = normalize(N * gs[1].vNg );
    //vOut.texCoord = VertexIn[0].texCoord;
    EmitVertex();
}

void returnGl_2(){
    gl_Position = projModelViewMatrix * (gl_in[2].gl_Position);
    vOut.normal = normalize(N * gs[2].vNg );
    //vOut.texCoord = VertexIn[0].texCoord;
    EmitVertex();
}

void returnGl_N01(){
    // this is the new vertex
    gl_Position = projModelViewMatrix * (gl_in[0].gl_Position + gl_in[1].gl_Position); //não teria que dividir por dois?
    vOut.normal = normalize(normalMatrix * (gs[0].vNg + gs[1].vNg));
    vOut.color  = (gs[0].vCg + gs[1].vCg) / 2;
    //vOut.texCoord = (VertexIn[0].texCoord +  VertexIn[1].texCoord) * 0.5;
    EmitVertex();
}

void returnGl_N12(){
    // this is the new vertex
    gl_Position = projModelViewMatrix * (gl_in[1].gl_Position + gl_in[2].gl_Position); //não teria que dividir por dois?
    vOut.normal = normalize(normalMatrix * (gs[1].vNg + gs[2].vNg));
    //vOut.texCoord = (VertexIn[1].texCoord +  VertexIn[2].texCoord) * 0.5;
    vOut.color  = (gs[1].vCg + gs[2].vCg) / 2;
    EmitVertex();
}

void returnGl_N20(){
    // this is the new vertex
    gl_Position = projModelViewMatrix * (gl_in[0].gl_Position + gl_in[2].gl_Position); //não teria que dividir por dois?
    vOut.normal = normalize(normalMatrix * (gs[0].vNg + gs[2].vNg));
    vOut.color  = (gs[0].vCg + gs[2].vCg) / 2;
    //vOut.texCoord = (VertexIn[0].texCoord +  VertexIn[2].texCoord) * 0.5;
    EmitVertex();
}

void pattern(int i){
    if(i==1){ //pattern **1**
        returnGl_0();       // copy attributes for the first vertex
        returnGl_N01();     // **this is the new vertex**
        returnGl_1();       // copy attributes for the second vertex
        returnGl_2();       // copy attributes for the last vertex
    }

    else if(i==2){ //pattern **2**
        returnGl_0();       // copy attributes for the first vertex
        returnGl_1();       // copy attributes for the second vertex
        returnGl_2();       // copy attributes for the last vertex
        returnGl_N20();     // **this is the new vertex**
    }

    else if(i==3){ //pattern **3**
        returnGl_0();       // copy attributes for the first vertex
        returnGl_1();       // copy attributes for the second vertex
        returnGl_N12();     // **this is the new vertex**
        returnGl_2();       // copy attributes for the last vertex
    }

    else if(){ //pattern **4**
        returnGl_0();       // copy attributes for the first vertex
        returnGl_N01();     // **this is the new vertex**
        returnGl_1();       // copy attributes for the second vertex
        returnGl_2();       // copy attributes for the last vertex
        returnGl_N20();     // **this is the new vertex**
    }

    else if(){ //pattern **5**
        returnGl_0();       // copy attributes for the first vertex
        returnGl_1();       // copy attributes for the second vertex
        returnGl_N12();     // **this is the new vertex**
        returnGl_2();       // copy attributes for the last vertex
        returnGl_N20();     // **this is the new vertex**
    }

    else if(){ //pattern **6**
        returnGl_0();       // copy attributes for the first vertex
        returnGl_N01();     // **this is the new vertex**
        returnGl_1();       // copy attributes for the second vertex
        returnGl_2();       // copy attributes for the last vertex
        returnGl_N20();     // **this is the new vertex**
    }

    else if(){ //pattern **7**
        returnGl_0();       // copy attributes for the first vertex
        returnGl_N01();     // **this is the new vertex**
        returnGl_1();       // copy attributes for the second vertex
        returnGl_N12();       // copy attributes for the second vertex
        returnGl_2();       // copy attributes for the last vertex
        returnGl_N20();     // **this is the new vertex**
    }
}

int getMaxOutput(int pattern){
    int n;
    if(pattern == 1 || pattern == 2 || pattern == 3)
        n = 2;
    if(pattern == 4 || pattern == 5 || pattern == 6)
        n = 3;
    if(pattern == 7)
        n = 4;
    return n;
}

float dist(vec4 cEdge){
    float dist = sqrt(pow(cEdge.x - px, 2)
                    + pow(cEdge.y - py, 2)
                    + pow(cEdge.z - pz, 2));
    return dist;
}

void tess(vec4 cEdge01, vec4 cEdge02, vec4 cEdge12);
    float dist01 = dist(cEdge01);
    float dist02 = dist(cEdge02);
    float dist12 = dist(cEdge12);
    if(dist01 <= 4 && dist02 <= 4 && dist12 <= 4 ){
        pattern(7);
    }

}

void main(){
    vec4 cEdge01 = (gl_in[0].gl_Position + gl_in[1].gl_Position) / 2;
    vec4 cEdge02 = (gl_in[0].gl_Position + gl_in[2].gl_Position) / 2;
    vec4 cEdge12 = (gl_in[1].gl_Position + gl_in[2].gl_Position) / 2;

    tess(cEdge01, cEdge02, cEdge12);
}
