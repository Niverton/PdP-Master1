#pragma once

#ifdef TEST_patch
#include "Patch_test.h"
#endif

#include "glad.h" // for GLint, GLuint

#if PLATFORM_Win
#include <glm\glm.hpp>
#else
#include <glm/glm.hpp>
#endif
#include <vector> // for vector
class Planet;     // lines 8-8
class Shader;     // lines 7-7

struct PatchVertex {
  PatchVertex(glm::vec2 position, glm::vec2 morphVec) {
    pos = position;
    morph = morphVec;
  }
  glm::vec2 pos;
  glm::vec2 morph;
};
struct PatchInstance {
  PatchInstance(short Level, glm::vec3 A, glm::vec3 R, glm::vec3 S) {
    level = Level;
    a = A;
    r = R;
    s = S;
  }
  int level;
  glm::vec3 a;
  glm::vec3 r;
  glm::vec3 s;
};
/**
  \brief  Use shader called patch to perform smooth transition with morph.

*/
class Patch {
public:
  Patch(short levels = 5);
  ~Patch();

  void SetPlanet(Planet *pPlanet) {
    m_pPlanet = pPlanet;
  }

  unsigned int GetVertexCount() const {
    return m_Vertices.size();
  }

  /**
    \brief  Initialize shader and OpenGL buffer
  */
  void Init();

    /**
   \brief 
   This function is executed only at the start of the application. It setup an openGl VBO/EBO mechanism, which will subdivise all leafs of the recursion in Triangulator into smaller triangles. For each vertex, he also specify the orientation of the morph. He does that by building an iteration, virtually keeping trace on which row/column he is, which allow him to know the exact position of the vertex, and it's morph orientation. 
   This iteration is build like this: 

   He begin on the row 0. Keeping in mind that the upper row have RC as coordinate:
   He will iterate on the first RC Vertex(RC represent the number of vertex that will cut the base into RC-1 equals part). For each one, he will:
   
1: calculate the relative position, knowing that we are on row 0/RC and coulmn column/RC 
2: calculate their morph orientation, thanks to the parity of row column. (We must ensure that for each vertex, if it morph top, the right neighboor need to morph left, and the upper one right and bot.)
3: build one triangle whit the neightboors(even the upper ones(upperRow + column), which are not done yet)

Then he is in row 1., which will contain RC-1 vertex. He calculate the coordinate of the first triangle of row 2... The iteration go on until the last upper vertex  
   * */
  void GenerateGeometry(short levels);
  /**
   \brief
   Take a vector of Triangle generated by the recursion in Triangulator, and bind it whit openGl, using the VBO/EBO mechanism created in generateGeometry.
   * */
  void BindInstances(std::vector<PatchInstance> &instances);
  /*
   \brief
   Upload the the level dependent morph distance in the opengl shader, according to the calculation of Triangulator
   * */
  void UploadDistanceLUT(std::vector<float> &distances);
  void Draw(bool white = false);

private:
#ifdef TEST_patch
  friend int test();
#endif
  std::vector<PatchVertex> m_Vertices;
  std::vector<unsigned int> m_Indices;

  Planet *m_pPlanet = nullptr;

  int m_NumInstances = 0;

  short m_Levels;
  unsigned int m_RC{};

  // OpenGl stuff
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_VBOInstance{};

  Shader *m_pPatchShader = nullptr;

  GLint m_uModel{};
  GLint m_uViewProj{};

  GLint m_uMaxHeight{};

  GLint m_uCamPos{};
  GLint m_uRadius{};
  float m_MorphRange = 0.5f;
  GLint m_uMorphRange{};

  // shading
  glm::vec3 m_Ambient = glm::vec3(0.05f, 0.05f, 0.08f);
  GLint m_uAmbient{};
};
