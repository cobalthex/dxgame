#pragma once

//A single bone that represents a bone of a skeleton of a mesh
class Bone
{
public:

protected:
	size_t index; //the index of this bone in the model's collection
	Bone* parent; //The parent bone. Null if root

	//transform
};