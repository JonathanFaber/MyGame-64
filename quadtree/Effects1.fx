///////////////////////////////simplex noise///////////////////////////////////////////
float2 hash(float2 p)
{
	p = float2(dot(p, float2(127.1, 311.7)),
		dot(p, float2(269.5, 183.3)));

	return -1.0 + 2.0*frac(sin(p)*43758.5453123);
}

float noise(in float2 p)
{
	const float K1 = 0.366025404f; // (sqrt(3)-1)/2;
	const float K2 = 0.211324865f; // (3-sqrt(3))/6;

	float2 i = floor(p + (p.x + p.y)*K1);

	float2 a = p - i + (i.x + i.y)*K2;
	float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0); //float2 of = 0.5 + 0.5*float2(sign(a.x-a.y), sign(a.y-a.x));
	float2 b = a - o + K2;
	float2 c = a - 1.0 + 2.0*K2;

	float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);

	float3 n = h*h*h*h*float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));

	return dot(n, float3(70.0f, 70.0f, 70.0f));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

float3 spherize(float3 pos) {
	float3 posSquare1;

	posSquare1.x = pos.x * sqrt(abs(1.0f - pos.y*pos.y / 2.0f - pos.z*pos.z / 2.0f + pos.y*pos.y*pos.z*pos.z / 3.0f));
	posSquare1.y = pos.y * sqrt(abs(1.0f - pos.x*pos.x / 2.0f - pos.z*pos.z / 2.0f + pos.x*pos.x*pos.z*pos.z / 3.0f));
	posSquare1.z = pos.z * sqrt(abs(1.0f - pos.y*pos.y / 2.0f - pos.x*pos.x / 2.0f + pos.y*pos.y*pos.x*pos.x / 3.0f));

	return posSquare1;
}

double3 spherize_d(double3 pos1) {
	double3 posSquare2;

	posSquare2.x = pos1.x * sqrt(abs(1.0 - pos1.y*pos1.y * 0.5 - pos1.z*pos1.z * 0.5 + pos1.y*pos1.y*pos1.z*pos1.z * 0.33333333333333333));
	posSquare2.y = pos1.y * sqrt(abs(1.0 - pos1.x*pos1.x * 0.5 - pos1.z*pos1.z * 0.5 + pos1.x*pos1.x*pos1.z*pos1.z * 0.33333333333333333));
	posSquare2.z = pos1.z * sqrt(abs(1.0 - pos1.y*pos1.y * 0.5 - pos1.x*pos1.x * 0.5 + pos1.y*pos1.y*pos1.x*pos1.x * 0.33333333333333333));

	return posSquare2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	Light light;
};

cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;
};

//cbuffer cbPerPlanet
//{
//	float4x4 WVP1;
//	float4x4 World1;
//	unsigned int excludeIndices[12];
//};

Texture2D ObjTexture[16];
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct PLANET_VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
	float height : HEIGHT;
	float landTypeHeight : LANDTYPEHEIGHT;
};

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

struct SPRITE_VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct INSTANCE_VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);

	output.normal = mul(normal, World);

	output.TexCoord = inTexCoord;

	return output;
}

PLANET_VS_OUTPUT PLANET_VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float height : HEIGHT, float landTypeHeight : LANDTYPEHEIGHT)
{
	PLANET_VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);

	output.normal = mul(normal, World);

	output.TexCoord = inTexCoord;

	output.height = height;

	output.landTypeHeight = landTypeHeight;

	return output;
}

SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	output.Pos = mul(float4(inPos, 1.0f), WVP).xyww;

	output.texCoord = inPos;

	return output;
}

SPRITE_VS_OUTPUT SPRITE_VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
	SPRITE_VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);

	output.TexCoord = inTexCoord;

	return output;
}

