#pragma once

#ifdef CHERRYDLL_EXPORTS
#define CHERRY_API __declspec(dllexport)
#else
#define CHERRY_API __declspec(dllimport)
#endif

class CHERRY_API Behaviour
{

	
public:

	Behaviour() {}
	virtual ~Behaviour() {};
	virtual void Start() {};
	virtual void Update() {};

};