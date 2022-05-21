#include <pch.hpp>

#include "uuid.hpp"

#include <random>
#include <unordered_map>


static std::random_device rDevice;
static std::mt19937_64 rEngine(rDevice());
static std::uniform_int_distribution<uint32_t> rDistribution;

CCUUID::CCUUID()
{
	m_uuid = rDistribution(rEngine);
}