#include "GameObject.h"

bool GameObject::Property::getStatic()
{
	return _this->isStatic;
}

void GameObject::Property::setStatic(bool value)
{
	_this->isActive = value;
}

bool GameObject::Property::getActive()
{
	return _this->isActive;
}

void GameObject::Property::setActive(bool value)
{
	_this->isActive = value;
}

std::vector<Component*> GameObject::Components::get(std::string type)
{
	std::vector<Component*> result;
	for (auto component : _this->components)
	{
		if (component->type == type)
			result.push_back(component);
	}
	return result;
}

void GameObject::Components::remove(std::string type)
{
	for (Component* component : _this->components)
	{
		if (component->type == type)
			component->selfDestory();
	}
}

std::vector<Component*> GameObject::Components::getInParent(std::string type)
{
	std::vector<Component*> result;
	for (auto component : _this->parent->components)
	{
		if (component->type == type)
		{
			result.push_back(component);
		}
	}
	return result;
}

std::vector<Component*> GameObject::Components::getInChildren(std::string type)
{
	std::vector<Component*> result;
	for (auto gameObject : _this->childrens)
	{
		for (auto component : gameObject->components)
		{
			if (component->type == type)
			{
				result.push_back(component);
			}
		}
	}
	return result;
}

void GameObject::Operation::selfDestory()
{

}
