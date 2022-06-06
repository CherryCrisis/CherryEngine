#pragma once

#include <memory>

#include "resource.hpp"

class Shader;


class ShaderProgram : public Resource<ShaderProgram>
{
private:
	void InitializeProgram();
	void CreateProgram();

public:
	std::vector<std::shared_ptr<Shader>> m_shaders;
	unsigned int m_shaderProgram = 0u;

	ShaderProgram(const char* programName) : Resource(programName) {}
	~ShaderProgram() = default;

	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, const char* vxFilepath, const char* fgFilepath, const char* gmFilepath);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg);
	static void Load(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Shader>& vx, std::shared_ptr<Shader>& fg, std::shared_ptr<Shader>& gm);

	void Delete() override;
	void Reload();
	void OnReload(Shader* shader);
	void Reload(std::shared_ptr<Shader> shader);
	void Invalidate();
};
