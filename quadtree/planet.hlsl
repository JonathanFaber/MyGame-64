#include "noise3d.hlsli"
#include "noise4d.hlsli"

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

cbuffer cbPerPlanet : register (b0)
{
	//float4x4 WVP;
	//float4x4 World;
	float3 Translation;
	float4x4 View;
	float4x4 Proj;
};

Texture2D ObjTexture[16];
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 position : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
	float height : HEIGHT;
	float landTypeHeight : LANDTYPEHEIGHT;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float height : HEIGHT, float landTypeHeight : LANDTYPEHEIGHT)
{
	VS_OUTPUT output;

	//inPos.w = 1.0f;
	output.Pos = float4(inPos.xyz + Translation, inPos.w);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Proj);

	output.position = inPos.xyz + Translation;

	output.normal = normal;

	output.TexCoord = inTexCoord;

	output.height = height;

	output.landTypeHeight = landTypeHeight;

	return output;
}

[maxvertexcount(72)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream <VS_OUTPUT> triStream) {
	// default terrain
	for (int i = 0; i < 3; i++) {
		triStream.Append(input[i]);
	}
	triStream.RestartStrip();
	//*
	// grass

	if (input[0].landTypeHeight <= 0.5f && input[0].height > 0.0001f && sqrt(input[0].position.x * input[0].position.x + input[0].position.y * input[0].position.y + input[0].position.z * input[0].position.z) < 40.0f) {
		VS_OUTPUT v[4];

		float3 fNormal = normalize(cross(input[2].position - input[1].position, input[1].position - input[0].position));
		if (dot(input[0].normal, fNormal) < 0.0f)
			fNormal *= -1;

		float3 center = (input[0].position + input[1].position + input[2].position) / 3.0f;

		float red = (cnoise(center - Translation) - 0.5f) / 12.0f;

		float windNoise = (cnoise(float4(center - Translation, timeElaps)) + cnoise(float4((center - Translation) * 2.0f, timeElaps)) / 2.0f) / 1.5f;

		float3 fTangent = input[2].position - input[0].position;
		float3 animatedNormal = normalize(lerp(fNormal, fTangent, windNoise));

		v[0] = input[0];
		v[0].height = 64.0f;
		v[0].landTypeHeight = red;
		v[0].position = input[0].position;
		v[0].normal = fNormal;
		v[0].TexCoord = float2(0.0f, 0.0f);
		v[0].Pos = mul(float4(v[0].position, 1.0f), View);
		v[0].Pos = mul(v[0].Pos, Proj);

		v[1] = input[0];
		v[1].height = 64.0f;
		v[1].landTypeHeight = red;
		v[1].position = v[0].position + animatedNormal;
		v[1].normal = fNormal;
		v[1].TexCoord = float2(0.0f, 1.0f);
		v[1].Pos = mul(float4(v[1].position, 1.0f), View);
		v[1].Pos = mul(v[1].Pos, Proj);

		v[2] = input[0];
		v[2].height = 64.0f;
		v[2].landTypeHeight = red;
		v[2].position = (input[1].position + input[2].position) / 2.0f;
		v[2].normal = fNormal;
		v[2].TexCoord = float2(1.0f, 0.0f);
		v[2].Pos = mul(float4(v[2].position, 1.0f), View);
		v[2].Pos = mul(v[2].Pos, Proj);

		v[3] = input[0];
		v[3].height = 64.0f;
		v[3].landTypeHeight = red;
		v[3].position = v[2].position + animatedNormal;
		v[3].normal = fNormal;
		v[3].TexCoord = float2(1.0f, 1.0f);
		v[3].Pos = mul(float4(v[3].position, 1.0f), View);
		v[3].Pos = mul(v[3].Pos, Proj);

		triStream.Append(v[0]);
		triStream.Append(v[2]);
		triStream.Append(v[3]);
		triStream.RestartStrip();

		triStream.Append(v[0]);
		triStream.Append(v[3]);
		triStream.Append(v[1]);
		triStream.RestartStrip();

		v[0] = input[0];
		v[0].height = 64.0f;
		v[0].landTypeHeight = red;
		v[0].position = input[1].position;
		v[0].normal = fNormal;
		v[0].TexCoord = float2(0.0f, 0.0f);
		v[0].Pos = mul(float4(v[0].position, 1.0f), View);
		v[0].Pos = mul(v[0].Pos, Proj);

		v[1] = input[0];
		v[1].height = 64.0f;
		v[1].landTypeHeight = red;
		v[1].position = v[0].position + animatedNormal;
		v[1].normal = fNormal;
		v[1].TexCoord = float2(0.0f, 1.0f);
		v[1].Pos = mul(float4(v[1].position, 1.0f), View);
		v[1].Pos = mul(v[1].Pos, Proj);

		v[2] = input[0];
		v[2].height = 64.0f;
		v[2].landTypeHeight = red;
		v[2].position = (input[0].position + input[2].position) / 2.0f;
		v[2].normal = fNormal;
		v[2].TexCoord = float2(1.0f, 0.0f);
		v[2].Pos = mul(float4(v[2].position, 1.0f), View);
		v[2].Pos = mul(v[2].Pos, Proj);

		v[3] = input[0];
		v[3].height = 64.0f;
		v[3].landTypeHeight = red;
		v[3].position = v[2].position + animatedNormal;
		v[3].normal = fNormal;
		v[3].TexCoord = float2(1.0f, 1.0f);
		v[3].Pos = mul(float4(v[3].position, 1.0f), View);
		v[3].Pos = mul(v[3].Pos, Proj);

		triStream.Append(v[0]);
		triStream.Append(v[2]);
		triStream.Append(v[3]);
		triStream.RestartStrip();

		triStream.Append(v[0]);
		triStream.Append(v[3]);
		triStream.Append(v[1]);
		triStream.RestartStrip();

		v[0] = input[0];
		v[0].height = 64.0f;
		v[0].landTypeHeight = red;
		v[0].position = input[2].position;
		v[0].normal = fNormal;
		v[0].TexCoord = float2(0.0f, 0.0f);
		v[0].Pos = mul(float4(v[0].position, 1.0f), View);
		v[0].Pos = mul(v[0].Pos, Proj);

		v[1] = input[0];
		v[1].height = 64.0f;
		v[1].landTypeHeight = red;
		v[1].position = v[0].position + animatedNormal;
		v[1].normal = fNormal;
		v[1].TexCoord = float2(0.0f, 1.0f);
		v[1].Pos = mul(float4(v[1].position, 1.0f), View);
		v[1].Pos = mul(v[1].Pos, Proj);

		v[2] = input[0];
		v[2].height = 64.0f;
		v[2].landTypeHeight = red;
		v[2].position = (input[0].position + input[1].position) / 2.0f;
		v[2].normal = fNormal;
		v[2].TexCoord = float2(1.0f, 0.0f);
		v[2].Pos = mul(float4(v[2].position, 1.0f), View);
		v[2].Pos = mul(v[2].Pos, Proj);

		v[3] = input[0];
		v[3].height = 64.0f;
		v[3].landTypeHeight = red;
		v[3].position = v[2].position + animatedNormal;
		v[3].normal = fNormal;
		v[3].TexCoord = float2(1.0f, 1.0f);
		v[3].Pos = mul(float4(v[3].position, 1.0f), View);
		v[3].Pos = mul(v[3].Pos, Proj);

		triStream.Append(v[0]);
		triStream.Append(v[2]);
		triStream.Append(v[3]);
		triStream.RestartStrip();

		triStream.Append(v[0]);
		triStream.Append(v[3]);
		triStream.Append(v[1]);
		triStream.RestartStrip();
	}
	//*/
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuse;


	//diffuse = ObjTexture[0].Sample( ObjSamplerState, input.TexCoord );
	//diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);

	if (input.height == 64.0f) {						// grass from geometry shader
		if (input.TexCoord.y > 0.5f)
			discard;

		clamp(input.TexCoord.y, 0.05f, 0.5f);
		diffuse = ObjTexture[3].Sample(ObjSamplerState, input.TexCoord);
		diffuse.r += input.landTypeHeight;
	}
	else {
		if (input.height <= 0.0f)						// water
			diffuse = float4(0.0f, 0.0f, 1.0f, 1.0f);
		else if (input.height <= 0.0001f)				// sand
			diffuse = ObjTexture[2].Sample(ObjSamplerState, input.TexCoord);
		else if (input.landTypeHeight > 0.5f)			// mountains
			diffuse = ObjTexture[1].Sample(ObjSamplerState, input.TexCoord);
		else if (input.landTypeHeight > 0.0f)			// plains
			diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);
		else											// hills
			diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);

		//diffuse.a = 1.0f;
	}

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);


	return float4(finalColor, diffuse.a);
}