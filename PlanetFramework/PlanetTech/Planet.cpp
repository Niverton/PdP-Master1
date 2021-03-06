#include "Planet.h"
#include "Context.h"
#include "Frustum.h"      // for Frustum
#include "InputManager.h" // for InputManager
#include "Patch.h"        // for Patch
#include "Texture.h"      // for Texture
#include "Time.h"         // for Time
#include "Transform.h"    // for Transform
#include "Triangulator.h" // for Triangulator
#include "glad.h"         // for GLfloat
#include "utils.h"        // for SafeDelete, INPUT, TIME

#if PLATFORM_Win
#include <glm\glm.hpp>
#else
#include <glm/glm.hpp>
#endif

Planet::Planet() {
  m_pTransform = new Transform();
  m_pTriangulator = new Triangulator(this);
  m_pPatch = new Patch(4);
  m_pPatch->SetPlanet(this);
}

void Planet::Init() {
  m_pTransform->SetRotation(glm::rotate(m_pTransform->GetRotation(),
                                        glm::radians(270.f),
                                        glm::vec3(0.0f, 1.0f, 0.0f)));
  // LoadTextures
  m_pDiffuse->Load(false);
  m_pHeight->Load(false);

  m_pTriangulator->Init();
  m_pPatch->Init();
}

void Planet::Update() {
  m_pTransform->SetPosition(0, 0, 0);
  // m_pTransform->SetPosition(m_Radius*sinf(TIME->GetTime()), 0, 0);
  if (INPUT->IsKeyboardKeyPressed('r')) {
    m_Rotate = !m_Rotate;
  }
  if (m_Rotate) {
    m_pTransform->SetRotation(
        glm::rotate(m_pTransform->GetRotation(),
                    -static_cast<GLfloat>(TIME->DeltaTime()) * 0.01f,
                    glm::vec3(0.0f, 1.0f, 0.0f)));
  }

  m_pTransform->UpdateTransforms();

  // Change Planet Geometry
  //**********************
  if (m_pTriangulator->Update()) {
    // Change the actual vertex positions
    m_pTriangulator->GenerateGeometry();
    // Bind patch instances
    m_pPatch->BindInstances(m_pTriangulator->m_Positions);
    m_pPatch->UploadDistanceLUT(m_pTriangulator->m_DistanceLUT);
  }
}

void Planet::Draw() {
  m_pPatch->Draw();
  if (m_pTriangulator->IsFrustumLocked()) {
    m_pTriangulator->GetFrustum()->Draw();
  }
}
void Planet::DrawWire() {
  m_pPatch->Draw(true);
  if (m_pTriangulator->IsFrustumLocked()) {
    m_pTriangulator->GetFrustum()->Draw();
  }
}

Planet::~Planet() {
  SafeDelete(m_pTransform);
  SafeDelete(m_pDiffuse);
  SafeDelete(m_pHeight);

  SafeDelete(m_pPatch);
  SafeDelete(m_pTriangulator);
}

int Planet::GetVertexCount() {
  return m_pTriangulator->GetVertexCount() * m_pPatch->GetVertexCount();
}
