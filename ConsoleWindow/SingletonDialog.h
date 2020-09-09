#pragma once
#include <QDialog>
#include <map>
class SingletonDialog:public QDialog
{
private:
	static std::map<std::string,SingletonDialog*> instanceMap;
	static std::string instanceType;
	static bool newInstanceFlag;

public:

	static void setInstanceType(std::string type)
	{
		instanceType = type;
	}
	void* operator new(size_t sz)
	{
		auto iter = instanceMap.find(instanceType);
		if (iter != instanceMap.cend())
		{
			newInstanceFlag = false;
			return (*iter).second;
		}

		void* ptr = instanceMap[instanceType] = reinterpret_cast<SingletonDialog*>(::new char[sz]);	
		newInstanceFlag = true;
		return ptr;
	}
	void operator delete(void* p)
	{
		instanceMap.erase(instanceMap.find(instanceType));

		::delete[]p;
	}
	static bool isInstanceNew()
	{
		return newInstanceFlag;
	}

	void close()
	{
		SingletonDialog* dialog = instanceMap.find(instanceType).operator*().second;
		for (int i = 0; i < dialog->children().size(); i++)
		{
			static_cast<QWidget*>(dialog->children()[i])->close();
		}
		QDialog::close();
	}

};

