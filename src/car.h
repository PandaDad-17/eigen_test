#ifndef __CAR_H__
#define __CAR_H__

#include <Eigen/Geometry>

#include <vector>

class Car
{
  // Simple non-descript car model
  const std::vector<Eigen::Vector3f> m_vertices = 
  {
    // Body bottom
    {-2.0f, -0.5f,  1.0f}, { 2.0f, -0.5f,  1.0f}, { 2.0f, -0.5f, -1.0f}, {-2.0f, -0.5f, -1.0f},
    // Body top (beltline)
    {-2.0f,  0.2f,  1.0f}, { 2.0f,  0.2f,  1.0f}, { 2.0f,  0.2f, -1.0f}, {-2.0f,  0.2f, -1.0f},
    // Roof
    {-0.5f,  0.8f,  0.8f}, { 1.0f,  0.8f,  0.8f}, { 1.0f,  0.8f, -0.8f}, {-0.5f,  0.8f, -0.8f},
    // Wheels (represented by squares for simplicity)
    {-1.2f, -0.8f,  1.0f}, {-0.8f, -0.8f,  1.0f}, {-1.2f, -0.5f,  1.0f}, {-0.8f, -0.5f,  1.0f}, // FL
    { 0.8f, -0.8f,  1.0f}, { 1.2f, -0.8f,  1.0f}, { 0.8f, -0.5f,  1.0f}, { 1.2f, -0.5f,  1.0f}, // FR
    {-1.2f, -0.8f, -1.0f}, {-0.8f, -0.8f, -1.0f}, {-1.2f, -0.5f, -1.0f}, {-0.8f, -0.5f, -1.0f}, // RL
    { 0.8f, -0.8f, -1.0f}, { 1.2f, -0.8f, -1.0f}, { 0.8f, -0.5f, -1.0f}, { 1.2f, -0.5f, -1.0f}  // RR
  };

  const std::vector<Eigen::Vector2i> m_edges = 
  {
    // Bottom rectangle
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    // Beltline rectangle
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    // Verticals body
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
    // Roof rectangle
    {8, 9}, {9, 10}, {10, 11}, {11, 8},
    // Roof pillars
    {4, 8}, {5, 9}, {6, 10}, {7, 11},
    // Wheels (Front Left)
    {12, 13}, {13, 15}, {15, 14}, {14, 12},
    // Wheels (Front Right)
    {16, 17}, {17, 19}, {19, 18}, {18, 16},
    // Wheels (Rear Left)
    {20, 21}, {21, 23}, {23, 22}, {22, 20},
    // Wheels (Rear Right)
    {24, 25}, {25, 27}, {27, 26}, {26, 24}  
  };

public:
  const std::vector<Eigen::Vector3f> vertices() const { return m_vertices; }
  const std::vector<Eigen::Vector2i> edges() const { return m_edges; }
};

#endif // __CAR_H__
