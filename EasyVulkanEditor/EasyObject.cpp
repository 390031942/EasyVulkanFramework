#include "EasyObject.h"

std::string EasyObject::toString()
{
	return std::string("class EasyObject at")+std::to_string(reinterpret_cast<long>(this));
}
