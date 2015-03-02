#include "Pch.hpp"
#include "Model.hpp"
#include "Common/PlatformHelpers.hpp"

Model::Model
(
	const DX::DeviceResourcesPtr& DeviceResources,
	const std::vector<VertexType>& Vertices,
	const std::vector<IndexType>& Indices,
	PrimitiveTopology Topology,
	const std::vector<ModelMesh>& Meshes,
	const std::vector<Joint>& Joints,
	const std::map<std::string, SkinnedSequence> Poses
	) :
	deviceResources(DeviceResources),
	devContext(DeviceResources->GetD3DDeviceContext()),
	meshes(Meshes),
	joints(Joints),
	poses(Poses)
{
	CreateFrom(DeviceResources, Vertices, Indices, Topology);
}

void Model::Draw(unsigned Slot) const
{
	Bind(Slot);

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		m.material.texture->Apply();
		//Draw the objects.
		devContext->DrawIndexed((unsigned)m.IndexCount(), (unsigned)m.StartIndex(), 0);
	}
}

void Model::Skin(const std::string& Pose, Matrix* PoseArray, size_t MaxPoses) const
{
	auto nj = min(MaxPoses, joints.size());
	std::vector<Matrix> poseMats(nj);

	bool hasPose = (poses.find(Pose) != poses.end());

	for (size_t i = 0; i < nj; i++)
	{
		auto parent = joints[i].parent;
		
		Matrix local;
		if (hasPose)
		{
			auto p = &poses.at(Pose).pose; //the current pose, transforms set by animation

			local *= Matrix::CreateScale(p->scales[i]);
			local *= Matrix::CreateFromQuaternion(p->rotations[i]);
			local *= Matrix::CreateTranslation(p->translations[i]);
		}
		else
			local = joints[i].transform;

		if (parent >= 0)
			poseMats[i] = local * poseMats[parent];
		else
			poseMats[i] = local;
	
		PoseArray[i] = (joints[i].inverseTransform * poseMats[i]).Transpose();
	}
}

BasicMesh<VertexTypes::VertexPositionColor, unsigned> Model::CreateSkeletalMesh(const std::string& Pose, const Color& VertexColor) const
{
	std::vector<VertexTypes::VertexPositionColor> vertices;
	std::vector<unsigned> indices;
	vertices.reserve(joints.size() * 3); //octahedrons (6 vertices)
	indices.reserve(joints.size() * 3); //8 faces

	VertexTypes::VertexPositionColor v;
	v.color = VertexColor.ToVector3();

	float radius = 0.05f;

	bool hasPose = (poses.find(Pose) != poses.end());

	unsigned i = 0;
	for (auto& j : joints)
	{
		if (j.parent < 0)
			continue;

		Matrix local, parent;
		if (hasPose)
		{
			auto p = &poses.at(Pose).pose; //the current pose, transforms set by animation

			local *= Matrix::CreateScale(p->scales[j.index]);
			local *= Matrix::CreateFromQuaternion(p->rotations[j.index]);
			local *= Matrix::CreateTranslation(p->translations[j.index]);

			parent *= Matrix::CreateScale(p->scales[j.parent]);
			parent *= Matrix::CreateFromQuaternion(p->rotations[j.parent]);
			parent *= Matrix::CreateTranslation(p->translations[j.parent]);
		}
		else
		{
			local = j.transform;
			parent = joints[j.parent].transform;
		}

		//parent joints
		v.position = Vector3::Transform(Vector3(-radius, 0, 0), parent);
		vertices.push_back(v);

		v.position = Vector3::Transform(Vector3(radius, 0, 0), parent);
		vertices.push_back(v);

		//current joint
		v.position = Vector3::Transform(Vector3::Zero, local);
		vertices.push_back(v);

		unsigned tris[] = { i, i + 1, i + 2, i + 2, i, i + 1 };
		indices.insert(indices.end(), tris, tris + ARRAYSIZE(tris));
		i += 3;
	}

	return BasicMesh<VertexTypes::VertexPositionColor, unsigned>(vertices, indices, PrimitiveTopology::List);
}