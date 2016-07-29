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
cbuffer cbPerFrameVS : register (b1)
{
	float timeElaps;
	float3 pad1;
};

struct Light
{
	float4 ambient;
	float4 diffuse;
	float3 dir;
};

cbuffer cbPerFramePS : register (b0)
{
	Light light;
	float pad2;
};

cbuffer cbPerObject : register (b0)
{
	float4x4 WVP;
	float4x4 World;
};

Texture2D ObjTexture[16];
SamplerState ObjSamplerState;
TextureCube SkyMap;

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
