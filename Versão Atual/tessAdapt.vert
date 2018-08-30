#version 430 core

in vec2 position;

uniform float amp;

out float h;
out vec3 vNormal;
out vec4 vColor;

out vec3 vPosition;

#define NUM_OCTAVES 5


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
    vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}


float fbm(vec2 x) {
	float v = 0.0;
	float a = 1;
	vec2 shift = vec2(100);
	// Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

void main(){
    h 			= fbm(position);//amp * cos((position.x)) * sin((position.y))+1;
    vNormal.x	= -sin(position.x)*sin(position.y);
    vNormal.z	= cos(position.x)*cos(position.y);
    vNormal.y   = h;

    vColor.r 	= h / 3.0;
    vColor.g 	= 0.0;
    vColor.b 	= 1.0 - h / 4.0;
    vColor.a 	= 1.0f;

    vPosition 	= vec3(position.x, h, position.y);
}
