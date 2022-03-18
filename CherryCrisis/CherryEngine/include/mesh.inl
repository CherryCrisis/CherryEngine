#pragma once

template <typename ... Args>
static void Mesh::Load(std::shared_ptr<Mesh> mesh, const char* shapeName, EMeshShape shapeType, Args... args)
{
	switch (shapeType)
	{
	case EMeshShape::CUBE:
		CreateCube(mesh, shapeName, args...);

	default:
		return;
	}
}