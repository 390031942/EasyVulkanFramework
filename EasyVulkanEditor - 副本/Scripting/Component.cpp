#include "Component.h"
ComponentDestoryQueue::Ptr ComponentDestoryQueue::m_instance_ptr;
std::mutex ComponentDestoryQueue::m_mutex;
std::string ComponentDestoryQueue::str;
