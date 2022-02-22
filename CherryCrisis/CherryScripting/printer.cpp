
#include "printer.hpp"

void Printer::Start() 
{
	Debug* debug = Debug::GetInstance();

	debug->Log("Printer Started !");
	//Engine::PlayFile("file1.wav");
}

void Printer::Update() 
{
	Debug* debug = Debug::GetInstance();

	if (InputManager::instance()->GetKey(Keycode::E)) 
		debug->Log("Printer is running ...");
}