#include "Camera.hpp"
#include "glm\ext\matrix_transform.hpp"

const float DEF_FOV = 45.F;

Camera::Camera() : mPosition(glm::vec3(0.F)), mTargetPosition(glm::vec3(0.F)),
mUp(glm::vec3(0.F, 1.F, 0.F)), mYaw(glm::pi<float>()), mPitch(0.F), WORLD_UP(0.F, 1.F, 0.F),
mRight(0.F), mFOV(DEF_FOV)
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(mPosition, mTargetPosition, mUp);
}

const glm::vec3& Camera::getLook() const
{
	return mLook;
}

const glm::vec3& Camera::getRight() const
{
	return mRight;
}

const glm::vec3& Camera::getUp() const
{
	return mUp;
}

const glm::vec3 Camera::getPosition() const
{
	return mPosition;
}

OrbitCamera::OrbitCamera() : mRadius(10.F)
{
}

void OrbitCamera::rotate(float yaw, float pitch)
{
	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);

	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.F + 0.1F, glm::pi<float>() / 2.F - 0.1F);

	updateCameraVectors();
}

void OrbitCamera::setLookAt(const glm::vec3& target)
{
	mTargetPosition = target;
}

void OrbitCamera::setRadius(float radius)
{
	mRadius = glm::clamp(radius, 2.F, 80.F);
}

void OrbitCamera::updateCameraVectors()
{
	mPosition.x = mTargetPosition.x + mRadius * cosf(mPitch) * sinf(mYaw);

	mPosition.y = mTargetPosition.y + mRadius * sinf(mPitch);

	mPosition.z = mTargetPosition.z + mRadius * cosf(mPitch) * cosf(mYaw);
}

FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
{
	mPosition = position;

	mYaw = yaw;

	mPitch = pitch;
}

void FPSCamera::setPosition(const glm::vec3& position)
{
	mPosition = position;
}

void FPSCamera::rotate(float yaw, float pitch)
{
	mYaw += glm::radians(yaw);

	mPitch += glm::radians(pitch);

	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.F + 0.1F, glm::pi<float>() / 2.F - 0.1F);

	updateCameraVectors();
}

void FPSCamera::move(const glm::vec3& offsetPos)
{
	mPosition += offsetPos;

	updateCameraVectors();
}

void FPSCamera::updateCameraVectors()
{
	glm::vec3 look;

	look.x = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.z = cosf(mPitch) * cosf(mYaw);

	mLook = glm::normalize(look);

	mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
	mUp = glm::normalize(glm::cross(mRight, mLook));

	mTargetPosition = mPosition + mLook;
}