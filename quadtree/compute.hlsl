struct InputData
{
	float3 position;
	float3 velocity;
	float3 initialVelocity;
};

struct OutputData
{
	float3 position;
	float3 velocity;
};

StructuredBuffer<InputData>  inputData   : register(t0);
RWStructuredBuffer<OutputData>        outputData          : register(u0);


[numthreads(1024, 1, 1)]
void CS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	outputData[dispatchThreadID.x].position = inputData[dispatchThreadID.x].position + inputData[dispatchThreadID.x].velocity;
}