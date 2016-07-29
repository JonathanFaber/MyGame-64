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

struct VS_OUTPUT
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

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

float4 diffuse;


diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);
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