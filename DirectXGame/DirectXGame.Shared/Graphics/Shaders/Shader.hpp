#pragma once

#include "Pch.hpp"
#include "Common/PlatformHelpers.hpp"
#include "Graphics/DeviceResources.hpp"

//Available shader types
enum class ShaderProgramType
{
	Unknown,
	Compute,
	Vertex,
	Hull,
	Domain,
	Geometry,
	Pixel
};

class ShaderProgramBase { protected: ShaderProgramBase() { } };

//A single shader program that is from a shader file
template <ShaderProgramType Ty>
class ShaderProgram : public ShaderProgramBase
{
public:
	ShaderProgram() { }
	ShaderProgram(const DeviceResourcesPtr& DeviceResources, const std::vector<uint8_t>& ShaderByteCode) //Create a shader, from bytecode
		: devContext(DeviceResources->GetD3DDeviceContext())
	{
		CreateD3dShader(DeviceResources, ShaderByteCode);
	}
	virtual ~ShaderProgram() { }

	inline ShaderProgramType Type() const { return Ty; }
	inline void Apply()
	{
		//Possibly still loading
		if (devContext == nullptr || shader == nullptr)
			return;

		switch (Ty)
		{
		case ShaderProgramType::Compute: devContext->CSSetShader((ID3D11ComputeShader*)shader.Get(), nullptr, 0); break;
		case ShaderProgramType::Vertex: devContext->VSSetShader((ID3D11VertexShader*)shader.Get(), nullptr, 0); break;
		case ShaderProgramType::Hull: devContext->HSSetShader((ID3D11HullShader*)shader.Get(), nullptr, 0); break;
		case ShaderProgramType::Domain: devContext->DSSetShader((ID3D11DomainShader*)shader.Get(), nullptr, 0); break;
		case ShaderProgramType::Geometry: devContext->GSSetShader((ID3D11GeometryShader*)shader.Get(), nullptr, 0); break;
		case ShaderProgramType::Pixel: devContext->PSSetShader((ID3D11PixelShader*)shader.Get(), nullptr, 0); break;
		}
	}

	//Is this shader valid?
	inline bool IsValid() const { return (devContext != nullptr && shader != nullptr); }

	inline ComPtr<ID3D11DeviceContext> DeviceContext() const { return devContext; }

protected:
	ComPtr<ID3D11DeviceContext> devContext;
	ComPtr<ID3D11DeviceChild> shader;

	//Create the D3D shader
	void CreateD3dShader(const DeviceResourcesPtr& DeviceResources, const std::vector<uint8_t>& ShaderByteCode)
	{
		if (DeviceResources == nullptr)
			return;

		if (devContext == nullptr)
			devContext = ComPtr<ID3D11DeviceContext>(DeviceResources->GetD3DDeviceContext());

		ComPtr<ID3D11Device> dev = ComPtr<ID3D11Device>(DeviceResources->GetD3DDevice());
		switch (Ty)
		{
		case ShaderProgramType::Compute:
		{
			ComPtr<ID3D11ComputeShader> sh;
			Sys::ThrowIfFailed(dev->CreateComputeShader(ShaderByteCode.data(), ShaderByteCode.size(), nullptr, sh.GetAddressOf()));
			shader = sh;
			break;
		}
		case ShaderProgramType::Vertex:
		{
			ComPtr<ID3D11VertexShader> sh;
			Sys::ThrowIfFailed(dev->CreateVertexShader(ShaderByteCode.data(), ShaderByteCode.size(), nullptr, sh.GetAddressOf()));
			shader = sh;
			break;
		}
		case ShaderProgramType::Hull:
		{
			ComPtr<ID3D11HullShader> sh;
			Sys::ThrowIfFailed(dev->CreateHullShader(ShaderByteCode.data(), ShaderByteCode.size(), nullptr, sh.GetAddressOf()));
			shader = sh;
			break;
		}
		case ShaderProgramType::Domain:
		{
			ComPtr<ID3D11DomainShader> sh;
			Sys::ThrowIfFailed(dev->CreateDomainShader(ShaderByteCode.data(), ShaderByteCode.size(), nullptr, sh.GetAddressOf()));
			shader = sh;
			break;
		}
		case ShaderProgramType::Geometry:
		{
			ComPtr<ID3D11GeometryShader> sh;
			Sys::ThrowIfFailed(dev->CreateGeometryShader(ShaderByteCode.data(), ShaderByteCode.size(), nullptr, sh.GetAddressOf()));
			shader = sh;
			break;
		}
		case ShaderProgramType::Pixel:
		{
			ComPtr<ID3D11PixelShader> sh;
			Sys::ThrowIfFailed(dev->CreatePixelShader(ShaderByteCode.data(), ShaderByteCode.size(), nullptr, sh.GetAddressOf()));
			shader = sh;
			break;
		}
		}
	}
};

typedef ShaderProgram<ShaderProgramType::Compute>		ComputeShader;
typedef ShaderProgram<ShaderProgramType::Vertex>		VertexShader;
typedef ShaderProgram<ShaderProgramType::Hull>			HullShader;
typedef ShaderProgram<ShaderProgramType::Domain>		DomainShader;
typedef ShaderProgram<ShaderProgramType::Geometry>		GeometryShader;
typedef ShaderProgram<ShaderProgramType::Pixel>			PixelShader;








enum class ShaderType
{
	Unknown,
	PositionColor,
	PositionTexture,
	Text,
	Lit,
	Unlit,
	LitSkinned,

	ShadowMap
};

//The abstract parent for composite shaders (can contain multiple shader bases, constant buffers, vertex types, etc)
class Shader
{
public:
	virtual void Apply() = 0; //Apply the shaders/etc in this implementation
	virtual void Update() = 0; //Update any buffers/etc in this implementation

	virtual inline ShaderType Type() const = 0;

	static Shader* ActiveShader; //the currently set shader. Useful for not rebinding the same shader
};