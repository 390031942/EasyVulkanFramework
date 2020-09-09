#pragma once
#include<vector>
#include<string>
#include<map>
using namespace std;

template<typename T>
class Container
{
private:
	std::map<std::string, T> objectMap;
public:
	T* get(std::string  uuid)
	{
		return objectMap[uuid];
	}

	void push(std::string uuid, T* ptr)
	{
		objectMap[uuid] = ptr; 
	}

	T* getNext()
	{
		
	}
};
