#include "Transform.h"

Transform::Transform()
{
	position = QVector3D(0, 0, 0);
	rotation = QVector3D(0, 0, 0);
	scale = QVector3D(1, 1, 1);
}

Transform::Transform(float x, float y, float z)
{
	position = QVector3D(x, y, z);
	rotation = QVector3D(0, 0, 0);
	scale = QVector3D(1, 1, 1);
}
