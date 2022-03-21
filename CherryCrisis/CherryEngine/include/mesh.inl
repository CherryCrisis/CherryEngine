#pragma once

template <typename ... Args>
static void Mesh::Load(std::shared_ptr<Mesh> mesh, EMeshShape shapeType, Args... args)
{
	switch (shapeType)
	{
	case EMeshShape::CUBE:
		CreateCube(mesh, args...);

	default:
		return;
	}
}