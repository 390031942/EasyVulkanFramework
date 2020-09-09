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
	//���ڼ���FPS�ĸ�������
	static int FPSCount;
	static long long startTime;
	static float currFPS;

	//���ڿ���֡���ʵĸ�������
	static long long beforeTime;

	//����FPS��ط���
	static void Init();
	static void Calculate();

	//����֡������ط���
	static void BeForeFrame();
	static void AfterFrame(int dstFPS);
};