INSTANCE_VS_OUTPUT INSTANCE_VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float3 instancePos : INSTANCEPOS, float3 instancePosBL : INSTANCEPOSBL, float3 instancePosBR : INSTANCEPOSBR, float3 instancePosTR : INSTANCEPOSTR, float3 instancePosTL : INSTANCEPOSTL, float3 instancePosSquare : INSTANCEPOSSQUARE, float instanceScale : INSTANCESCALE, uint indexID : SV_VertexID)
{
	INSTANCE_VS_OUTPUT output;


	///*

	double maxLength = 4194304.0;
	double instanceScale_d = double(instanceScale);

	if (instanceScale_d > 1024.0) {

		double div = double(1.0f / 64.0f) * double(1.0f / 32.0f) * double(1.0f / 64.0f) * double(1.0f / 32.0f);

		double3 tempPos = double3(double(inPos.x), double(inPos.y), double(inPos.z));




		tempPos.x *= instanceScale_d;
		tempPos.y *= instanceScale_d;
		tempPos.z *= instanceScale_d;

		double3 instancePosSquare_d = double3(double(instancePosSquare.x), double(instancePosSquare.y), double(instancePosSquare.z));

		tempPos.x += instancePosSquare_d.x;
		tempPos.y += instancePosSquare_d.y;
		tempPos.z += instancePosSquare_d.z;

		//float3 tempPos1 = float3(tempPos.x, tempPos.y, tempPos.z);
		tempPos.x *= div;
		tempPos.y *= div;
		tempPos.z *= div;

		tempPos = spherize_d(tempPos);
		tempPos.x *= maxLength;
		tempPos.y *= maxLength;
		tempPos.z *= maxLength;


		double3 middlePos = spherize_d(double3(instancePosSquare_d.x*div, instancePosSquare_d.y*div, instancePosSquare_d.z*div));
		middlePos.x *= maxLength;
		middlePos.y *= maxLength;
		middlePos.z *= maxLength;

		tempPos.x += double(instancePos.x) - middlePos.x;
		tempPos.y += double(instancePos.y) - middlePos.y;
		tempPos.z += double(instancePos.z) - middlePos.z;


		inPos.x = float(tempPos.x);
		inPos.y = float(tempPos.y);
		inPos.z = float(tempPos.z);
	}
	else {
		// Do bilinear interpolation

		inPos.x = (instancePosBL.x * (1.0f - inTexCoord.x) * (1.0f - inTexCoord.y) + instancePosBR.x * (1.0f + inTexCoord.x) * (1.0f - inTexCoord.y) + instancePosTR.x * (1.0f + inTexCoord.x) * (1.0f + inTexCoord.y) + instancePosTL.x * (1.0f - inTexCoord.x) * (1.0f + inTexCoord.y)) / 4.0f;
		inPos.y = (instancePosBL.y * (1.0f - inTexCoord.x) * (1.0f - inTexCoord.y) + instancePosBR.y * (1.0f + inTexCoord.x) * (1.0f - inTexCoord.y) + instancePosTR.y * (1.0f + inTexCoord.x) * (1.0f + inTexCoord.y) + instancePosTL.y * (1.0f - inTexCoord.x) * (1.0f + inTexCoord.y)) / 4.0f;
		inPos.z = (instancePosBL.z * (1.0f - inTexCoord.x) * (1.0f - inTexCoord.y) + instancePosBR.z * (1.0f + inTexCoord.x) * (1.0f - inTexCoord.y) + instancePosTR.z * (1.0f + inTexCoord.x) * (1.0f + inTexCoord.y) + instancePosTL.z * (1.0f - inTexCoord.x) * (1.0f + inTexCoord.y)) / 4.0f;
	}


	//*/
	

	output.Pos = mul(inPos, WVP);

	output.normal = mul(normal, World);

	output.TexCoord = inTexCoord;

	return output;
}




float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuse;


	diffuse = ObjTexture[0].Sample( ObjSamplerState, input.TexCoord );
	//diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);

	/*

	const int octaves = 16;
	float f[octaves];
	float fsum = 0.0f;
	float amplitude = 0.5f;
	float frequency = 2.0f;

	for (int i = 0; i < octaves; i++) {
		f[i] = (noise(float2(input.TexCoord.x*frequency + i, input.TexCoord.y*frequency + i)));
		f[i] = (1.0f - abs(f[i])) * amplitude;
		fsum += f[i];

		amplitude *= 0.5f;
		frequency *= 2.0f;
	}

	//fsum *= 0.5f;
	//fsum = 0.5 + 0.5*fsum; // so that its between 0 and 1

	//f *= smoothstep(0.0, 0.005, abs(input.TexCoord.x));
	*/

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);


	//finalColor = float3(fsum, fsum, fsum);


	return float4(finalColor, diffuse.a);
	//return diffuse;
}

float4 PLANET_PS(PLANET_VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuse;


	//diffuse = ObjTexture[0].Sample( ObjSamplerState, input.TexCoord );
	//diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);

	if (input.height <= 0.0f)						// water
		diffuse = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (input.landTypeHeight <= 0.0f)			// hills
		diffuse = ObjTexture[1].Sample(ObjSamplerState, input.TexCoord);
	else											// plains
		diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);

	//else if (input.height <= 0.0001f)
	//	diffuse = float4(1.0f, 1.0f, 0.0f, 1.0f); // yellow sand

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);

	//finalColor = float3(fsum, fsum, fsum);


	return float4(finalColor, diffuse.a);
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample(ObjSamplerState, input.texCoord);
}

float4 SPRITE_PS(SPRITE_VS_OUTPUT input) : SV_TARGET
{
	float4 diffuse;

	diffuse = ObjTexture[1].Sample(ObjSamplerState, input.TexCoord);

	//if (diffuse.a <= 0.15f)
	//	discard;
	//else
	//	diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);

	float3 finalColor;

	finalColor = diffuse;

	//return float4(float3(0.0f, 0.0f, 1.0f - finalColor.b), diffuse.a);
	return float4(finalColor, diffuse.a);
}

float4 INSTANCE_PS(INSTANCE_VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);


	/*
	float4 diffuse;


	//diffuse = ObjTexture[0].Sample( ObjSamplerState, input.TexCoord );
	//diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);

	const int octaves = 16;
	float f[octaves];
	float fsum = 0.0f;
	float amplitude = 0.5f;
	float frequency = 2.0f;

	for (int i = 0; i < octaves; i++) {
		f[i] = (noise(float2(input.TexCoord.x*frequency + i, input.TexCoord.y*frequency + i)));
		f[i] = (1.0f - abs(f[i])) * amplitude;
		fsum += f[i];

		amplitude *= 0.5f;
		frequency *= 2.0f;
	}

	//fsum *= 0.5f;
	//fsum = 0.5 + 0.5*fsum; // so that its between 0 and 1

	//f *= smoothstep(0.0, 0.005, abs(input.TexCoord.x));

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);


	finalColor = float3(fsum, fsum, fsum);
	*/
	float3 finalColor = float3(1.0f, 1.0f, 1.0f);


	//return float4(finalColor, diffuse.a);
	return float4(finalColor, 1.0f);
}
