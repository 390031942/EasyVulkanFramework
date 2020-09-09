#pragma once
#include "../EasyObject.h"
#include "Component.h"
#include "Transform.h"
#define __Inner_NameSpace_Begin__(_parent_class_,_namespace_name_) class _namespace_name_ \
															{ _parent_class_ * _this;  public:
					
#define __Inner_NameSpace_End__(__namespace__inst__) }__namespace__inst__;

#define __Property_Begin__(_parent_class_) __Inner_NameSpace_Begin__(_parent_class_,Property)

#define __Property_End__ __Inner_NameSpace_End__(property)

#define __Coms_Begin__(_parent_class_)  __Inner_NameSpace_Begin__(_parent_class_,Components)

#define __Coms_End__ __Inner_NameSpace_End__(Components)

#define __Operation_Begin__(_parent_class_)  __Inner_NameSpace_Begin__(_parent_class_,Operation)

#define __Operation_End__ __Inner_NameSpace_End__(operation)

class Transform;
class GameObject:public EasyObject
{
	_uuid_declare_
private:
	bool        isStatic;
	bool        isActive;
	std::string runTimeUUID;

	std::vector<Component*> components;
public:
	GameObject* parent;
	std::vector<GameObject*> childrens;

	__Property_Begin__(GameObject)
	bool getStatic();
	void setStatic(bool value);
	bool getActive();
	void setActive(bool value);
	__Property_End__
	
	
	__Coms_Begin__(GameObject)
	std::vector<Component*> get(std::string type);
	void remove(std::string type);
	std::vector<Component*> getInParent(std::string type);
	std::vector<Component*> getInChildren(std::string type);
	__Coms_End__


	__Operation_Begin__(GameObject)
	void selfDestory();
	__Operation_End__

};

