#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

class Camera
{
public:
	~Camera();

	glm::mat4 getViewMatrix() const;

	virtual void setPosition(const glm::vec3& position) {}
	virtual void rotate(float yaw, float pitch) {}
	virtual void move(const glm::vec3& offsetPos) {}

	const glm::vec3& getLook() const;
	const glm::vec3& getRight() const;
	const glm::vec3& getUp() const;
	const glm::vec3 getPosition() const;

	float getFOV() const { return mFOV; }
	void setFOV(float fov) { fov = mFOV; }

protected:
	Camera();

	virtual void updateCameraVectors() {}

	glm::vec3 mPosition;
	glm::vec3 mTargetPosition;
	glm::vec3 mUp;
	glm::vec3 mLook;
	glm::vec3 mRight;

	const glm::vec3 WORLD_UP;

	float mYaw;
	float mPitch;
	float mFOV;
};

class FPSCamera : public Camera
{
public:
	FPSCamera(glm::vec3 position = glm::vec3(0.F), float yaw = glm::pi<float>(),
		float pitch = 0.F);

	virtual void setPosition(const glm::vec3& position);
	virtual void rotate(float yaw, float pitch);
	virtual void move(const glm::vec3& offsetPos);

private:
	virtual void updateCameraVectors();
};

class OrbitCamera : public Camera
{
public:
	OrbitCamera();

	virtual void rotate(float yaw, float pitch);

	void setLookAt(const glm::vec3& target);
	void setRadius(float radius);

private:
	void updateCameraVectors();

	float mRadius;
};