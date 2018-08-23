void emitir(vec3 v0, vec3 v1, vec3 v2){
    c1 = (v2 + v0) / 2;
    cH = (v0 + v1) / 2;
    c2 = (v1 + v2) / 2;

    gl_Position = c1;
    EmitVertex();

    gl_Position = cH;
    EmitVertex();

    gl_Position = c2;
    EmitVertex();
    EndPrimitive();

    emitir(v0, cH, c1);
    emitir(cH, v1, c2);
    emitir(c1, c2, v2);
    emitir(c1, cH, c2);
}


float distancia(vec3 x1, vec3 x2){
    return sqrt((x2.x - x1.x)*(x2.x - x1.x) +
                (x2.y - x1.y)*(x2.y - x1.y) +
                (x2.z - x1.z)*(x2.z - x1.z));
}

float dist(vec3 x1, float px, float py, float pz){
    return sqrt((px - x1.x)*(px - x1.x) +
                (py - x1.y)*(py - x1.y) +
                (pz - x1.z)*(pz - x1.z));
}
void main(){
    vec3 v0, v1, v2, c1, c2, cH;

//    if(gl_in[1].gl_Position - gl_in[0].gl_Position >= gl_in[2].gl_Position - gl_in[1].gl_Position &&
//       gl_in[1].gl_Position - gl_in[0].gl_Position >= gl_in[0].gl_Position - gl_in[2].gl_Position){
    if( dist(gl_in[0].gl_Position, gl_in[1].gl_Position) >= dist(gl_in[1].gl_Position, gl_in[2].gl_Position) &&
        dist(gl_in[0].gl_Position, gl_in[1].gl_Position) >= dist(gl_in[2].gl_Position, gl_in[0].gl_Position)){
        v0 = gl_in[0].gl_Position;
        v1 = gl_in[1].gl_Position;
        v2 = gl_in[2].gl_Position;

        cH = (v0 + v1) / 2;
        c2 = (v1 + v2) / 2;
        c1 = (v2 + v0) / 2;
        int lod;
        float distH = dist(cH, px, py, pz);
        float distC1 = dist(c1, px, py, pz);
        float distC2 = dist(c2, px, py, pz);

        if((distH < 15) ||
           (distC1 < 15) ||
           (distC2 < 15)){

            if((distH < 15) && (distC1 < 15) && (distC2 < 15)){ //7
                    gl_Position = c1;
                    EmitVertex();

                    gl_Position = cH;
                    EmitVertex();

                    gl_Position = c2;
                    EmitVertex();
                    EndPrimitive();

                    emitir(v0, cH, c1); //ok
                    emitir(cH, v1, c2); //ok
                    emitir(c1, c2, v2); //ok
                    emitir(c2, c1, cH); //ok
            }
             else if((distH < 15) && !(distC1 < 15) && (distC2 < 15)){ //6
                gl_Position = cH;
                EmitVertex();

                gl_Position = c2;
                EmitVertex();
                EndPrimitive();

                emitir(v2, cH, v0);
                emitir(cH, v1, c2);
                emitir(v2, cH, v2);
            }
            else if((distH < 15) && (distC1 < 15) && !(distC2 < 15)){ //5
                gl_Position = c1;
                EmitVertex();

                gl_Position = cH;
                EmitVertex();
                EndPrimitive();

                emitir(v0, cH, c1);
                emitir(cH, v1, v2);
                emitir(c1, cH, c2);
            }
            else if(!(distH < 15) && (distC1 < 15) && (distC2 < 15)){ //4
                gl_Position = c1;
                EmitVertex();

                gl_Position = c2;
                EmitVertex();
                EndPrimitive();

                emitir(v0, v1, c2);
                emitir(v0, c2, c1);
                emitir(c1, c2, v2);
            }
            else if((distH < 15) && !(distC1 < 15) && !(distC2 < 15)){ //3
                gl_Position = cH;
                EmitVertex();
                EndPrimitive();

                emitir(v0, cH, v2);
                emitir(cH, v1, v2);
            }
            else if(!(distH < 15) && (distC1 < 15) && !(distC2 < 15)){//2
                gl_Position = c1;
                EmitVertex();

                emitir(v0, v1, c1);
                emitir(c1, v1, c2);
            }
            else if(!(distH < 15) && !(distC1 < 15) && (distC2 < 15)){ //1
                gl_Position = c2;
                EmitVertex();
                EndPrimitive();

                emitir(v0, v1, c2);
                emitir(v0, c2, v2);
            }

        }
//    else if(gl_in[2].gl_Position - gl_in[1].gl_Position > gl_in[0].gl_Position - gl_in[2].gl_Position &&
//            gl_in[2].gl_Position - gl_in[1].gl_Position > gl_in[1].gl_Position - gl_in[0].gl_Position){
    else if(dist(gl_in[1].gl_Position, gl_in[2].gl_Position) > dist(gl_in[2].gl_Position, gl_in[0].gl_Position) &&
            dist(gl_in[1].gl_Position, gl_in[2].gl_Position) > dist(gl_in[0].gl_Position, gl_in[1].gl_Position)){
        v0 = gl_in[1].gl_Position;
        v1 = gl_in[2].gl_Position;
        v2 = gl_in[0].gl_Position;
    }
//    else if(gl_in[0].gl_Position - gl_in[2].gl_Position > gl_in[1].gl_Position - gl_in[0].gl_Position &&
//            gl_in[0].gl_Position - gl_in[2].gl_Position > gl_in[2].gl_Position - gl_in[1].gl_Position){
    else if( dist(gl_in[2].gl_Position, gl_in[0].gl_Position) > dist(gl_in[0].gl_Position, gl_in[1].gl_Position) &&
             dist(gl_in[2].gl_Position, gl_in[0].gl_Position) > dist(gl_in[1].gl_Position, gl_in[2].gl_Position)){

        v0 = gl_in[2].gl_Position;
        v1 = gl_in[0].gl_Position;
        v2 = gl_in[1].gl_Position;
    }
    cH = (v0 + v1) / 2;
    c2 = (v1 + v2) / 2;
    c1 = (v2 + v0) / 2;
    int lod;
    float distH = dist(cH, px, py, pz);
    float distC1 = dist(c1, px, py, pz);
    float distC2 = dist(c2, px, py, pz);

    if((distH < 15) ||
       (distC1 < 15) ||
       (distC2 < 15){

        if((distH < 15) &&
           (distC1 < 15) &&
           (distC2 < 15){
            gl_Position = c1;
            EmitVertex();

            gl_Position = cH;
            EmitVertex();

            gl_Position = c2;
            EmitVertex();
            EndPrimitive();

            if(dist(, cH) >= dist(cH, c2) &&
               dist(c1, cH) >= dist(c2, c1)){
                    v0 = c1;
                    v1 = cH;
                    v2 = c2;
            }
            else if(dist(cH, c2) > dist(c2, c1) &&
                    dist(cH, c2) > dist(c1, cH)){
                    v0 = cH;
                    v1 = c2;
                    v2 = c1;
            }
            else if(dist(c2, c1) > dist(c1, cH) &&
                    dist(c2, c1) > dist(cH, c2)){
                    v0 = c2;
                    v1 = c1;
                    v2 = cH;
            }
            cH = (v0 + v1) / 2;
            c2 = (v1 + v2) / 2;
            c1 = (v2 + v0) / 2;
            int lod;
            float distH = dist(cH, px, py, pz);

            float distC1 = dist(c1, px, py, pz);

            float distC2 = dist(c2, px, py, pz);
        }


}

float dist(vec4 cEdge){
    float dist = sqrt(pow(cEdge.x - px, 2)
                    + pow(cEdge.y - py, 2)
                    + pow(cEdge.z - pz, 2));
    return dist;
}

void nivelTess(vec4 cH, vec4 cC1, vec4 cC2, float distP, vec4 v0, vec4 v1, vec4 v2){
    if(distP >= 60){

        return;
    }
    if(distP < 60 && distP >= 45){
       if(dist(cH)<= 5 || dist(cC1)<= 5 || dist(cC2)<= 5){
    }
    if(distP < 45 && distP >= 30){

    }
    if(distP < 30 && distP >= 15){

    }
    if(distP < 15){
        if(dist(cH) < 15 || dist(cC1)< 15 || dist(cC2)< 15){
            if(dist(cH) < 15 && dist(cC1) < 15 && dist(cC2) < 15){
                vec4
            }
            if(dist(cH) < 15 && !(dist(cC1) < 15) && dist(cC2) < 15){

            }
            if(dist(cH) < 15 && dist(cC1) < 15 && !(dist(cC2) < 15)){

            }
            if(!(dist(cH) < 15) && dist(cC1) < 15 && dist(cC2) < 15){

            }
            if(dist(cH) < 15 && !(dist(cC1) < 15 && dist(cC2) < 15)){

            }
            if(!(dist(cH) < 15 ) && dist(cC1) < 15 && !(dist(cC2) < 15)){

            }
            if(!(dist(cH) < 15 && dist(cC1) < 15 )&& dist(cC2) < 15){

            }
        }
    }

}
