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

INSTANCE_VS_OUTPUT INSTANCE_VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float2 instancePos : INSTANCEPOS, float4 instanceHeights : INSTANCEHEIGHTS, float3 instanceRot : INSTANCEROT, uint indexID : SV_VertexID)
{
	INSTANCE_VS_OUTPUT output;

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


	float2 windDirection = float2(0.7071f, 0.7071f);
	float windSpeed = 0.0f;
	float windIntensity = 0.125f;

	instanceRot = float3(cos(windDirection.x*windSpeed + 1.57f), abs(sin(windDirection.x*windSpeed + 1.57f)) * abs(sin(windDirection.y*windSpeed + 1.57f)) - 1.0f, cos(windDirection.y*windSpeed + 1.57f));

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
	else if (input.height <= 0.0001f)				// sand
		diffuse = ObjTexture[2].Sample(ObjSamplerState, input.TexCoord);
	else if (input.landTypeHeight > 0.5f)			// mountains
		diffuse = ObjTexture[1].Sample(ObjSamplerState, input.TexCoord);
	else if (input.landTypeHeight > 0.0f)			// plains
		diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);
	else											// hills
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

	float4 diffuse;
	/*
	if (input.texID == 0)
	diffuse = ObjTexture[0].Sample( ObjSamplerState, input.TexCoord );
	else if (input.texID == 1)
	diffuse = ObjTexture[1].Sample( ObjSamplerState, input.TexCoord );
	else if (input.texID == 2)
	diffuse = ObjTexture[2].Sample( ObjSamplerState, input.TexCoord );
	else if (input.texID == 3)
	diffuse = ObjTexture[3].Sample( ObjSamplerState, input.TexCoord );
	else if (input.texID == 4)
	*/

	diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);

	float3 finalColor;

	finalColor = diffuse * light.ambient;

	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);
}
