#pragma once
#ifndef EASY_VULKAN_ASSERT
#define EASY_VULKAN_ASSERT

#include "EasyVulkanGlobal.h"

#include <qmessagebox.h>
#include <QFileInfo>
#define VkAssertTitle QString("Vulkan Fatal Error!")
#define VkQuitCode    -2181

#ifdef VK_ASSERT_ENABLE
#define VkAssert(expression,function) if(!expression) {\
                                 QMessageBox::critical(NULL,VkAssertTitle,QString("Vk Function:%1 Failed at File:%2 Line %3 ").\
                                 arg(function).arg(QFileInfo(__FILE__).fileName()).arg(__LINE__));\
                                 exit(VkQuitCode);}
#else
#define VkAssert(expression,function) 
#endif
#endif