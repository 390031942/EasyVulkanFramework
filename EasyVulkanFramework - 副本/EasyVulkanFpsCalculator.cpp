#include "EasyVulkanFpsCalculator.h"


int EasyVulkanFpsCalculator::FPSCount;
long long EasyVulkanFpsCalculator::startTime;
float EasyVulkanFpsCalculator::currFPS;
long long EasyVulkanFpsCalculator::beforeTime;

static inline long long  currentTimeMillis()
{
	return  GetTickCount64();//��ȡʱ�����ݼ����������
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
	//����ָ��FPS��Ӧ��ÿ֡������
	int dstSpan = (int)(1000 / dstFPS) + 1;
	//�����֡��ʱ
	long long span = currentTimeMillis() - beforeTime;
	//�����֡��ʱС��ָ��FPS��Ӧ��ÿ֡����������붯̬ʱ������

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
