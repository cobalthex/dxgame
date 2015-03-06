#include "Pch.hpp"
#include "Shader.hpp"

void Shader::Apply() const
{
	switch (type)
	{
	case ShaderType::Compute:
		devContext->CSSetShader((ID3D11ComputeShader*)shader.Get(), nullptr, 0);
		break;

	case ShaderType::Vertex:
		devContext->VSSetShader((ID3D11VertexShader*)shader.Get(), nullptr, 0);
		break;

	case ShaderType::Domain:
		devContext->DSSetShader((ID3D11DomainShader*)shader.Get(), nullptr, 0);
		break;

	case ShaderType::Hull:
		devContext->HSSetShader((ID3D11HullShader*)shader.Get(), nullptr, 0);
		break;

	case ShaderType::Geometry:
		devContext->GSSetShader((ID3D11GeometryShader*)shader.Get(), nullptr, 0);
		break;

	case ShaderType::Pixel:
		devContext->PSSetShader((ID3D11PixelShader*)shader.Get(), nullptr, 0);
		break;
	}
}