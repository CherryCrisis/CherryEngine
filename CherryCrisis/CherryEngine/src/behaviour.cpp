#include <pch.hpp>

#include <BEHAVIOUR.HPP>

std::string Behaviour::Serialize()
{
	std::string value;
	// loop on every metada to write them
	for (const Field field : m_metadatas.m_fields)
	{
		const std::type_index type = field.m_value.type();
		
		value += field.m_name+": ";

		if (type == typeid(CCMaths::Vector3*))
		{
			CCMaths::Vector3 val = *std::any_cast<CCMaths::Vector3*>(field.m_value);
			value += std::to_string(val.x) +"/"+ std::to_string(val.y) +"/"+ std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(CCMaths::Vector3))
		{
			CCMaths::Vector3 val = std::any_cast<CCMaths::Vector3>(field.m_value);
			value += std::to_string(val.x) + "/" + std::to_string(val.y) + "/" + std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(std::string))
		{
			std::string val = std::any_cast<std::string>(field.m_value);
			": " + val + "\n";
			continue;
		}


		if (type == typeid(float))
		{
			float val = std::any_cast<float>(field.m_value);
			": " + std::to_string(val) + "\n";
			continue;
		}

		//UnHandled Cases (useful to find them)
		value += ": " + std::string("#ERROR# ")+ std::string(type.name()) + std::string(" is not handled !") +"\n";

	}

	return value;
}