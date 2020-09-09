#include "Transform.h"

Transform::Transform()
{
	this->globalPosition = QVector3D(0, 0, 0);
	this->globalRotation = QVector3D(0, 0, 0);
	this->globalScale = QVector3D(1, 1, 1);
}

Transform::Transform(float x, float y, float z)
{
	this->globalPosition = QVector3D(x, y, z);
	this->globalRotation = QVector3D(0, 0, 0);
	this->globalScale = QVector3D(1, 1, 1);
}
void Transform::MoveTo(float x, float y, float z)
{
	this->globalPosition = QVector3D(x, y, z);
}

void Transform::RotateAroundSelf(QVector3D eulerAngles)
{
	QQuaternion quaternion = QQuaternion::fromEulerAngles(eulerAngles);
	this->localRotation = quaternion * this->localRotation;
}
void Transform::RotateAroundSpace(QVector3D eulerAngles)
{
	QQuaternion quaternion = QQuaternion::fromEulerAngles(eulerAngles);
	QQuaternion _quaternion = quaternion.inverted();

	this->globalRotation = quaternion * _quaternion * this->globalRotation;
}

void Transform::RotateAround(Transform centerTransform, QVector3D axis, float angle)
{
	QVector3D pos = this->globalPosition;
	QQuaternion quaternion = QQuaternion::fromAxisAndAngle(axis, angle);
	QVector3D delta = pos - centerTransform.globalPosition;

	delta = quaternion * delta;
	pos = centerTransform.globalPosition + delta;

	this->globalPosition = pos;
	//this->RotateAround(axis, angle * *0.01745329f);

}

float* Transform::GetTransformMartix()
{
	/*EasyVulkanMatrixState::pushMatrix();

	EasyVulkanMatrixState::translate(position.x(), position.y(), position.z());

	EasyVulkanMatrixState::rotate(rotation.x(), 1, 0, 0);
	EasyVulkanMatrixState::rotate(rotation.y(), 0, 1, 0);
	EasyVulkanMatrixState::rotate(rotation.z(), 0, 0, 1);

	EasyVulkanMatrixState::scale(scale.x(), scale.y(), scale.z());

	float* martix = EasyVulkanMatrixState::getFinalMatrix();
	EasyVulkanMatrixState::popMatrix();

	return martix;*/

	return NULL;
}
