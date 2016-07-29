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

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float2 instancePos : INSTANCEPOS, float4 instanceHeights : INSTANCEHEIGHTS, uint indexID : SV_VertexID)
{
	VS_OUTPUT output;

	float firstPosX = inPos.x;
	float firstPosY = inPos.y;
	float firstPosZ = inPos.z;

	/*
	if (indexID == 0 || indexID == 4 || indexID == 6)
	inPos.y += (instanceHeights.x+instanceHeights.w)/2.0f;
	else if (indexID == 1 || indexID == 5 || indexID == 7)
	inPos.y += (instanceHeights.y+instanceHeights.z)/2.0f;
	else if (indexID == 2 || indexID == 8 || indexID == 9)
	inPos.y += (instanceHeights.x+instanceHeights.y)/2.0f;
	else if (indexID == 3 || indexID == 10 || indexID == 11)
	inPos.y += (instanceHeights.w+instanceHeights.z)/2.0f;
	else if (indexID == 0+12 || indexID == 4+12 || indexID == 6+12 || indexID == 2+12 || indexID == 8+12 || indexID == 9+12)
	inPos.y += instanceHeights.x;
	else if (indexID == 1+12 || indexID == 5+12 || indexID == 7+12)
	inPos.y += instanceHeights.y;
	else if (indexID == 3+12 || indexID == 10+12 || indexID == 11+12)
	inPos.y += instanceHeights.w;
	*/
	inPos.y += (instanceHeights.x + instanceHeights.y + instanceHeights.z + instanceHeights.w) / 4.0f;

	float windSpeed = 0.5f;
	float windIntensity = 0.5f;
	float2 windDirection;
	windDirection.x = cos(windIntensity*timeElaps);
	windDirection.y = sin(windIntensity*timeElaps);
	//windDirection = normalize(windDirection);

	float3 instanceRot = float3(cos(windDirection.x*windSpeed + 1.57f), abs(sin(windDirection.x*windSpeed + 1.57f)) * abs(sin(windDirection.y*windSpeed + 1.57f)) - 1.0f, cos(windDirection.y*windSpeed + 1.57f));


	inPos.x += instanceRot.x*firstPosY;
	inPos.y += instanceRot.y*firstPosY;
	inPos.z += instanceRot.z*firstPosY;

	inPos.x += instancePos.x;
	inPos.z += instancePos.y;

	output.Pos = mul(inPos, WVP);

	output.normal = mul(normal, World);

	output.TexCoord = inTexCoord;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	clamp(input.TexCoord.y, 0.0f, 0.95f);

	float4 diffuse;
	diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);

	float3 finalColor;

	finalColor = diffuse * light.ambient;

	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);
}