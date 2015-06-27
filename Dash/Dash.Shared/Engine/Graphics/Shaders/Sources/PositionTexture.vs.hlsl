//A very basic shader that renders triangles/lines with a texture

cbuffer WorldBuffer : register(b0)
{
	matrix WorldViewProjection;
}

struct VSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

VSOut main(float4 Position : POSITION, float2 TexCoord : TEXCOORD)
{
	VSOut v;
	v.position = mul(Position, WorldViewProjection);
	v.texCoord = TexCoord;
	return v;
}