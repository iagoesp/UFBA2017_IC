in float p;
in vec3 Normal;
in vec3 vPos;
in vec3 camPos;
in vec3 eyePos;
//in vec3 tePos;
out vec4 FragColor;

vec3 lightDir = vec3(0.f, 1.f, -1.f);

float normaScale(float value)
{
  return value/10000000.f;
}

void main(){
    float len = length(vPos); //o tePos ou o p substituiria o vPos
    len = normaScale(len);
    vec3 height;
    height = vec3(0,0,p/15.f);
    vec3 n = normalize(Normal);
    vec3 l = normalize(lightDir);

    float light = 0.001 + max(0.0, dot(n, l));
    vec3 color = vec3(sqrt(light))+tan(height);
    FragColor = vec4(color, 1.0);//+Iamb+Idiff+Ispec;
}
