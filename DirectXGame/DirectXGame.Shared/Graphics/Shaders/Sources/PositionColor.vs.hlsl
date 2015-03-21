//A very basic shader that renders triangles/lines with a color - Usually used for debugging

cbuffer WorldBuffer : register(b0)
{
	matrix WorldViewProjection;
}

struct VSOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VSOut main(float4 Position : POSITION, float4 Color : COLOR)
{
	VSOut v;
	v.position = mul(Position, WorldViewProjection);
	v.color = Color;
	return v;
}