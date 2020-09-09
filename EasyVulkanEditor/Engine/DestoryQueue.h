#pragma once
#include <queue>

template <typename Target>
class DestoryQueue
{
public:
	std::queue<Target> queue;

	void pushTarget(Target target)
	{
		queue.push(target);
	}

	void doDestory()
	{
		for (auto target : queue)
		{
			delete target;
		}
	}
};
