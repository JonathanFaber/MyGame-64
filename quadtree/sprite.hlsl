Texture2D ObjTexture[16];
SamplerState ObjSamplerState;

cbuffer cbPerObject : register (b0)
{
	float4x4 WVP;
	float4x4 World;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);

	output.TexCoord = inTexCoord;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
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
