#include "EasyVulkanFpsCalculator.h"


int EasyVulkanFpsCalculator::FPSCount;
long long EasyVulkanFpsCalculator::startTime;
float EasyVulkanFpsCalculator::currFPS;
long long EasyVulkanFpsCalculator::beforeTime;

static inline long long  currentTimeMillis()
{
	return  GetTickCount64();//获取时间数据计算出毫秒数
}

void EasyVulkanFpsCalculator::Init()
{
	FPSCount = 0;
	startTime = currentTimeMillis();
}

void EasyVulkanFpsCalculator::Calculate()
{
	FPSCount++;
	if (FPSCount == 100)
	{
		FPSCount = 0;
		long long endTime = currentTimeMillis();
		currFPS = (float)(1000.0 / ((endTime - startTime) / 100.0));
		startTime = endTime;
		printf("FPS: %f\n", EasyVulkanFpsCalculator::currFPS);
	}
}

void EasyVulkanFpsCalculator::BeForeFrame()
{
	beforeTime = currentTimeMillis();
}

void EasyVulkanFpsCalculator::AfterFrame(int dstFPS)
{
	//计算指定FPS对应的每帧毫秒数
	int dstSpan = (int)(1000 / dstFPS) + 1;
	//计算此帧耗时
	long long span = currentTimeMillis() - beforeTime;
	//如果此帧耗时小于指定FPS对应的每帧毫秒数则加入动态时间休眠

	if (span < dstSpan)
	{
		if (!VulkanConfig::Display::USE_MAX_FPS)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(dstSpan - span));
		}
	}
}
