#include "Consts.hlsli"

Texture2D DepthTexture : register(t0);
SamplerState Sampler : register(s0); //should be of type clamp

cbuffer LightColorBuffer : register(b1)
{
	float4 LightColor;
}

struct PixelInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float4 lightViewPos : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

float4 main(PixelInput Input) : SV_TARGET
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;


	// Set the bias value for fixing the floating point precision issues.
	bias = 0.001f;
	//Now in the pixel shader we set the default color to be black(shadowed).
	
	// Set the default output color to be black (shadow).
	color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Calculate the projected texture coordinates.
	projectTexCoord.x = Input.lightViewPos.xx / Input.lightViewPos.w / 2.0f + 0.5f;
	projectTexCoord.y = -Input.lightViewPos.y / Input.lightViewPos.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = DepthTexture.Sample(Sampler, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = Input.lightViewPos.z / Input.lightViewPos.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if (lightDepthValue < depthValue)
		{
			// Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(Input.normal, Input.lightPos));

			// If this pixel is illuminated then set it to pure white (non-shadow).
			if (lightIntensity > 0.0f)
			{
				//And if we determine a pixel is illuminated then it is colored pure white.
					 
				color += (LightColor * lightIntensity);
			}
		}
	}
	return color;
}
