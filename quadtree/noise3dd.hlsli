//
// GLSL textureless classic 3D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-10-11
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/stegu/webgl-noise
//

double4 fract(double4 vec4) {
	return vec4 - floor(vec4);
}

double3 fract(double3 vec3) {
	return vec3 - floor(vec3);
}

double2 fract(double2 vec2) {
	return vec2 - floor(vec2);
}

double fract(double vec) {
	return vec - floor(vec);
}

double mod(double x, double y)
{
	return x - y * double(floor(float(x) / float(y)));
}

double3 mod289(double3 x)
{
	return x - floor(x * (0.00346020761246)) * 289.0;
}

double4 mod289(double4 x)
{
	return x - floor(x * (0.00346020761246)) * 289.0;
}

double4 permute(double4 x)
{
	return mod289(((x*34.0) + 1.0)*x);
}

double4 taylorInvSqrt(double4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

double3 fade(double3 t) {
	return t*t*t*(t*(t*6.0 - 15.0) + 10.0);
}

// Classic Perlin noise
double cnoise(double3 P)
{
	double3 Pi0 = floor(P); // Integer part for indexing
	double3 Pi1 = Pi0 + double3(1.0, 1.0, 1.0); // Integer part + 1
	Pi0 = mod289(Pi0);
	Pi1 = mod289(Pi1);
	double3 Pf0 = P - floor(P); // Fractional part for interpolation
	double3 Pf1 = Pf0 - double3(1.0, 1.0, 1.0); // Fractional part - 1.0
	double4 ix = double4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
	double4 iy = double4(Pi0.yy, Pi1.yy);
	double4 iz0 = Pi0.zzzz;
	double4 iz1 = Pi1.zzzz;

	double4 ixy = permute(permute(ix) + iy);
	double4 ixy0 = permute(ixy + iz0);
	double4 ixy1 = permute(ixy + iz1);

	double4 gx0 = ixy0 * (0.1428571428571429);
	double4 gy0 = fract(floor(gx0) * (0.1428571428571429)) - 0.5;
	gx0 = fract(gx0);
	double4 gz0 = double4(0.5f, 0.5f, 0.5f, 0.5f) - abs(gx0) - abs(gy0);
	double4 sz0 = step(gz0, double4(0.0, 0.0, 0.0, 0.0));
	gx0 -= sz0 * (step(0.0, gx0) - 0.5);
	gy0 -= sz0 * (step(0.0, gy0) - 0.5);

	double4 gx1 = ixy1 * (0.1428571428571429);
	double4 gy1 = fract(floor(gx1) * (0.1428571428571429)) - 0.5;
	gx1 = fract(gx1);
	double4 gz1 = double4(0.5, 0.5, 0.5, 0.5) - abs(gx1) - abs(gy1);
	double4 sz1 = step(gz1, double4(0.0, 0.0, 0.0, 0.0));
	gx1 -= sz1 * (step(0.0, gx1) - 0.5);
	gy1 -= sz1 * (step(0.0, gy1) - 0.5);

	double3 g000 = double3(gx0.x, gy0.x, gz0.x);
	double3 g100 = double3(gx0.y, gy0.y, gz0.y);
	double3 g010 = double3(gx0.z, gy0.z, gz0.z);
	double3 g110 = double3(gx0.w, gy0.w, gz0.w);
	double3 g001 = double3(gx1.x, gy1.x, gz1.x);
	double3 g101 = double3(gx1.y, gy1.y, gz1.y);
	double3 g011 = double3(gx1.z, gy1.z, gz1.z);
	double3 g111 = double3(gx1.w, gy1.w, gz1.w);

	double4 norm0 = taylorInvSqrt(double4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
	g000 *= norm0.x;
	g010 *= norm0.y;
	g100 *= norm0.z;
	g110 *= norm0.w;
	double4 norm1 = taylorInvSqrt(double4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
	g001 *= norm1.x;
	g011 *= norm1.y;
	g101 *= norm1.z;
	g111 *= norm1.w;

	double n000 = dot(g000, Pf0);
	double n100 = dot(g100, double3(Pf1.x, Pf0.yz));
	double n010 = dot(g010, double3(Pf0.x, Pf1.y, Pf0.z));
	double n110 = dot(g110, double3(Pf1.xy, Pf0.z));
	double n001 = dot(g001, double3(Pf0.xy, Pf1.z));
	double n101 = dot(g101, double3(Pf1.x, Pf0.y, Pf1.z));
	double n011 = dot(g011, double3(Pf0.x, Pf1.yz));
	double n111 = dot(g111, Pf1);

	double3 fade_xyz = fade(Pf0);
	double4 n_z = lerp(double4(n000, n100, n010, n110), double4(n001, n101, n011, n111), fade_xyz.z);
	double2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
	double n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
	return 2.2 * n_xyz;
}

// Classic Perlin noise, periodic variant
double pnoise(double3 P, double3 rep)
{
	double3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
	double3 Pi1 = mod(Pi0 + double3(1.0, 1.0, 1.0), rep); // Integer part + 1, mod period
	Pi0 = mod289(Pi0);
	Pi1 = mod289(Pi1);
	double3 Pf0 = fract(P); // Fractional part for interpolation
	double3 Pf1 = Pf0 - double3(1.0, 1.0, 1.0); // Fractional part - 1.0
	double4 ix = double4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
	double4 iy = double4(Pi0.yy, Pi1.yy);
	double4 iz0 = Pi0.zzzz;
	double4 iz1 = Pi1.zzzz;

	double4 ixy = permute(permute(ix) + iy);
	double4 ixy0 = permute(ixy + iz0);
	double4 ixy1 = permute(ixy + iz1);

	double4 gx0 = ixy0 * (0.1428571428571429);
	double4 gy0 = fract(floor(gx0) * (0.1428571428571429)) - 0.5;
	gx0 = fract(gx0);
	double4 gz0 = double4(0.5f, 0.5f, 0.5f, 0.5f) - abs(gx0) - abs(gy0);
	double4 sz0 = step(gz0, double4(0.0f, 0.0f, 0.0f, 0.0f));
	gx0 -= sz0 * (step(0.0, gx0) - 0.5);
	gy0 -= sz0 * (step(0.0, gy0) - 0.5);

	double4 gx1 = ixy1 * (0.1428571428571429);
	double4 gy1 = fract(floor(gx1) * (0.1428571428571429)) - 0.5;
	gx1 = fract(gx1);
	double4 gz1 = double4(0.5, 0.5, 0.5, 0.5) - abs(gx1) - abs(gy1);
	double4 sz1 = step(gz1, double4(0.0, 0.0, 0.0, 0.0));
	gx1 -= sz1 * (step(0.0, gx1) - 0.5);
	gy1 -= sz1 * (step(0.0, gy1) - 0.5);

	double3 g000 = double3(gx0.x, gy0.x, gz0.x);
	double3 g100 = double3(gx0.y, gy0.y, gz0.y);
	double3 g010 = double3(gx0.z, gy0.z, gz0.z);
	double3 g110 = double3(gx0.w, gy0.w, gz0.w);
	double3 g001 = double3(gx1.x, gy1.x, gz1.x);
	double3 g101 = double3(gx1.y, gy1.y, gz1.y);
	double3 g011 = double3(gx1.z, gy1.z, gz1.z);
	double3 g111 = double3(gx1.w, gy1.w, gz1.w);

	double4 norm0 = taylorInvSqrt(double4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
	g000 *= norm0.x;
	g010 *= norm0.y;
	g100 *= norm0.z;
	g110 *= norm0.w;
	double4 norm1 = taylorInvSqrt(double4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
	g001 *= norm1.x;
	g011 *= norm1.y;
	g101 *= norm1.z;
	g111 *= norm1.w;

	double n000 = dot(g000, Pf0);
	double n100 = dot(g100, double3(Pf1.x, Pf0.yz));
	double n010 = dot(g010, double3(Pf0.x, Pf1.y, Pf0.z));
	double n110 = dot(g110, double3(Pf1.xy, Pf0.z));
	double n001 = dot(g001, double3(Pf0.xy, Pf1.z));
	double n101 = dot(g101, double3(Pf1.x, Pf0.y, Pf1.z));
	double n011 = dot(g011, double3(Pf0.x, Pf1.yz));
	double n111 = dot(g111, Pf1);

	double3 fade_xyz = fade(Pf0);
	double4 n_z = lerp(double4(n000, n100, n010, n110), double4(n001, n101, n011, n111), fade_xyz.z);
	double2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
	double n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x);
	return 2.2 * n_xyz;
}
