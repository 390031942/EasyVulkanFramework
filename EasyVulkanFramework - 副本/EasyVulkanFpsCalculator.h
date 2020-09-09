#pragma once
#include <time.h>  
#include <chrono>
#include <thread>
#include<windows.h>
#include<winbase.h>

#include "EasyVulkanConfig.h"
class EasyVulkanFpsCalculator
{
public:
	//用于计算FPS的辅助变量
	static int FPSCount;
	static long long startTime;
	static float currFPS;

	//用于控制帧速率的辅助变量
	static long long beforeTime;

	//计算FPS相关方法
	static void Init();
	static void Calculate();

	//控制帧速率相关方法
	static void BeForeFrame();
	static void AfterFrame(int dstFPS);
};

