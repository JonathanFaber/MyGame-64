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
	float3 position : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
	float height : HEIGHT;
	float landTypeHeight : LANDTYPEHEIGHT;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float height : HEIGHT, float landTypeHeight : LANDTYPEHEIGHT)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);

	output.position = mul(inPos, World).xyz;

	output.normal = mul(normal, World);

	output.TexCoord = inTexCoord;

	output.height = height;

	output.landTypeHeight = landTypeHeight;

	return output;
}

[maxvertexcount(24)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream <VS_OUTPUT> triStream) {
	// default terrain
	for (int i = 0; i < 3; i++) {
		triStream.Append(input[i]);
	}
	triStream.RestartStrip();
	/*
	// grass
	VS_OUTPUT v[4];
	float3 pos[3];
	float3 norm[3];
	for (int i = 0; i < 3; i++) {
		pos[i] = input[i].position;
		norm[i] = input[i].normal;
	}

	if (sqrt(pos[0].x * pos[0].x + pos[0].y * pos[0].y + pos[0].z * pos[0].z) < 50.0f) {
		float3 fNormal = normalize(cross(pos[2] - pos[1], pos[1] - pos[0]));
		float3 center = (pos[0] + pos[1] + pos[2]) / 3.0f;

		v[0] = input[0];
		//v[1] = 

	}
	//*/
}

float4 PS(VS_OUTPUT input) : SV_TARGET
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