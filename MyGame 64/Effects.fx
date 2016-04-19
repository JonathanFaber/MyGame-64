
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
	float biomeHeight : BIOMEHEIGHT;
	float heat : HEAT;
	float moisture : MOISTURE;
	float3 position : PIZZA; //because why not
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

PLANET_VS_OUTPUT PLANET_VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float inBiomeHeight : BIOMEHEIGHT, float inHeat : HEAT, float inMoisture : MOISTURE)
{
	PLANET_VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);

	output.normal = mul(normal, World);

	output.TexCoord = inTexCoord;

	output.biomeHeight = inBiomeHeight;

	output.heat = inHeat;

	output.moisture = inMoisture;

	output.position.x = inPos.x;
	output.position.y = inPos.y;
	output.position.z = inPos.z;
	

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

	
	if (diffuse.a < 0.5f)
		discard;
	//else if (diffuse.r + diffuse.g + diffuse.b < 0.15f && diffuse.r == diffuse.g && diffuse.g == diffuse.b)
	//else if (diffuse.r + diffuse.g + diffuse.b < 0.2f)
	//	discard;
	
	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);
	//finalColor += saturate(light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);
}

float4 PLANET_PS(PLANET_VS_OUTPUT input) : SV_TARGET
{
	//if (input.draw == false)
	//	discard;

	input.normal = normalize(input.normal);

	float4 diffuse;
	/*
	if (input.biomeHeight > 0.33f && input.biomeHeight < 0.35f)
		diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (input.biomeHeight > 0.31f && input.biomeHeight < 0.33f)
		diffuse = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (input.moisture > 0.32f && input.moisture < 0.34f)
		diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	else if (input.moisture < -0.32f && input.moisture > -0.34f)
		diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	else if (input.heat > 0.33f)
		diffuse = float4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (input.heat > -0.33f)
		diffuse = float4(1.0f, 1.0f, 0.0f, 1.0f);
	else if (input.heat < -0.33f)
		diffuse = float4(1.0f, 0.0f, 1.0f, 1.0f);
	else
		diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	*/
	if (input.heat < -0.25f && input.moisture >= -0.33f && input.biomeHeight >= 0.25f)		// Snowy forest
		diffuse = float4(0.7f, 0.85f, 0.7f, 1.0f);
	else if (input.heat < -0.33f)															// Snowy plains
		diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	else if (input.heat >= 0.33f && input.moisture < -0.33f && input.biomeHeight >= 0.25f)	// Desert
		diffuse = float4(1.0f, 1.0f, 0.0f, 1.0f);
	else if (input.heat >= 0.33f && input.moisture < 0.33f && input.biomeHeight >= 0.25f)	// Savanah
		diffuse = float4(0.6f, 1.0f, 0.0f, 1.0f);
	else if (input.heat >= 0.33f && input.moisture >= 0.33f && input.biomeHeight >= 0.25f)	// Rain forest
		diffuse = float4(0.0f, 0.3f, 0.0f, 1.0f);
	else if (input.heat >= 0.0f && input.moisture >= 0.33f && input.biomeHeight >= 0.25f)	// Swamp
		diffuse = float4(0.0f, 0.4f, 0.0f, 1.0f);
	else if (input.heat >= 0.0f && input.moisture >= -0.2f && input.biomeHeight >= 0.25f)	// Forest
		diffuse = float4(0.0f, 0.5f, 0.3f, 1.0f);
	else if (input.heat < 0.0f && input.moisture >= -0.2f && input.biomeHeight >= 0.25f)	// Canadian forest
		diffuse = float4(0.0f, 0.5f, 0.4f, 1.0f);
	else if (input.biomeHeight >= 0.25f)													// Plains of grass
		diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (input.biomeHeight >= 0.2f)														// Shallow water
		diffuse = float4(0.0f, 1.0f, 1.0f, 1.0f);
	else if (input.biomeHeight < 0.2f)														// Ocean and lakes
		diffuse = float4(0.0f, 0.0f, 1.0f, 1.0f);

	/*
	else if (input.biomeHeight >= 0.33f)
		diffuse = ObjTexture[0].Sample( ObjSamplerState, input.TexCoord );
	else
		diffuse = ObjTexture[3].Sample( ObjSamplerState, input.TexCoord );
	*/

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	//finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);
	finalColor += saturate(1.0f * light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample(ObjSamplerState, input.texCoord);
}

float4 SPRITE_PS(SPRITE_VS_OUTPUT input) : SV_TARGET
{
	float4 diffuse;
	
	diffuse = ObjTexture[1].Sample( ObjSamplerState, input.TexCoord );
	
	//if (diffuse.a <= 0.15f)
	//	discard;
	//else
	//	diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);

	float3 finalColor;

	finalColor = diffuse;

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
