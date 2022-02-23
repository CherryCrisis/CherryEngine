#pragma once

template <typename ... Args>
static Resource* Mesh::Create(const char* shapeName, EMeshShape shapeType, Args... args)
{
	switch (shapeType)
	{
	case EMeshShape::CUBE:
		return CreateCube(shapeName, args...);

	default:
		return nullptr;
	}
}