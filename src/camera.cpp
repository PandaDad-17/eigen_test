
#include "camera.h"

void camera::move(float x, float y, float z)
{
  Eigen::Vector3f right = rotation_matrix.col(0);
  Eigen::Vector3f up = rotation_matrix.col(1);
  Eigen::Vector3f forward = rotation_matrix.col(2);

  // Negative forward to handle Eigen right-handedness.
  position += (right * x) + (up * y) + (-forward * z);
}

void camera::rotate(float pitch, float yaw, float roll)
{
  // Rotate by yaw first in order to keep orientation like that of a human head.
  Eigen::Matrix3f rotation_delta;
  rotation_delta = 
    Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitY()) *
    Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitX()) *
    Eigen::AngleAxisf(roll, Eigen::Vector3f::UnitZ());

  rotation_matrix = rotation_matrix * rotation_delta;

  // Avoid matrix drift from stacking floating point errors.
  Eigen::Vector3f forward = rotation_matrix.col(2).normalized();
  Eigen::Vector3f right = Eigen::Vector3f::UnitY().cross(forward).normalized();
  Eigen::Vector3f up = forward.cross(right);

  rotation_matrix.col(0) = right;
  rotation_matrix.col(1) = up;
  rotation_matrix.col(2) = forward;
}

void camera::look_at(const Eigen::Vector3f& target)
{
  Eigen::Vector3f forward = (position - target).normalized();
  Eigen::Vector3f right = Eigen::Vector3f::UnitY().cross(forward).normalized();
  Eigen::Vector3f up = forward.cross(right);

  rotation_matrix.col(0) = right;
  rotation_matrix.col(1) = up;
  rotation_matrix.col(2) = forward;
}

Eigen::Matrix4f camera::get_view_matrix() const
{
  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
  Eigen::Matrix3f R = rotation_matrix.transpose();

  // Set top-left 3x3 blocks to rotation.
  view.block<3, 3>(0, 0) = R;

  // Set the top 3 of the last column to the translation.
  view.block<3, 1>(0, 3) = -R * position;

  return view;
}


