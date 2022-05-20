%{
	#include "singleton.hpp"
	#include "scene_manager.hpp"
%}

%include std_string.i

%nodefaultctor SceneManager;

class SceneManager
{
public:
	static bool ChangeScene(const char* filepath);
};