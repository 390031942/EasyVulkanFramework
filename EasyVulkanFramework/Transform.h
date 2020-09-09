#pragma once
/*EasyVulkan*/
#include "Matrix/EasyVulkanMatrixState.h"
/*QCLASS*/
#include <qvector3d.h>
#include <qquaternion.h>
#include <qtransform.h>
class Transform
{

public:
	QVector3D position;
	QVector3D rotation;
	QVector3D scale;

	QVector3D localPosition;
	QVector3D localRotation;
	QVector3D localScale;

	Transform();

	Transform(float x, float y, float z);

	void   MoveTo(float x, float y = 0.0f, float z = 0.0f);
	void   RotateAroundSelf(QVector3D eulerAngles);

	void   RotateAroundSpace(QVector3D eulerAngles);
	void   RotateAround(Transform centerTransform, QVector3D axis, float angle);
	float* GetTransformMartix();
};

