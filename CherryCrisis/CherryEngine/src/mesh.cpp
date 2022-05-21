#include "pch.hpp"

#include "mesh.hpp"

#include <assimp/mesh.h>

void Mesh::Delete()
{
    m_gpuMesh.reset();
}

void Mesh::Load(std::shared_ptr<Mesh> mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const AABB& aabb)
{
    mesh->m_aabb = aabb;
    mesh->m_vertices = std::move(vertices);
    mesh->m_indices = std::move(indices);
}

void Mesh::Load(std::shared_ptr<Mesh> mesh, EMeshShape shapeType, float xHalfRes, float yHalfRes, float zHalfRes)
{
    switch (shapeType)
    {
    case EMeshShape::QUAD:
        CreateQuad(mesh, xHalfRes, yHalfRes);
        break;

    case EMeshShape::CUBE:
        CreateCube(mesh, xHalfRes, yHalfRes, zHalfRes);
        break;

    case EMeshShape::SPHERE:
        CreateSphere(mesh, xHalfRes, yHalfRes, zHalfRes);
        break;

    case EMeshShape::CYLINDER:
        CreateCylinder(mesh, xHalfRes, yHalfRes, zHalfRes);
        break;

    default:
        return;
    }
}

void Mesh::CreateCylinder(std::shared_ptr<Mesh> mesh, float radius, float halfHeight, float longitude)
{
    std::vector<Vertex> vertices;
    vertices.reserve((size_t)longitude * 2 + 2);

    float const S = 1.f / (longitude - 1.f);
    float add = 2 * PI / longitude;

    Vector3 min, max = { 0 };
    min.y = -halfHeight;
    max.y = halfHeight;

    for (int s = 0; s < longitude; s++)
    {
        Vertex vertex;

        float x = cosf(add * s);
        float z = sinf(add * s);

        vertex.uv = { s * S , 0 };

        Vector3 temp = CCMaths::Vector3(x, 0, z).Normalized();
        vertex.position = temp * radius;

        CCMaths::Min(min.x, vertex.position.x);
        CCMaths::Min(min.z, vertex.position.z);
        CCMaths::Max(max.x, vertex.position.x);
        CCMaths::Max(max.z, vertex.position.z);

        vertex.normal = vertex.tangent = vertex.bitangent = temp;

        vertex.position.y = halfHeight;
        vertex.normal.y = 1;
        vertex.normal.Normalize();
        vertices.push_back(vertex);

        vertex.position.y = -halfHeight;
        vertex.uv.y = 1;

        vertex.normal = temp;
        vertex.normal.y = -1;
        vertex.normal.Normalize();
        vertices.push_back(vertex);
    }

    Vertex vertex;
    vertex.uv = { 0.5f, 0.5f };
    vertex.position = { 0.f, 0.f, 0.f };

    vertex.normal = vertex.tangent = vertex.bitangent = CCMaths::Vector3(0, 1, 0);
    vertex.position.y = halfHeight;
    vertices.push_back(vertex);

    vertex.normal = vertex.tangent = vertex.bitangent = CCMaths::Vector3(0, -1, 0);
    CCMaths::Vector3(0, -1, 0);
    vertex.bitangent = CCMaths::Vector3(0, -1, 0);
    vertex.position.y = -halfHeight;
    vertices.push_back(vertex);

    std::vector<unsigned int> indices;
    indices.reserve((size_t)longitude * 12);

    for (int s = 0; s < longitude; s++)
    {
        indices.push_back(2 * s);
        indices.push_back((2 * s + 2) % ((int)longitude * 2));
        indices.push_back(2 * s + 1);

        indices.push_back((2 * s + 3) % ((int)longitude * 2));
        indices.push_back(2 * s + 1);
        indices.push_back((2 * s + 2) % ((int)longitude * 2));

        indices.push_back(vertices.size() - 2);
        indices.push_back((2 * s + 2) % ((int)longitude * 2));
        indices.push_back(2 * s);

        indices.push_back(vertices.size() - 1);
        indices.push_back(2 * s + 1);
        indices.push_back((2 * s + 3) % ((int)longitude * 2));
    }

    Load(mesh, vertices, indices);
}

