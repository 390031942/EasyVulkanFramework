#pragma once

#include <QWindow>
#include "EasyVulkanRenderer.h"


class EasyVulkanWindow : public QWindow
{
	Q_OBJECT

public:
	EasyVulkanWindow();
	~EasyVulkanWindow();
	EasyVulkanRenderer* render;
	
	void InitVulkan()
	{
		render = new EasyVulkanRenderer;
		render->SetWindowHandle((HWND)winId());
		render->CreateVkInstance();
		render->CreateDebugConsole();
		render->EnumerateVkPhysicalDevices();
		render->CreateVkDevice();
		render->CreateVkCommandBuffer();
		render->InitQueue();
		render->CreateVkSwapChain();
		render->CreateVkDepthBuffer();
		render->CreateVkRenderPass();
		render->CreateFrameBuffer();
		render->CreatePipeline();
		render->CreateFence();
		render->InitPresentInfo();
		render->InitTransformMartix();
		render->InitDrawableResources();
	}
	void StartNextFrame()
	{
		render->Draw();
	}
	void DestoryVulkan()
	{
		render->DestroyFence();
		render->DestroyPipeline();
		render->DestroyDrawableObject();
		render->DestroyFrameBuffer();
		render->DestroyRenderPass();
		render->DestroyDepthBuffer();
		render->DestroySwapChain();
		render->DestroyCommandBuffer();
		render->DestroyDevices();
		render->DestroyDebugConsole();
		render->DestroyInstance();
	}
};
