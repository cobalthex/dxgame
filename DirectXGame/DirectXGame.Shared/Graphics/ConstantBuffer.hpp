#pragma once

#include "Pch.hpp"
#include "Common/DeviceResources.hpp"

#define CBUFFER_ALIGN __declspec(align(16))

//The basis for a constant buffer definition that is submitted to the GPU. Default aligned at 16 bytes
CBUFFER_ALIGN struct ConstantBufferDef { };

//A single constant buffer. It contains a pointer to the created constant buffer
template <class BufferType>
class ConstantBuffer
{
public:
	ConstantBuffer() : constantBuffer(nullptr), devContext(nullptr), data() { }
	ConstantBuffer(const DX::DeviceResourcesPtr& DeviceResources, const BufferType& InitialData)
		: devContext(DeviceResources->GetD3DDeviceContext()), data(InitialData)
	{
		CD3D11_BUFFER_DESC constantBufferDesc (sizeof(BufferType), D3D11_BIND_CONSTANT_BUFFER);
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = &InitialData;
		sub.SysMemPitch = 0;
		sub.SysMemSlicePitch = 0;
		DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, &sub, &constantBuffer));
	}
	ConstantBuffer(const DX::DeviceResourcesPtr& DeviceResources) : ConstantBuffer(DeviceResources, BufferType()) { }

	virtual ~ConstantBuffer() { }

	ComPtr<ID3D11Buffer> Buffer() const { return constantBuffer; } //get the underlying constant buffer pointer

	inline void Update() const { devContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, &data, 0, 0); } //update the constant buffer on the GPU with the values stored in data
	inline void ApplyVS(unsigned Slot = 0) const { devContext->VSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Apply this constant buffer to a vertex shader
	inline void ApplyPS(unsigned Slot = 0) const { devContext->PSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Apply this constant buffer to a pixel shader

	CBUFFER_ALIGN BufferType data; //the data that the buffer should store

protected:
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11DeviceContext> devContext;
};