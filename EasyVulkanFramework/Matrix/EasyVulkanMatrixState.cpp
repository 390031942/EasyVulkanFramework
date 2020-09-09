#include "EasyVulkanMatrixState.h"
#include <math.h>

float EasyVulkanMatrixState::vulkanClipMatrix[16];
float EasyVulkanMatrixState::currentMatrix[16];
float EasyVulkanMatrixState::ProjectMatrix[16];
float EasyVulkanMatrixState::mVMatrix[16];
float EasyVulkanMatrixState::mMVPMatrix[16];
float EasyVulkanMatrixState::mStack[10][16];
int EasyVulkanMatrixState::stackTop = -1;
float EasyVulkanMatrixState::cx;
float EasyVulkanMatrixState::cy;
float EasyVulkanMatrixState::cz;

void EasyVulkanMatrixState::setInitStack()
{
	EasyVulkanMatrix::setIdentityM(currentMatrix, 0);
	//��ʼ����OpenGL��׼�豸�ռ䵽Vulkan�豸�ռ�任�ľ���
	//OpenGL��׼�豸�ռ�XYZ�����᷶Χ���Ǵ�-1.0��+1.0
	//Vulkan�豸�ռ�XYZ�����᷶Χ�ֱ��� -1.0��+1.0��+1.0��-1.0��0.0��+1.0
	//�任ʱ�����ϲ��õ������ż�ƽ�ƾ���
	//X�᲻��Y���÷�Z������0.5
	//���ź�Z������ƽ��0.5
	vulkanClipMatrix[0] = 1.0f;
	vulkanClipMatrix[1] = 0.0f;
	vulkanClipMatrix[2] = 0.0f;
	vulkanClipMatrix[3] = 0.0f;

	vulkanClipMatrix[4] = 0.0f;
	vulkanClipMatrix[5] = -1.0f;
	vulkanClipMatrix[6] = 0.0f;
	vulkanClipMatrix[7] = 0.0f;

	vulkanClipMatrix[8] = 0.0f;
	vulkanClipMatrix[9] = 0.0f;
	vulkanClipMatrix[10] = 0.5f;
	vulkanClipMatrix[11] = 0.0f;

	vulkanClipMatrix[12] = 0.0f;
	vulkanClipMatrix[13] = 0.0f;
	vulkanClipMatrix[14] = 0.5f;
	vulkanClipMatrix[15] = 1.0f;
}

void EasyVulkanMatrixState::pushMatrix()
{
	stackTop++;
	for (int i = 0; i<16; i++)
	{
		mStack[stackTop][i] = currentMatrix[i];
	}
}

void EasyVulkanMatrixState::popMatrix()
{
	for (int i = 0; i<16; i++)
	{
		currentMatrix[i] = mStack[stackTop][i];
	}
	stackTop--;
}

void EasyVulkanMatrixState::translate(float x, float y, float z)
{
	EasyVulkanMatrix::translateM(currentMatrix, 0, x, y, z);
}

void EasyVulkanMatrixState::rotate(float angle, float x, float y, float z)
{
	EasyVulkanMatrix::rotateM(currentMatrix, 0, angle, x, y, z);
}

void EasyVulkanMatrixState::scale(float x, float y, float z)
{
	EasyVulkanMatrix::scaleM(currentMatrix, 0, x, y, z);
}

void EasyVulkanMatrixState::setCamera
(
	float cx,
	float cy,
	float cz,
	float tx,
	float ty,
	float tz,
	float upx,
	float upy,
	float upz
)
{
	EasyVulkanMatrixState::cx = cx;
	EasyVulkanMatrixState::cy = cy;
	EasyVulkanMatrixState::cz = cz;
	EasyVulkanMatrix::setLookAtM
	(
		mVMatrix,
		0,

		cx,
		cy,
		cz,

		tx,
		ty,
		tz,

		upx,
		upy,
		upz
	); 
}

void EasyVulkanMatrixState::setProjectFrustum
(
	float left,
	float right,
	float bottom,
	float top,
	float near,
	float far
)
{
	EasyVulkanMatrix::frustumM(ProjectMatrix, 0, left, right, bottom, top, near, far);
}

float* EasyVulkanMatrixState::getFinalMatrix()
{
	EasyVulkanMatrix::multiplyMM(mMVPMatrix, 0, mVMatrix, 0, currentMatrix, 0);
	EasyVulkanMatrix::multiplyMM(mMVPMatrix, 0, ProjectMatrix, 0, mMVPMatrix, 0);
	EasyVulkanMatrix::multiplyMM(mMVPMatrix, 0, vulkanClipMatrix, 0, mMVPMatrix, 0);
	return mMVPMatrix;
}

float* EasyVulkanMatrixState::getMMatrix()
{
	return currentMatrix;
}




