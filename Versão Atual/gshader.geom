

void main(){
    vec4 v0, v1, v2, c1, c2, cH;
    if(gl_in[1].gl_Position - gl_in[0].gl_Position >= gl_in[2].gl_Position - gl_in[1].gl_Position &&
       gl_in[1].gl_Position - gl_in[0].gl_Position >= gl_in[0].gl_Position - gl_in[2].gl_Position){
        v0 = gl_in[0].gl_Position;
        v1 = gl_in[1].gl_Position;
        v2 = gl_in[2].gl_Position;
    }
    else if(gl_in[2].gl_Position - gl_in[1].gl_Position > gl_in[0].gl_Position - gl_in[2].gl_Position &&
            gl_in[2].gl_Position - gl_in[1].gl_Position > gl_in[1].gl_Position - gl_in[0].gl_Position){
        v0 = gl_in[1].gl_Position;
        v1 = gl_in[2].gl_Position;
        v2 = gl_in[0].gl_Position;
    }
    else if(gl_in[0].gl_Position - gl_in[2].gl_Position > gl_in[1].gl_Position - gl_in[0].gl_Position &&
            gl_in[0].gl_Position - gl_in[2].gl_Position > gl_in[2].gl_Position - gl_in[1].gl_Position){
        v0 = gl_in[2].gl_Position;
        v1 = gl_in[0].gl_Position;
        v2 = gl_in[1].gl_Position;
    }
    cH = (v0 + v1) / 2;
    c2 = (v1 + v2) / 2;
    c1 = (v2 + v0) / 2;
    int lod;
    float distH = sqrt((cH.x - px)*(cH.x - px)
                  + (cH.y - py)*(cH.y - py)
                  + (cH.z - pz)*(cH.z - pz));

    float distC1 = sqrt((c1.x - px)*(c1.x - px)
                  + (c1.y - py)*(c1.y - py)
                  + (c1.z - pz)*(c1.z - pz));

    float distC2 = sqrt((c2.x - px)*(c2.x - px)
                  + (c2.y - py)*(c2.y - py)
                  + (c2.z - pz)*(c2.z - pz));

    if(distH < 30 || distC1 < 30 || distC2 < 30){
        if(distH < 30 && distC1 < 30 && distC2 < 30){
            gl_Position = c1;
            EmitVertex();

            gl_Position = cH;
            EmitVertex();

            gl_Position = c2;
            EmitVertex();
        }
        else if(dist(cH) < 15 && !(dist(cC1) < 15) && dist(cC2) < 15){

        }
        else if(dist(cH) < 15 && dist(cC1) < 15 && !(dist(cC2) < 15)){

        }
        else if(!(dist(cH) < 15) && dist(cC1) < 15 && dist(cC2) < 15){

        }
        else if(dist(cH) < 15 && !(dist(cC1) < 15 && dist(cC2) < 15)){

        }
        else if(!(dist(cH) < 15 ) && dist(cC1) < 15 && !(dist(cC2) < 15)){

        }
        else if(!(dist(cH) < 15 && dist(cC1) < 15 )&& dist(cC2) < 15){

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
