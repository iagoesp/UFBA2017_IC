#version 330 core

in float y;
out vec4 frag;

void main(){
  float sqrt_y = sqrt(abs(y));

  if(y>=0.5f)
    frag = vec4(sqrt_y/1.1f,0,sqrt_y/16,1);
  if(y>0.2f && y<0.5f)
    frag = vec4(sqrt_y/1.2f,0,sqrt_y/16,1);
  if(0.2f>=y && y>-0.2f)
    frag = vec4(0.1+y*1.3,0,0.1-y*2,1);
  else if(y<=-0.2f)
    frag = vec4(sqrt_y/16,0,sqrt_y,1);

//  if (new_y > 1.0)
//    frag = vec4( sqrt_y, sqrt_y/16, sqrt_y/8, 1.0); //2
//
//  else if (new_y > 0.7 && new_y <= 1.0)
//      frag = vec4( new_y - sqrt_y/2, sqrt_y/8, sqrt_y/4, 1.0); //1
//
//	else if (new_y > 0.4 && new_y <= 0.7)
//    frag = vec4( new_y - sqrt_y/2, sqrt_y/(y+1.f), new_y - sqrt_y/2, 1.0); //3
//
//  else if (new_y > 0.0 && new_y <= 0.4)
//    frag = vec4( 0.0, 0.0, 1.0- sqrt_y, 1.0); //4
//
//  else if (new_y < 0.0)
//     //5
//
//  else if (y > 0.0 && y <= 0.2)
//    frag = vec4( 0.0, 1.0, 1.0, 1.0); //6
//
//  else if (y <= 0.0)
//    frag = vec4( 1.0, 1.0, 0.6, 1.0); //7

//	if (y > 1.0)
//  {
//    frag = vec4( 1.0, 0.0, 0.0, 1.0);
//  }
//	else if (y > 0.7 && y <= 1.0)
//  {
//    frag = vec4( 1.0, 1.0, 0.0, 1.0);
//  }
//  else if (y > 0.3 && y <= 0.7){
//    frag = vec4( 1.0, 1.0, 1.0, 1.0);
//  }
//  else if (y > 0.0 && y <= 0.3){
//    frag = vec4( 0.0, 1.0, 1.0, 1.0);
//  }
//  else if (y <= 0.0){
//    frag = vec4( 0.0, 0.0, 1.0, 1.0);
//  }
}
