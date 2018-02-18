#version 330 core

in vec3 y;
in vec3 tePatchDistance;

out vec4 frag;

void main(){
    frag = vec4( tePatchDistance.x , tePatchDistance.y , tePatchDistance.z , 1.0); //escala 1
    /*
    float new_y = p1;

    if (new_y > 1.0)
        frag = vec4( 1.0 , 0.0 , 0.0 , 1.0); //escala 1

    else if (new_y > 0.5 && new_y <= 1.0)
        frag = vec4( 1.0, 0, 0.5, 1.0);     //escala 2

	else if (new_y > 0.0 && new_y <= 0.5)
        frag = vec4( 1.0 , 0.0 , 1.0, 1.0); //escala 3

    else if (new_y > -0.5 && new_y <= 0.0)
        frag = vec4( 0.5, 0.0, 1.0, 1.0);   //escala 4

    else if (new_y < -0.5)
        frag = vec4( 0.0, 0.0, 1.0, 1.0);   //escala 5
*/
}
