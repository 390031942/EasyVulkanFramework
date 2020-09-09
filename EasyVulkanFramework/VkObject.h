#pragma once

/*EasyVulkan*/
#include "Transform.h"

/*STL*/
#include <string>
#include <vector>

/*QCLASS*/
#include <QUuid>
#include <QFile>

/*MACRO*/
#define CHECK_OBJECT_NOTNULL(object) if(object == NULL) return;

class VkObject
{
public:
	std::string						name;
	bool							actived;
	bool							isStatic;
	Transform						transform;

private:
	VkObject*						parent;
	std::vector<VkObject*>          children;
	std::string                     uuid;

public:
	VkObject();

	void				            SetParent(VkObject* parent);
	void							AddChild(VkObject* child);	
	void						    RemoveChild(VkObject* child);
	std::string					    GetUUId();

	//virtual VkObject*			    Copy(const VkObject*) = 0;
	virtual int					    GetRTTI() = 0;
};

