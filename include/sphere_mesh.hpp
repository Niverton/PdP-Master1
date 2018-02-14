#pragma once

#include "type.hpp"
#include <glm/glm.hpp>
#include <ostream>
#include <vector>

class SphereMesh {
  friend void SpherifiedCube(uint32_t, SphereMesh &);
  friend std::ostream &operator<<(std::ostream &o, const SphereMesh &mesh);

public:
  SphereMesh(float radius_, int density);
  std::ostream &operator<<(std::ostream &o);

  std::vector<type::Vertex> &getVertices() {
    return vertices;
  }
  std::vector<type::Face> &getFaces() {
    return faces;
  }

private:
  void Generate(int density);

  int addVertex(glm::vec3 v);
  void addQuad(int a, int b, int c, int d);
  void addQuadAlt(int a, int b, int c, int d);

  float radius;

  std::vector<type::Vertex> vertices;
  std::vector<type::Face> faces;
};
