#pragma once

#include "resource.hpp"
#include "cherry_macros.hpp"

#include "maths/vector2.hpp"

struct Character {
	unsigned int	  TextureID; // ID handle of the glyph texture
	CCMaths::Vector2  Size;      // Size of glyph
	CCMaths::Vector2  Bearing;   // Offset from baseline to left/top of glyph
	unsigned int	  Advance;   // Horizontal offset to advance to next glyph
};

class CCENGINE_API Font : public Resource<Font>
{
public:
	Font(const char* filepath) : Resource(filepath) {}

	static void Load(std::shared_ptr<Font> font);

	std::map<char, Character> Characters;
	
	void Delete() override;
	void Reload() {}

};