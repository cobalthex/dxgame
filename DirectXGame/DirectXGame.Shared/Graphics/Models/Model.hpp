#pragma once

#include "Pch.hpp"
#include "Mesh.hpp"
#include "Bone.hpp"

//A basic model. Contains a group of meshes and a skeleton
class Model
{
public:
	std::vector<Mesh> meshes;
	std::vector<Bone> bones; //a collection of all of the bones in the mesh
	Bone* root; //the root bone (null if bones is empty)
};