#version 430 core
#extension GL_EXT_gpu_shader4 : enable
#define F2 0.366025403
#define G2 0.211324865

in vec2 position;

uniform float amp;

out float h;
out vec3 vNormal;
out vec4 vColor;

out vec3 vPosition;

float gx0, gy0, gx1, gy1, gx2, gy2;

float grad2lut[8][2] = {
		{ -1.0f, -1.0f }, { 1.0f, 0.0f } , { -1.0f, 0.0f } , { 1.0f, 1.0f } ,
		{ -1.0f, 1.0f } , { 0.0f, -1.0f } , { 0.0f, 1.0f } , { 1.0f, -1.0f }
};

void grad2_0(int hash) {
		int h = hash & 7;
		gx0 = grad2lut[h][0];
		gy0 = grad2lut[h][1];
		return;
}

void grad2_1(int hash) {
		int h = hash & 7;
		gx1 = grad2lut[h][0];
		gy1 = grad2lut[h][1];
		return;
}

void grad2_2(int hash) {
		int h = hash & 7;
		gx2 = grad2lut[h][0];
		gy2 = grad2lut[h][1];
		return;
}

int perm[512] = {
	  151,160,137,91,90,15, 131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
  };

vec3 dnoise(vec2 v){
	float n0, n1, n2; // Noise contributions from the three corners
	// Skew the input space to determine which simplex cell we're in
	float s = (v.x+v.y)*F2; // Hairy factor for 2D
	float xs = v.x + s;
	float ys = v.y + s;
	int i = int(floor(xs));
	int j = int(floor(ys));

	float t = (i+j)*G2;
	float X0 = i-t; // Unskew the cell origin back to (x,y) space
	float Y0 = j-t;
	float x0 = v.x-X0; // The x,y distances from the cell origin
	float y0 = v.y-Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if(x0>y0) {i1=1; j1=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else {i1=0; j1=1;}      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Wrap the GLuinteger indices at 256, to avoid indexing details::perm[] out of bounds
	int ii = i & 0xff;
	int jj = j & 0xff;

	 /* Gradients at simplex corners */

/* Calculate the contribution from the three corners */
	float t0 = 0.5f - x0 * x0 - y0 * y0;
	float t20, t40;
	if( t0 < 0.0f ) t40 = t20 = t0 = n0 = gx0 = gy0 = 0.0f; /* No influence */
	else {
		grad2_0( perm[ii + perm[jj]]);
		t20 = t0 * t0;
		t40 = t20 * t20;
		n0 = t40 * ( gx0 * x0 + gy0 * y0 );
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;
	float t21, t41;
	if( t1 < 0.0f ) t21 = t41 = t1 = n1 = gx1 = gy1 = 0.0f; /* No influence */
	else {
		grad2_1( perm[ii + i1 + perm[jj + j1]]);
		t21 = t1 * t1;
		t41 = t21 * t21;
		n1 = t41 * ( gx1 * x1 + gy1 * y1 );
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;
	float t22, t42;
	if( t2 < 0.0f ) t42 = t22 = t2 = n2 = gx2 = gy2 = 0.0f; /* No influence */
	else {
		grad2_2(perm[ii + 1 + perm[jj + 1]]);
		t22 = t2 * t2;
		t42 = t22 * t22;
		n2 = t42 * ( gx2 * x2 + gy2 * y2 );
	}

	float temp0 = t20 * t0 * ( gx0* x0 + gy0 * y0 );
	float dnoise_dx = temp0 * x0;
	float dnoise_dy = temp0 * y0;
	float temp1 = t21 * t1 * ( gx1 * x1 + gy1 * y1 );
	dnoise_dx += temp1 * x1;
	dnoise_dy += temp1 * y1;
	float temp2 = t22 * t2 * ( gx2* x2 + gy2 * y2 );
	dnoise_dx += temp2 * x2;
	dnoise_dy += temp2 * y2;
	dnoise_dx *= -8.0f;
	dnoise_dy *= -8.0f;
	dnoise_dx += t40 * gx0 + t41 * gx1 + t42 * gx2;
	dnoise_dy += t40 * gy0 + t41 * gy1 + t42 * gy2;
	dnoise_dx *= 40.0f; /* Scale derivative to match the noise scaling */
	dnoise_dy *= 40.0f;

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the GLuinterval [-1,1].
	return vec3( 40.0f * (n0 + n1 + n2), dnoise_dx, dnoise_dy ); // TODO: The scale factor is preliminary!
}

float iqfBm(vec2 v, int octaves, float lacunarity, float gain )
{
	float sum	= 0.0;
	float amp	= 1f;
	float dx	= 0.0;
	float dy	= 0.0;
	float freq	= 1.0;
	for( int i = 0; i < octaves; i++ ) {
		vec3 d = dnoise( v * freq );
		dx += d.y;
		dy += d.y;
		sum += amp * d.x / ( 1.0 + dx*dx + dy*dy );
		freq *= lacunarity;
		amp *= gain;
	}

	return sum;
}

void main(){
    h 			= iqfBm (position, 32, 2.0f, 0.55f);//amp * cos((position.x)) * sin((position.y))+1;
    vNormal.x	= (position.x);
    vNormal.z	= (position.y);
    vNormal.y   = h;

    vColor.r 	= h*2 / 3.0;
    vColor.g 	= 0.0;
    vColor.b 	= 1.0 - h*2 / 4.0;
    vColor.a 	= 1.0f;

    vPosition 	= vec3(position.x, h, position.y);
}
