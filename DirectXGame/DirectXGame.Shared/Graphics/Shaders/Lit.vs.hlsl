cbuffer SceneConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

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
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 position = float4(input.position, 1.0f);

	// Transform the vertex position into projected space.
	position = mul(position, model);
	position = mul(position, view);
	position = mul(position, projection);
	output.position = position;

	// Pass the color through without modification.
	output.color = input.color;
	output.texCoord = input.texCoord;
	output.normal = input.normal;

	return output;
}