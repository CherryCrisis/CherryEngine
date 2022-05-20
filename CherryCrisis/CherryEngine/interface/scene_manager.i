%{
	#include "singleton.hpp"
	#include "scene_manager.hpp"
%}

%include std_string.i

%nodefaultctor SceneManager;

class SceneManager
{
public:
	static bool LoadScene(const char* filepath);
};