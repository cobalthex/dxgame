#include "Pch.hpp"
#include "Model.hpp"
#include "Common/PlatformHelpers.hpp"

const unsigned Model::vertexStride = sizeof(VertexType);

Model::Model
(
	const DX::DeviceResourcesPtr& DeviceResources,
	PrimitiveTopology Topology,
	const std::vector<VertexType>& Vertices,
	const std::vector<unsigned>& Indices,
	const std::vector<Mesh>& Meshes,
	const std::vector<Joint>& Joints,
	const std::map<std::string, SkinnedSequence> Poses
	) :
	devContext(DeviceResources->GetD3DDeviceContext()),
	vertexCount(Vertices.size()),
	indexCount(Indices.size()),
	meshes(Meshes),
	joints(Joints),
	poses(Poses),
	topology(Topology),
	jointMats(),
	invJointMats()
{
	//create vertex buffer
	CD3D11_BUFFER_DESC vertexBufferDesc((unsigned)Vertices.size() * vertexStride, D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = Vertices.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertices));

	//create index buffer
	CD3D11_BUFFER_DESC indexBufferDesc((unsigned)Indices.size() * sizeof(unsigned), D3D11_BIND_INDEX_BUFFER);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = Indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &indices));

	auto nj = min(MAX_JOINTS, joints.size());
	jointMats.resize(nj);
	invJointMats.resize(nj);
}

bool Model::BindPose(const std::string& Pose, DirectXGame::ObjectConstantBufferDef& Buffer)
{
	if (poses.find(Pose) == poses.end())
		return false;

	//make sure enough space
	auto nj = min(MAX_JOINTS, joints.size());
	jointMats.resize(nj);
	invJointMats.resize(nj);
	
	auto& pose = poses.at(Pose).pose; //the current pose, transforms set by animation

	for (size_t i = 0; i < nj; i++)
	{
		jointMats[i] = Matrix::CreateFromQuaternion(joints[i].rotation);
		jointMats[i] *= Matrix::CreateScale(joints[i].scale);
		jointMats[i] *= Matrix::CreateTranslation(joints[i].translation);

		invJointMats[i] = jointMats[i].Invert();

		if (joints[i].parent >= 0)
		{
			jointMats[i] = jointMats[joints[i].parent] * jointMats[i];
			invJointMats[i] *= invJointMats[joints[i].parent];
		}
	}

	for (size_t i = 0; i < nj; i++)
	{
		Matrix matrix;
		matrix *= Matrix::CreateFromQuaternion(pose.rotations[i]);
		matrix *= Matrix::CreateScale(pose.scales[i]);
		matrix *= Matrix::CreateTranslation(pose.translations[i]);
		
		if (joints[i].parent >= 0)
			matrix = jointMats[i] * matrix * invJointMats[i];
		else
			matrix *= invJointMats[i];

		Buffer.joints[i] = matrix;
	}

	return true;
}

void Model::Bind(unsigned Slot) const
{
	if (vertexCount < 1 || indexCount < 1)
		return; //nothing to draw

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT offset = 0;
	UINT stride = vertexStride;
	devContext->IASetVertexBuffers(Slot, 1, vertices.GetAddressOf(), &stride, &offset);

	devContext->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);
}
void Model::Draw(unsigned Slot) const
{
	Bind(Slot);

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		m.material.texture->Apply();
		// Draw the objects.
		devContext->DrawIndexed((unsigned)m.IndexCount(), (unsigned)m.StartIndex(), 0);
	}
}