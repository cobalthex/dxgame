cbuffer WorldBuffer : register(b0)
{
	matrix World;
	matrix InverseTransposeWorld;
	matrix WorldViewProjection;
	float4x3 joints[64];
}

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
	uint4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHT;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 position : SV_POSITION; //position in screen coordinates
	float4 worldPosition : POSITION; //position in world coordinates
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

float4x3 Skin(float4 Weights, uint4 Indices)
{
	float4x3 result = Weights.x * joints[Indices.x];
	result = result + Weights.y * joints[Indices.y];
	result = result + Weights.z * joints[Indices.z];
	result = result + Weights.w * joints[Indices.w];
	return result;
}

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 position = float4(input.position, 1.0f);
	float4x3 skin = Skin(input.blendWeights, input.blendIndices);

	position.xyz = mul(position, skin);
	input.normal.xyz = mul(input.normal, skin);


	// Transform the vertex position into projected space.

	output.worldPosition = mul(position, World);
	output.position = mul(position, WorldViewProjection);

	// Pass the color through without modification.
	output.color = input.color;
	output.texCoord = input.texCoord;
	output.normal = mul(input.normal, (float3x3)InverseTransposeWorld);

	return output;
}