#pragma once
/*STL*/
#include <map>
#include<string>
#include <vector>
/*Boost*/
#include <boost/shared_ptr.hpp>
/*QT*/
#include <QMutex>
#include <QMutexLocker>

//user_shared_class to define a class which is shared between C++ and python
#define _shared_

template<typename T>
static boost::shared_ptr<T> get_shared_ptr()
{
    return new T;
}

#define _Singleton_declare_(cls)  public:static cls* getInstance();static cls* __instance; static QMutex __mutex;

#define _Singleton_impl_(cls)     cls* cls::__instance = 0;                      \
                                  QMutex cls::__mutex; cls* cls::getInstance() \
                                  {                                                              \
                                      if (!__instance)                    \
                                      {                                                          \
                                          QMutexLocker locker(&__mutex);                         \
                                          if(!__instance)                                        \
                                          {                                                      \
                                                 cls* instance = new cls;                        \
                                                __instance = instance;                            \
                                          }                                                      \
                                      }                                                          \
                                      return __instance;                                         \
                                  }

#define _uuid_declare_         std::string getUUID();
#define _uuid_impl_(cls,uuid)  std::string cls::getUUID(){ return uuid;}

class EasyObject
{
public:
    virtual std::string getUUID() = 0;
    virtual std::string toString();
};
