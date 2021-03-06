#include "Camera.h"
#include "Context.h"
#include "InputManager.h"      // for InputManager
#include "PlanetTech/Planet.h" // for Planet
#include "Settings.h"          // for Settings::WindowSettings
#include "Time.h"              // for Time
#include "Transform.h"         // for Transform
#include "utils.h"             // for INPUT, TIME, WINDOW
#include <cmath>               // for powf, cos, sin, sqrtf

#if PLATFORM_Win
#include <SDL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#else
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

Camera::Camera() {
  m_pTransform = new Transform();
  m_pTransform->SetPosition(0, 0, -6);
}

void Camera::Update() {
  m_Moved = false;
  if (INPUT->IsMouseButtonDown(1)) {
    glm::vec2 look = INPUT->GetMouseMovement();
    look.x *= -m_RotationSpeed * TIME->DeltaTime();
    look.y *= -m_RotationSpeed * TIME->DeltaTime();
    m_pTransform->SetRotation(
        glm::rotate(m_pTransform->GetRotation(), look.y, glm::vec3(1, 0, 0)));
    m_pTransform->SetRotation(
        glm::rotate(m_pTransform->GetRotation(), look.x, glm::vec3(0, 1, 0)));
  }
  if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_4) && m_FOV < 110) {
    m_FOV += 30.f * TIME->DeltaTime();
    m_Moved = true;
  }
  if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_KP_6) && m_FOV > 5) {
    m_FOV -= 30.f * TIME->DeltaTime();
    m_Moved = true;
  }
  if (INPUT->IsKeyboardKeyDown('w')) {
    m_Altitude -= ((m_Altitude * 2.0f) - m_Altitude) * TIME->DeltaTime();
    m_Moved = true;
  }
  if (INPUT->IsKeyboardKeyDown('s')) {
    m_Altitude += ((m_Altitude * 2.0f) - m_Altitude) * TIME->DeltaTime();
    m_Moved = true;
  }
  float deltaLong = m_Longitude;
  if (INPUT->IsKeyboardKeyDown('a')) {
    m_Longitude -= 0.5f * TIME->DeltaTime();
    m_Moved = true;
  }
  if (INPUT->IsKeyboardKeyDown('d')) {
    m_Longitude += 0.5f * TIME->DeltaTime();
    m_Moved = true;
  }
  deltaLong -= m_Longitude;
  m_pTransform->SetRotation(
      glm::rotate(m_pTransform->GetRotation(), deltaLong, glm::vec3(0, 1, 0)));

  float dist = m_pPlanet->GetRadius() + m_Altitude;
  // std::cout << "Altitude (earth): " << (dist - m_pPlanet->GetRadius()) *
  // (6371.f / m_pPlanet->GetRadius()) << std::endl;

  // Calculate far plane based on planet
  m_FarPlane =
      sqrtf(powf(m_pPlanet->GetRadius() + m_Altitude, 2) -
            powf(m_pPlanet->GetRadius(), 2)) +
      sqrtf(powf(m_pPlanet->GetRadius() + m_pPlanet->GetMaxHeight(), 2) -
            powf(m_pPlanet->GetRadius(), 2));
  m_NearPlane = m_FarPlane * 0.000003f;

  m_pTransform->SetPosition(sin(m_Longitude) * dist, 0,
                            -cos(m_Longitude) * dist);

  m_pTransform->UpdateTransforms();

  if (m_PerspectiveProjection) {
    m_Projection = glm::perspectiveLH(glm::radians(m_FOV),
                                      static_cast<float>(WINDOW.Width) /
                                          static_cast<float>(WINDOW.Height),
                                      m_NearPlane, m_FarPlane);
  } else {
    float viewWidth = (m_Size > 0) ? m_Size * WINDOW.AspectRatio : WINDOW.Width;
    float viewHeight = (m_Size > 0) ? m_Size : WINDOW.Height;
    m_Projection =
        glm::ortho(0.f, viewWidth, viewHeight, 0.f, m_NearPlane, m_FarPlane);
  }

  glm::vec3 worldPos = m_pTransform->GetPosition();
  glm::vec3 lookAt = worldPos + m_pTransform->GetForward();
  glm::vec3 upVec = m_pTransform->GetUp(); // glm::vec3(0, 1, 0);//
  m_View = glm::lookAtLH(worldPos, lookAt, upVec);

  m_ViewInverse = glm::inverse(m_View);
  m_ViewProjection = m_Projection * m_View;
  m_ViewProjectionInverse = glm::inverse(m_ViewProjection);
}

Camera::~Camera() {
  SafeDelete(m_pTransform);
}
