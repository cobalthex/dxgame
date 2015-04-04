float4x3 Skin(matrix Joints[128], float4 Weights, uint4 Indices)
{
	float4x3 result = Weights.x * (float4x3)Joints[Indices.x];
	result = result + Weights.y * (float4x3)Joints[Indices.y];
	result = result + Weights.z * (float4x3)Joints[Indices.z];
	result = result + Weights.w * (float4x3)Joints[Indices.w];
	return result;
}