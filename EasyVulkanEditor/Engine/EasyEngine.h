#pragma once
#include "Component.h"
#include "GameObject.h"
#include "DestoryQueue.h"
#include "Container.h"
class EasyEngine
{
	DestoryQueue<Component*> destoryQueue_Component;
	DestoryQueue<GameObject*> destoryQueue_GameObject;

	Container<Component*> container_Component;
	Container<GameObject*> container_GameObject;

	std::vector<std::string> build_in_Components_UUID;

	void doInit()
	{

	}

	void doStart()
	{

	}

	void doUpdate()
	{

	}

	void doShutdown()
	{

	}

	void updateTransform()
	{
		for (GameObject* obj : container_GameObject)
		{

		}
	}
};

