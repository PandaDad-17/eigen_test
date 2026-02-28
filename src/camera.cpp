
#include "camera.h"

void Camera::set(const Eigen::Vector3f& position, float nearPlane, float farPlane, float fov) noexcept
{
  m_position = position;
  m_near = nearPlane;
  m_far = farPlane;
  m_fieldOfView = fov;
}

void Camera::move(Eigen::Vector3f translation) noexcept
{
  // Negative forward to handle Eigen right-handedness.
  translation.z() *= -1;

  m_position += m_orientation * translation;
}

void Camera::rotate(float pitch, float yaw, float roll) noexcept
{
  Eigen::Quaternionf qYaw(Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitY()));
  Eigen::Quaternionf qPitch(Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitX()));
  Eigen::Quaternionf qRoll(Eigen::AngleAxisf(roll, Eigen::Vector3f::UnitZ()));

  // Rotate by yaw first in order to keep orientation like that of a human head (and not tilt like a plane).
  m_orientation = qYaw * m_orientation * qPitch * qRoll;

  // Avoid matrix drift from stacking floating point errors.
  m_orientation.normalize();
}

void Camera::lookAt(const Eigen::Vector3f& target) noexcept
{
  Eigen::Vector3f forward = (m_position - target).normalized();
  Eigen::Vector3f right = Eigen::Vector3f::UnitY().cross(forward).normalized();
  Eigen::Vector3f up = right.cross(right).normalized();

  Eigen::Matrix3f rotationMatrix;
  rotationMatrix.col(0) = right;
  rotationMatrix.col(1) = up;
  rotationMatrix.col(2) = forward;

  m_orientation = Eigen::Quaternionf(rotationMatrix);
  m_orientation.normalize();
}

Eigen::Matrix4f Camera::viewMatrix() const noexcept
{
  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
  Eigen::Matrix3f transpose = m_orientation.toRotationMatrix().transpose();

  // Set top-left 3x3 blocks to rotation.
  view.block<3, 3>(0, 0) = transpose;

  // Set the top 3 of the last column to the translation.
  view.block<3, 1>(0, 3) = -transpose * m_position;

  return view;
}


