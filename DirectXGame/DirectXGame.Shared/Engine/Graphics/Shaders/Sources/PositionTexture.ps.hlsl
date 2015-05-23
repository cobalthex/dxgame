//A very basic pixel shader that shades a position with a texture

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0); //should be of type clamp

float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_TARGET
{
	return Texture.Sample(Sampler, TexCoord);
}