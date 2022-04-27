#pragma once

#include "viewer.hpp"
#include "cell.hpp"

#include "maths.hpp"

struct Portal : public Viewer
{
	CCMaths::Vector3 m_position = CCMaths::Vector3::Zero;
	CCMaths::Vector3 m_rotation = CCMaths::Vector3::Zero;

	Portal* m_linkedPortal = nullptr;

	void Link(Portal* toLinkTo)
	{
		if (!toLinkTo)
			return;

		m_linkedPortal = toLinkTo;
		toLinkTo->m_linkedPortal = this;
	}
};