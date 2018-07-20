

void main(){
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    tess(v0, v1, v2);
}

void tess(vec4 v0, vec4 v1, vec4 v2){
    vec4 cC1;
    vec4 cC2;
    vec4 cH;
    if(v1-v0 > v2-v1 && v1-v0 > v0-v2){
        cH  = (v0 + v1) / 2;
        cC2 = (v1 + v2) / 2;
        cC1 = (v2 + v0) / 2;
    }
    else if(v2-v1 > v1-v0 && v2-v1 > v0-v2){
        cH  = (v1 + v2) / 2;
        cC2 = (v2 + v0) / 2;
        cC1 = (v0 + v1) / 2;
    }
    else if(v0-v2 > v1-v0 && v0-v2 > v2-v1){
        cH  = (v2 + v0) / 2;
        cC2 = (v0 + v1) / 2;
        cC1 = (v1 + v2) / 2;
    }
    float dist=min(dist(cC2), min(dist(cH), dist(cC1)));
    bool subdiv = false;
    nivelTess(cH, cC1, cC2, dist, subdiv, v0, v1, v2);
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
