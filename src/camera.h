#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <Eigen/Geometry>

class alignas(16) Camera
{
  Eigen::Quaternionf m_orientation{Eigen::Quaternionf::Identity()};
  Eigen::Vector3f m_position{Eigen::Vector3f::Zero()};
  float m_near, m_far;
  float m_fieldOfView;

public:
  Camera() = default;

  void set(const Eigen::Vector3f& position, float nearPlane, float farPlane, float fov) noexcept;
  
  void move(Eigen::Vector3f translation) noexcept;
  void rotate(float pitch, float yaw, float roll) noexcept;
  void lookAt(const Eigen::Vector3f& target) noexcept;

  [[nodiscard]] Eigen::Matrix4f viewMatrix() const noexcept;

  [[nodiscard]] float nearPlane() const noexcept { return m_near; }
  [[nodiscard]] float farPlane() const noexcept { return m_far; }

  [[nodiscard]] float fov() const noexcept { return m_fieldOfView; }
};

#endif // __CAMERA_H__
