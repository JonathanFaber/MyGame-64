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
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 viewDir : VIEWDIR;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);

	output.normal = mul(normal, World);

	output.tangent = mul(tangent, World);

	output.bitangent = mul(bitangent, World);

	output.TexCoord = inTexCoord;

	output.viewDir = normalize(-mul(inPos, World).xyz);

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//input.normal = normalize(input.normal);

	float4 diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);
	float4 normalMap = ObjTexture[1].Sample(ObjSamplerState, input.TexCoord);
	float4 specularMap = ObjTexture[2].Sample(ObjSamplerState, input.TexCoord);

	// make range [-1,1]
	normalMap = normalMap * 2.0f - 1.0f;

	// Create the Texture Space
	float3x3 texSpace = float3x3(input.tangent, input.bitangent, input.normal);

	// Convert normal from normal map to texture space and store in input.normal
	input.normal = normalize(mul(normalMap, texSpace));

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);

	// specular
	float specularPower = 32.0f;

	float3 reflectDir = reflect(-light.dir, input.normal);
	float4 specular = pow(saturate(dot(reflectDir, input.viewDir)), specularPower) * specularMap;

	finalColor = saturate(finalColor + specular.rgb);

	return float4(finalColor, diffuse.a);
}