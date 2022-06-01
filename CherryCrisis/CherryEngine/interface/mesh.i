%{
	#include "mesh.hpp"
%}

%include vector3.i

%nodefaultctor Mesh;

%include <std_shared_ptr.i>
%shared_ptr(Mesh)

class Mesh
{
public:
	static void CreateCylinder(std::shared_ptr<Mesh> mesh, float radius, float halfHeight, float longitude);
	static void CreateSphere(std::shared_ptr<Mesh> mesh, float radius, float latitude, float longitude);
	static void CreateCube(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes, float zHalfRes);
	static void CreateQuad(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes);
};