#include "VkObject.h"

void VkObject::SetParent(VkObject* parent)
{
	CHECK_OBJECT_NOTNULL(parent);
	this->parent = parent;
}

void VkObject::AddChild(VkObject* child)
{
	CHECK_OBJECT_NOTNULL(child);
	this->children.push_back(child);
}

void VkObject::RemoveChild(VkObject* child)
{
	for (auto iter = children.cbegin(); iter != children.cend(); iter++)
	{
		if (*iter == child)
			children.erase(iter);
	}
}

VkObject::VkObject()
{
	this->uuid = QUuid::createUuid().toString().toStdString();
}

std::string VkObject::GetUUId()
{
	return this->uuid;
}
