#version 330 core

in float y;

out vec4 fragColor;

void main(){
    float new_y = y;

    if (new_y > 1.0)
        fragColor = vec4( 1.0 , 0.0 , 0.0 , 1.0); //escala 1

    else if (new_y > 0.5 && new_y <= 1.0)
        fragColor = vec4( 1.0, 0, 0.5, 1.0);     //escala 2

	else if (new_y > 0.0 && new_y <= 0.5)
        fragColor = vec4( 1.0 , 0.0 , 1.0, 1.0); //escala 3

    else if (new_y > -0.5 && new_y <= 0.0)
        fragColor = vec4( 0.5, 0.0, 1.0, 1.0);   //escala 4

    else if (new_y < -0.5)
        fragColor = vec4( 0.0, 0.0, 1.0, 1.0);   //escala 5

}
