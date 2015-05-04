//Provides structures and functions for implementing phong/blinn lighting
//parts borrowed from http://3dgep.com/texturing-lighting-directx-11/#Shaders

#include "Consts.hlsli"

cbuffer MaterialBuffer : register(b0)
{
	float4 Emissive;
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float SpecularPower;
	bool UseTexture;
};

//A single light
struct Light
{
	float4 position;
	float4 direction;
	float4 color;

	float spotAngle;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	uint type; //directional, point, or spot
	bool isEnabled;
};

cbuffer LightBuffer : register(b1)
{
	float4 EyePosition; //the position of the camera
	float4 GlobalAmbience; //global ambient light

	Light Lights[MAX_LIGHTS];
};


//Calculate Diffuse lighting
float4 CalcDiffuse(Light Li, float3 L, float3 N) //Light, light vector, surface normal
{
	return Li.color * max(0, dot(N, L));
}
//Calculate Phong specular lighting
float4 CalcSpecularPhong(Light Li, float3 V, float3 L, float3 N) //Light, viewer vector, light vector, surface normal
{
	float3 R = normalize(reflect(-L, N));
	return Li.color * pow(max(0, dot(R, V)), SpecularPower);
}
//Calculate Blinn-Phong (approximation) specular lighting
float4 CalcSpecularBlinnPhong(Light Li, float3 V, float3 L, float3 N) //Light, viewer vector, light vector, surface normal
{
	float3 H = normalize(L + V);
	return Li.color * pow(max(0, dot(N, H)), SpecularPower);
}

#define CalcSpecular(Li, V, L, N) CalcSpecularBlinnPhong(Li, V, L, N) //the current specular function

//Calcuate attenuation based on attenuation parameters
float CalcAttenuation(Light Li, float D) //Light, distance
{
	return 1.0f / (Li.constantAttenuation + Li.linearAttenuation * D + Li.quadraticAttenuation * D * D);
}

//The diffuse and specular calculations 
struct LightCalc
{
	float4 diffuse;
	float4 specular;
};

//Calculate diffuse and specular lighting for a point light. Uses Blinn Phong
LightCalc CalcPointLight(Light Li, float3 V, float4 P, float3 N) //Light, viewer vector, point in world space, normal
{
	LightCalc result;

	float3 L = (Li.position - P).xyz; //Light vector
	float distance = length(L);
	L /= distance;

	float attenuation = CalcAttenuation(Li, distance);

	result.diffuse = CalcDiffuse(Li, L, N) * attenuation;
	result.specular = CalcSpecular(Li, V, L, N) * attenuation;

	return result;
}
//Calculate diffuse and specular light for a directional (at infinite distance) light
LightCalc CalcDirectionalLight(Light Li, float3 V, float4 P, float3 N) //Light, viewer vector, shaded point, normal
{
	LightCalc result;

	float3 L = -Li.direction.xyz;

	result.diffuse = CalcDiffuse(Li, L, N);
	result.specular = CalcSpecular(Li, V, L, N);

	return result;
}
//calculate the intensity of a spotlight
float CalcSpotCone(Light Li, float3 L) //Light, light vector
{
	float minCos = cos(Li.spotAngle);
	float maxCos = (minCos + 1.0f) / 2.0f;
	float cosAngle = dot(Li.direction.xyz, -L);

	return smoothstep(minCos, maxCos, cosAngle);
}
//Calculate a conical spotlight that forms a cone outward from the origin of light
LightCalc CalcSpotLight(Light Li, float3 V, float4 P, float3 N) //Light, viewer vector, shaded point, normal
{
	LightCalc result;

	float3 L = (Li.position - P).xyz;
	float distance = length(L);
	L = L / distance;

	float attenuation = CalcAttenuation(Li, distance);
	float spotIntensity = CalcSpotCone(Li, L);

	result.diffuse = CalcDiffuse(Li, L, N) * attenuation * spotIntensity;
	result.specular = CalcSpecular(Li, V, L, N) * attenuation * spotIntensity;

	return result;
}
//The packaged lighting calculation
LightCalc ComputeLighting(float4 P, float3 N)
{
	float3 V = normalize(EyePosition - P).xyz;

	LightCalc totalResult = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	[unroll]
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		LightCalc result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }; //note: this can be modified to give a colored shadow effect

		if (!Lights[i].isEnabled)
			continue;

		switch (Lights[i].type)
		{
		case LIGHT_DIRECTIONAL:
			result = CalcDirectionalLight(Lights[i], V, P, N); break;

		case LIGHT_POINT:
			result = CalcPointLight(Lights[i], V, P, N); break;

		case LIGHT_SPOT:
			result = CalcSpotLight(Lights[i], V, P, N); break;
		}

		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}

	totalResult.diffuse = saturate(totalResult.diffuse);
	totalResult.specular = saturate(totalResult.specular);

	return totalResult;
}