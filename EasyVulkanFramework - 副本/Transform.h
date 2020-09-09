#pragma once
#include <qvector3d.h>

class Transform
{
public:
	QVector3D position;
	QVector3D rotation;
	QVector3D scale;

	Transform();

	Transform(float x, float y, float z);
};

