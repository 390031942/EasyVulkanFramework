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
void Transform::MoveTo(float x, float y, float z)
{
	position = QVector3D(x, y, z);
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

	this->rotation = quaternion * _quaternion * this->rotation;
}

void Transform::RotateAround(Transform centerTransform, QVector3D axis, float angle)
{
	QVector3D pos = this->position;
	QQuaternion quaternion = QQuaternion::fromAxisAndAngle(axis, angle);
	QVector3D delta = pos - centerTransform.position;

	delta = quaternion * delta;
	pos = centerTransform.position + delta;

	this->position = pos;
	//this->RotateAround(axis, angle * *0.01745329f);

}

float* Transform::GetTransformMartix()
{
	EasyVulkanMatrixState::pushMatrix();

	EasyVulkanMatrixState::translate(position.x(), position.y(), position.z());

	EasyVulkanMatrixState::rotate(rotation.x(), 1, 0, 0);
	EasyVulkanMatrixState::rotate(rotation.y(), 0, 1, 0);
	EasyVulkanMatrixState::rotate(rotation.z(), 0, 0, 1);

	EasyVulkanMatrixState::scale(scale.x(), scale.y(), scale.z());

	float* martix = EasyVulkanMatrixState::getFinalMatrix();
	EasyVulkanMatrixState::popMatrix();

	return martix;
}
