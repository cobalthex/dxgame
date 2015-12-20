#pragma once

#include "Pch.hpp"
#include "DeviceResources.hpp"
#include "Skinning.hpp"
#include "Math.hpp"

using namespace Math;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;
	XMFLOAT4 tangent;
	PackedVector::XMUBYTE4 blendIndices;
	PackedVector::XMUBYTEN4 blendWeights;
};

struct Material
{
	bool isLit;
};

struct ModelMesh
{
	std::string name;
	unsigned startVertex, vertexCount;
	unsigned startIndex, indexCount;

	Material material;
};

#define MAX_JOINTS 128

class Model
{
public:
	Model() = default;
	Model(const DeviceResourcesPtr& DevRes, std::istream& Input, ComPtr<ID3D12GraphicsCommandList>& CommandList);

	std::map<std::string, ModelMesh> meshes;

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	unsigned indexCount, vertexCount;

	std::vector<Joint> joints; //All of the joints connecting the meshes in this model
	std::map<std::string, SkinnedSequence> poses; //A collection of animated poses
	std::string pose; //the current pose (maps to poses)

	inline void Skin(Matrix* PoseArray, size_t MaxPoses = MAX_JOINTS) const { Skin(pose, PoseArray, MaxPoses); } //bind the current pose to a constant buffer
	void Skin(const std::string& Pose, Matrix* PoseArray, size_t MaxPoses = MAX_JOINTS) const; //Bind a pose to a constant buffer (uses bind pose if doesn't exist)

protected:

	DeviceResourcesPtr deviceResources;

private:
	void GpuCreate(const std::vector<Vertex>& Vertices, const std::vector<unsigned>& Indices, const ComPtr<ID3D12GraphicsCommandList>& CommandList);
	ComPtr<ID3D12Resource> indexBufferUpload;
	ComPtr<ID3D12Resource> vertexBufferUpload;
};