void Mesh::CreateSphere(std::shared_ptr<Mesh> mesh, float radius, float latitude, float longitude)
{
    std::vector<Vertex> vertices;

    unsigned int uint_lon = static_cast<unsigned int>(longitude);
    unsigned int uint_lat = static_cast<unsigned int>(latitude);
    vertices.reserve((size_t)uint_lat * (size_t)uint_lon);

	float const R = 1.f / (latitude - 1.f);
	float const S = 1.f / (longitude - 1.f);
    
    Vector3 min, max = { 0 };

    for (unsigned int r = 0; r < uint_lat; r++)
    {
        for (unsigned int s = 0; s < uint_lon; s++)
        {
            Vertex vertex;

            float const y = CCMaths::Sin(-(CCMaths::PI * 0.5f) + CCMaths::PI * r * R);
            float const x = CCMaths::Cos(2 * CCMaths::PI * s * S) * sin(PI * r * R);
            float const z = CCMaths::Sin(2 * CCMaths::PI * s * S) * sin(PI * r * R);

            vertex.uv.x = s * S;
            vertex.uv.y = r * R;

            vertex.position = { x, y, z };
            vertex.position = vertex.position.Normalized() * radius;

            CCMaths::Min(min.x, vertex.position.x);
            CCMaths::Min(min.y, vertex.position.y);
            CCMaths::Min(min.z, vertex.position.z);
            CCMaths::Max(max.x, vertex.position.x);
            CCMaths::Max(max.y, vertex.position.y);
            CCMaths::Max(max.z, vertex.position.z);

            vertex.normal = vertex.tangent = vertex.bitangent = CCMaths::Vector3(x, y, z);

            vertices.push_back(vertex);
        }
    }

    std::vector<unsigned int> indices;
    indices.reserve((size_t)uint_lat * (size_t)uint_lon * 6);

    for (unsigned int r = 0; r < uint_lat; r++)
    {
        for (unsigned int s = 0; s < uint_lon; s++)
        {
            indices.push_back(r * uint_lon + s);
            indices.push_back((r + 1) * uint_lon + s);
            indices.push_back(r * uint_lon + (s + 1));

            indices.push_back((r + 1) * uint_lon + (s + 1));
            indices.push_back(r * uint_lon + (s + 1));
            indices.push_back((r + 1) * uint_lon + s);
	    }
    }

    Load(mesh, vertices, indices);
}

void Mesh::CreateCube(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes, float zHalfRes)
{
    std::vector<Vertex> vertices;
    vertices.reserve(8);

    for (int i = 0; i < 8; i++)
    {
        int xSign = BoolSign(!BoolPattern(i, 2));
        int ySign = BoolSign(!BoolPattern(i, 4));
        int zSign = BoolSign(!BoolPattern(i, 1));

        Vertex vertex;

        vertex.position.x = xSign * xHalfRes;
        vertex.position.y = ySign * yHalfRes;
        vertex.position.z = zSign * zHalfRes;

        vertex.normal = CCMaths::Vector3((float)xSign, (float)ySign, (float)zSign);
        vertex.tangent = CCMaths::Vector3((float)xSign, (float)ySign, (float)zSign);
        vertex.bitangent = CCMaths::Vector3((float)xSign, (float)ySign, (float)zSign);

        vertices.push_back(vertex);
    }

    std::vector<unsigned int> indices;
    indices.reserve(36);
    
    //Above ABC,BCD
    indices.push_back(0); indices.push_back(2); indices.push_back(1);
    indices.push_back(3); indices.push_back(1); indices.push_back(2);

    //Following EFG,FGH
    indices.push_back(6); indices.push_back(4); indices.push_back(5);
    indices.push_back(5); indices.push_back(7); indices.push_back(6);

    //Left ABF,AEF
    indices.push_back(5); indices.push_back(0); indices.push_back(1);
    indices.push_back(0); indices.push_back(5); indices.push_back(4);

    //Right side CDH,CGH
    indices.push_back(2); indices.push_back(7); indices.push_back(3);
    indices.push_back(7); indices.push_back(2); indices.push_back(6);

    //ACG,AEG
    indices.push_back(0); indices.push_back(6); indices.push_back(2);
    indices.push_back(6); indices.push_back(0); indices.push_back(4);

    //Behind BFH,BDH
    indices.push_back(1); indices.push_back(7); indices.push_back(5);
    indices.push_back(7); indices.push_back(1); indices.push_back(3);
    
    Load(mesh, vertices, indices);
}

void Mesh::CreateQuad(std::shared_ptr<Mesh> mesh, float xHalfRes, float yHalfRes)
{
    std::vector<Vertex> vertices;
    vertices.reserve(4);

    Vector3 min, max = 0;
    min.z = -0.1f;
    max.z = 0.1f;

    for (int i = 0; i < 4; i++)
    {
        int xPattern = !BoolPattern(i + 1, 2);
        int yPattern = !BoolPattern(i, 2);

        Vertex vertex;
        vertex.uv.x = (float)xPattern;
        vertex.uv.y = (float)yPattern;

        int xSign = BoolSign(xPattern);
        int ySign = BoolSign(yPattern);

        vertex.position.x = xSign * xHalfRes;
        vertex.position.y = ySign * yHalfRes;

        min.x = CCMaths::Min(min.x, vertex.position.x);
        min.y = CCMaths::Min(min.y, vertex.position.y);

        max.x = CCMaths::Max(max.x, vertex.position.x);
        max.y = CCMaths::Max(max.y, vertex.position.y);

        vertices.push_back(vertex);
    }

    std::vector<unsigned int> indices;
    indices.reserve(6);

    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(2); indices.push_back(3); indices.push_back(0);

    Load(mesh, vertices, indices, { min, max });
}

void Mesh::ClearData()
{
    m_vertices.clear();
    m_indices.clear();
}


void Mesh::Reload(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const AABB& aabb)
{
    ClearData();

    m_aabb = aabb;
    m_vertices = std::move(vertices);
    m_indices = std::move(indices);
}
