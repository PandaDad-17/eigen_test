#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <Eigen/Geometry>

class camera
{
  Eigen::Matrix3f rotation_matrix;
  Eigen::Vector3f position;
  float near, far;

  [[nodiscard]] float normalize_angle(float a)
  {
    // Eigen internally wraps angles from -pi -> pi.
    return Eigen::Rotation2D(a).smallestAngle();
  }

public:
  camera(const Eigen::Vector3f& pos, float near_plane, float far_plane)
    : position(pos)
    , near(near_plane)
    , far(far_plane)
  {
  }

  void move(float x, float y, float z);
  void rotate(float pitch, float yaw, float roll);
  void look_at(const Eigen::Vector3f& target);

  Eigen::Matrix4f get_view_matrix() const;

  float near_plane() const { return near; }
  float far_plane() const { return far; }
};

#endif // __CAMERA_H__
