#include <pch.hpp>

#include "ui_text_renderpass.hpp"

#include "camera.hpp"
#include "framebuffer.hpp"
#include "ui_text.hpp"
#include "viewer.hpp"


TextRenderPass::TextRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/TextShader.vert", "Assets/Shaders/TextShader.frag")
{
	if (m_program)
	{
		m_callExecute = CCCallback::BindCallback(&TextRenderPass::Execute, this);
	
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

template <>
int TextRenderPass::Subscribe(UIText* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_texts.insert(toGenerate);

	return 1;
}

template <>
void TextRenderPass::Unsubscribe(UIText* toGenerate)
{
	m_texts.erase(toGenerate);
}
// Font rendering from LearnGL
// https://learnopengl.com/In-Practice/Text-Rendering
void TextRenderPass::Execute(Viewer* viewer, Framebuffer* framebuffer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer->width, framebuffer->height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 proj = CCMaths::Matrix4::Orthographic(0.0f, static_cast<const float>(framebuffer->width),
														   0.0f, static_cast<const float>(framebuffer->height), -1.f, 5.f);

	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "projection"), 1, GL_FALSE, proj.data);
	for (UIText* text : m_texts)
	{
		CCMaths::Vector3 pos = text->GetPos();	
		glUniform3f(glGetUniformLocation(m_program->m_shaderProgram, "textColor"), 1.f, 1.f, 1.f);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// iterate through all characters
		std::string::const_iterator c;
		std::string textValue = text->GetText();
		float size = text->GetSize() / 50.f;
 		for (c = textValue.begin(); c != textValue.end(); c++)
		{
			Character ch = text->m_font->Characters[*c];

			float xpos = pos.x + ch.Bearing.x * size;
			float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * size;

			float w = ch.Size.x * size;
			float h = ch.Size.y * size;
			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			glBindTextureUnit(0, ch.TextureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			pos.x += (ch.Advance >> 6) * size; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisable(GL_BLEND);
}
