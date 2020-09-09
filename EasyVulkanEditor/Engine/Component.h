#pragma once
#include <string>
#include <queue>
#include <boost/shared_ptr.hpp>
#include <mutex>
#include "GameObject.h"


#define __build_In_Component__ 

class GameObject;
class Component
{
public:
	bool             enabled;
	std::string      type;
	std::string      uid;

	GameObject* gameObject;
	void selfDestory();
};
