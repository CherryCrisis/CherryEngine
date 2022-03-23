#include <pch.hpp>

#include <BEHAVIOUR.HPP>

std::string Behaviour::Serialize()
{
	std::string value;
	value += "m_owner:" + std::to_string(m_owner->GetUUID()) + "\n";
	// loop on every metadata to write them
	for (const auto& [fieldName, fieldRef] : m_metadatas.m_fields)
	{
		const std::type_index type = fieldRef.m_value.type();
		
		value += fieldRef.m_name + ":";

		if (type == typeid(CCMaths::Vector3**))
		{
			CCMaths::Vector3* ptr = *std::any_cast<CCMaths::Vector3**>(fieldRef.m_value);
			CCMaths::Vector3 val = *ptr;

			value += std::to_string(val.x) +"/"+ std::to_string(val.y) +"/"+ std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(CCMaths::Vector3*))
		{
			CCMaths::Vector3 val = *std::any_cast<CCMaths::Vector3*>(fieldRef.m_value);
			value += std::to_string(val.x) + "/" + std::to_string(val.y) + "/" + std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(std::string*))
		{
			std::string* val = std::any_cast<std::string*>(fieldRef.m_value);
			value += *val + "\n";
			continue;
		}

		if (type == typeid(float*))
		{
			float val = *std::any_cast<float*>(fieldRef.m_value);
			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(int*))
		{
			int val = *std::any_cast<int*>(fieldRef.m_value);
			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(Behaviour**))
		{
			Behaviour** ptrr = std::any_cast<Behaviour**>(fieldRef.m_value);
			Behaviour* ptr = *ptrr;
			
			if (ptr)
				value += std::to_string((uint64_t)ptr->GetUUID()) + "\n";
			else
				value += "none\n";
			continue;
		}

 		//UnHandled Cases (useful to find them)
		value += ": " + std::string("#ERROR# ")+ std::string(type.name()) + std::string(" is not handled !") +"\n";

	}

	return value;
}