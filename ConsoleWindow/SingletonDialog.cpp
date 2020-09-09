#include "SingletonDialog.h"
std::string                             SingletonDialog::instanceType;
std::map<std::string, SingletonDialog*> SingletonDialog::instanceMap;
bool                                    SingletonDialog::newInstanceFlag;