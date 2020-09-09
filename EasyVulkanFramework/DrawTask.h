#pragma once
#include "Texture.h"
#include "EasyVulkanPipeline.h"
#include <queue>
#include "EasyVulkanConfig.h"
#include <boost/thread.hpp>

class DrawTaskQueue
{
	static std::vector<int>            drawableRTTI;
	static std::queue<DrawableObject*> drawableObjectQueue;

	static bool isDrawableObject(VkObject* object)
	{
		for (int i = 0; i < drawableRTTI.size(); i++)
		{
			if (drawableRTTI.at(i) == object->GetRTTI())
				return true;
		}
		return false;
	}


public:
	static void addToQueue(DrawableObject* object)
	{
		if (!isDrawableObject(object))
		{
			return;
		}
		drawableObjectQueue.push(object);
	}

	static DrawableObject* getTopObject()
	{
		if (!drawableObjectQueue.size())
			return NULL;

		DrawableObject* object = drawableObjectQueue.front();
		drawableObjectQueue.pop();

		return object;
	}
};
class DrawTaskManager
{
private:
	static std::map<DrawableObject*, Drawable*> refMap;

	static boost::thread queueWatcherThread;

public:
	static vector<Drawable*> getDrawable()
	{
		vector<Drawable*> result;
		result.resize(refMap.size());

		for (auto it = refMap.cbegin(); it != refMap.end(); it++)
		{
			result.push_back((*it).second);
		}
		return result;
	}
	static void doQueueWatcher()
	{
		while (true)
		{
			DrawableObject* object;
			while((object = DrawTaskQueue::getTopObject()) != NULL)
			{
				addObject(object);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(VulkanConfig::DrawQueueWatcher::ScanDelayMillionSeconds));
		}
	}
	static void init()
	{
		queueWatcherThread = boost::thread(&DrawTaskManager::doQueueWatcher);
		queueWatcherThread.detach();
	}


	static void shutdown()
	{
		queueWatcherThread.interrupt();
	}
	static void addObject(DrawableObject* object)
	{
		if (object->GetRTTI() == RTTI_DRAWABLE_SPRITE)
		{
			Sprite* sprite = new Sprite;
			SpriteObject* spriteObject = static_cast<SpriteObject*>(object);
			if (sprite->Create(spriteObject->spriteName))
			{
				refMap[spriteObject] = sprite;
				spriteObject->valid = true;
			}
		}
	}

	static void removeObject(DrawableObject* object)
	{
		auto pair = refMap.find(object);
		Drawable* drawable = (*pair).second;

		delete drawable;
		drawable = NULL;
		refMap.erase(pair);
	}

	static void doDrawTask()
	{
		for (auto pair : refMap)
		{
			pair.second->Draw();
		}
	}
};

