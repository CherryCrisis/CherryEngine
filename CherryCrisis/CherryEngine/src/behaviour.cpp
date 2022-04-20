#include <pch.hpp>

// TODO: Change this
#include "behaviour.hpp"
#include "transform.hpp"
#include "bool3.hpp"

std::string Behaviour::Serialize()
{
	std::string value;
	value += "m_owner:" + std::to_string(m_owner->GetUUID()) + "\n";
	
	// loop on every metadata to write them
	for (const auto& [fieldName, fieldRef] : m_metadatas.m_fields)
	{
		const std::type_index& type = fieldRef.m_type;
		
		value += fieldRef.m_name + ":";

		if (type == typeid(CCMaths::Vector3))
		{
			CCMaths::Vector3 val = *std::any_cast<CCMaths::Vector3*>(fieldRef.m_value);
			value += std::to_string(val.x) + "/" + std::to_string(val.y) + "/" + std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(Bool3))
		{
			Bool3 val = *std::any_cast<Bool3*>(fieldRef.m_value);
			value += std::to_string(val.x) + "/" + std::to_string(val.y) + "/" + std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(std::string))
		{
			std::string val = *std::any_cast<std::string*>(fieldRef.m_value);
			if (val.size() > 0)
				value += val + "\n";
			else
				value += "null\n";
			continue;
		}

		if (type == typeid(float))
		{
			float val = *std::any_cast<float*>(fieldRef.m_value);
			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(int))
		{
			int val = *std::any_cast<int*>(fieldRef.m_value);
			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(bool))
		{
			bool val = *std::any_cast<bool*>(fieldRef.m_value);
			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(Behaviour*))
		{
			Behaviour** ptrr = std::any_cast<Behaviour**>(fieldRef.m_value);
			Behaviour* ptr = *ptrr;
			
			if (ptr)
				value += std::to_string((uint32_t)ptr->GetUUID()) + "\n";
			else
				value += "none\n";
			continue;
		}

 		//Unhandled Cases (useful to find them)
		value +=  std::string("#ERROR# ")+ std::string(type.name()) + std::string(" is not handled !") +"\n";
	}

	//Loop on every property to save them
	for (const auto& [propName, propRef] : m_metadatas.m_properties)
	{
		const std::type_index& type = propRef->GetGetType();

		value += propName + ":";

		if (type == typeid(CCMaths::Vector3))
		{
			CCMaths::Vector3 val;
			propRef->Get(&val);
			value += std::to_string(val.x) + "/" + std::to_string(val.y) + "/" + std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(Bool3))
		{
			Bool3 val;
			propRef->Get(&val);
			value += std::to_string(val.x) + "/" + std::to_string(val.y) + "/" + std::to_string(val.z) + "\n";
			continue;
		}

		if (type == typeid(std::string))
		{
			std::string val;
			propRef->Get(&val);

			if (val.size() > 0)
				value += val + "\n";
			else
				value += "null\n";
			continue;
		}

		if (type == typeid(float))
		{
			float val;
			propRef->Get(&val);

			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(int))
		{
			int val;
			propRef->Get(&val);

			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(bool))
		{
			bool val;
			propRef->Get(&val);
			value += std::to_string(val) + "\n";
			continue;
		}

		if (type == typeid(Behaviour*))
		{
			Behaviour* ptr;
			propRef->Get(&ptr);

			if (ptr)
				value += std::to_string((uint32_t)ptr->GetUUID()) + "\n";
			else
				value += "none\n";
			continue;
		}

		if (type == typeid(Transform*))
		{
			Transform* ptr;
			propRef->Get(&ptr);

			if (ptr)
				value += std::to_string((uint32_t)ptr->GetUUID()) + "\n";
			else
				value += "none\n";
			continue;
		}

		//UnHandled Cases (useful to find them)
		value += std::string("#ERROR# ") + std::string(type.name()) + std::string(" is not handled !") + "\n";

	}

	return value;
}