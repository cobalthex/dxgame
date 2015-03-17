#pragma once

#include "Pch.hpp"
#include "Graphics/DeviceResources.hpp"

#define CBUFFER_ALIGN __declspec(align(16))

//The basis for a constant buffer definition that is submitted to the GPU Default aligned at 16 bytes
CBUFFER_ALIGN struct ConstantBufferDef { };

//A single constant buffer. It contains a pointer to the created constant buffer (Default slot is the default bind slot for Apply, can be overriden)
//Note: All constant buffers have dynamic usage and therefore require Map()
template <class BufferType>
class ConstantBuffer
{
public:
	ConstantBuffer() : constantBuffer(nullptr), devContext(nullptr), data() { }
	ConstantBuffer(const DX::DeviceResourcesPtr& DeviceResources, const BufferType& InitialData)
		: devContext(DeviceResources->GetD3DDeviceContext()), data(InitialData)
	{
		static_assert((sizeof(BufferType) % 16) == 0, "Constant Buffer must be 16-byte aligned");

		CD3D11_BUFFER_DESC constantBufferDesc (sizeof(BufferType), D3D11_BIND_CONSTANT_BUFFER);
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = &InitialData;
		sub.SysMemPitch = 0;
		sub.SysMemSlicePitch = 0;
		Sys::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, &sub, &constantBuffer));
	}
	ConstantBuffer(const DX::DeviceResourcesPtr& DeviceResources) : ConstantBuffer(DeviceResources, BufferType()) { }

	virtual ~ConstantBuffer() { }

	ComPtr<ID3D11Buffer> Buffer() const { return constantBuffer; } //get the underlying constant buffer pointer

	//update the constant buffer on the GPU with the values stored in data
	inline virtual void Update() const
	{
		auto cb = constantBuffer.Get();
		D3D11_MAPPED_SUBRESOURCE map;
		devContext->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		CopyMemory(map.pData, &data, sizeof(data));
		devContext->Unmap(cb, 0);
	}
	//Update the constant buffer on the gpu with values in Buffer
	inline virtual void Update(const BufferType& Buffer) const
	{
		auto cb = constantBuffer.Get();
		D3D11_MAPPED_SUBRESOURCE map;
		devContext->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		CopyMemory(map.pData, &Buffer, sizeof(Buffer));
		devContext->Unmap(cb, 0);
	}

	inline void BindVertex(unsigned Slot) const { devContext->VSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Bind this constant buffer to a vertex shader
	inline void BindPixel(unsigned Slot) const { devContext->PSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Bind this constant buffer to a pixel shader
	inline void BindDomain(unsigned Slot) const { devContext->DSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Bind this constant buffer to a domain shader
	inline void BindHull(unsigned Slot) const { devContext->HSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Bind this constant buffer to a hull shader
	inline void BindGeometry(unsigned Slot) const { devContext->GSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Bind this constant buffer to a geometry shader
	inline void BindCompute(unsigned Slot) const { devContext->CSSetConstantBuffers(Slot, 1, constantBuffer.GetAddressOf()); } //Bind this constant buffer to a compute shader

	CBUFFER_ALIGN BufferType data; //the data that the buffer should store

protected:
	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11DeviceContext> devContext;
};