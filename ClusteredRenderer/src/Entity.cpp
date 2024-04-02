#include "Entity.hpp"

#include "Components.hpp"

Entity::Entity(entt::entity handle, Scene* scene)
	: m_EntityHandle(handle), m_Scene(scene)
{
}

kb::UUID Entity::GetUUID() { 
	return GetComponent<IDComponent>().ID; 
}
const std::string& Entity::GetName() { 
	return GetComponent<TagComponent>().Tag; 
}