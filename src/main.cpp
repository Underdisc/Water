//////////////////////////////////////////////////////////////////////////////
/// @file main.cpp
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-22-5
///
/// @brief
/// Entry point for water simulation testing.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <fstream>
#include <utility>

#include <GL/glew.h>
#include <GLM/glm/gtc/type_ptr.hpp>
#include "ext/imgui.h"
#include "ext/imgui_impl_sdl_gl3.h"

#include "Context.h"
#include "OpenGLContext.h"
#include "GenericAction.h"
#include "Time.h"
#include "Error.h"
#include "Camera.h"
#include "CameraController.h"
#include "ShaderLibrary.h"
#include "Framer.h"


#include "Water.h"
#include "WaterFFT.h"

#define SHOW_BASIS
//#define WATER_GERSTNER

class vec3
{
public:
  vec3(float x, float y, float z) : 
    x(x), y(y), z(z) {}
  union {
    struct {
      float x, y, z;
    };
    struct {
      float r, g, b;
    };
  };
};

// temporary
GLuint x_axis_vbo;
GLuint y_axis_vbo;
GLuint z_axis_vbo;
GLuint test_vbo;

GLuint x_axis_vao;
GLuint y_axis_vao;
GLuint z_axis_vao;
GLuint test_vao;

glm::vec2 location(0.0f, 0.0f);

LineShader * line_shader;
// temporary


bool demo_mode = false;
bool editor_show = true;
vec3 editor_clear_color(0.0f, 0.0f, 0.0f);
float editor_height_scale = 0.35;
float editor_displace_scale = 0.35f;

inline void WindowInit()
{
  Context::Create("Water", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();
  Context::CheckEvents();
}

inline void InitialUpdate()
{
  Time::Update();
  ImGui_ImplSdlGL3_NewFrame(Context::SDLWindow());
  Context::CheckEvents();
  GenericAction::UpdateAll();
}

void EditorUpdate()
{
  if ((Input::KeyDown(Key::SHIFTLEFT) || Input::KeyDown(Key::SHIFTRIGHT)) && Input::KeyPressed(Key::H))
      editor_show = !editor_show;
  if (!editor_show) 
    return;
  ImGui::Begin("Editor");
  if (ImGui::CollapsingHeader("Debug Info")) 
  {
    ImGui::Text("Time Passed: %f", Time::TotalTime());
    ImGui::Text("FPS: %f", Framer::AverageFPS());
    ImGui::Text("Frame Usage: %f", Framer::AverageFrameUsage());
  }
  if (ImGui::CollapsingHeader("Global Properties")) 
  {
    ImGui::DragFloat("Time Scale", &Time::m_TimeScale, 0.01f);
  }
  if (ImGui::CollapsingHeader("Hotkeys")) 
  {
    ImGui::BulletText("Hide/Show Editor: Shift + H");
  }
  #ifndef WATER_GERSTNER
  if (ImGui::CollapsingHeader("Other"))
  {
    ImGui::DragFloat("Height Scale", &editor_height_scale, 0.01f);
    ImGui::DragFloat("Displace Scale", &editor_displace_scale, 0.01f);
  }
  WaterFFTHolder::GetWaterFFT()->m_HeightScale = editor_height_scale;
  WaterFFTHolder::GetWaterFFT()->m_DisplaceScale = editor_displace_scale;
  #endif // !WATER_GERSTNER
  ImGui::End();
}

class Simulation
{
public:
  Simulation() {}
  void Initialize(bool run_gerstner);
  void Clean();
  void Run(Camera * cam);
  bool gerstner;
  Water * water;
  WaterFFT * water_fft;
};

void Simulation::Initialize(bool run_gerstner)
{
  gerstner = run_gerstner;
  if(gerstner)
  {
    water = new Water(100, 100);
    WaterGerstnerRenderer::SetWater(water);
    WaterEditor::SetWater(water);
    WaterEditor::m_Show = true;
  }
  else
  {
    WaterFFTHolder::Initialize();
    water_fft = WaterFFTHolder::GetWaterFFT();
    const GLfloat * vbuff = (GLfloat *)water_fft->VertexBuffer();
    const GLuint * ibuff = (GLuint *)water_fft->IndexBuffer();
    const GLfloat * obuff = (GLfloat *)water_fft->OffsetBuffer();
    unsigned vbuff_sb = water_fft->VertexBufferSizeBytes();
    unsigned ibuff_sb = water_fft->IndexBufferSizeBytes();
    unsigned ibuff_s = water_fft->IndexBufferSize();
    unsigned obuff_sb = water_fft->OffsetBufferSizeBytes();
    unsigned obuff_s = water_fft->OffsetBufferSize();
    WaterRenderer::SetBuffers(vbuff, 
                              ibuff,
                              obuff,
                              vbuff_sb,
                              ibuff_sb,
                              ibuff_s, obuff_sb,
                              obuff_s);
    //water_fft->UseIntensityMap("intensity0.png");
    WaterFFTThread::Execute(Time::TotalTimeScaled);
  }
}

void Simulation::Clean()
{
  if(!gerstner)
  {
    WaterFFTThread::Terminate();
    WaterFFTHolder::Purge();
  }
}

void Simulation::Run(Camera * cam)
{
  if(gerstner)
  {
    if(editor_show)
      WaterEditor::DisplayEditor();
    water->Update();
    WaterGerstnerRenderer::Render();
  }
  else
  {
    WaterFFTThread::Wait();
    glm::mat4 projection = glm::perspective(glm::radians(90.0f),
      OpenGLContext::AspectRatio(), 0.1f, 1000.0f);
    WaterRenderer::Render(cam->Location(), projection, cam->WorldToCamera());
  }
}

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  try {

    WindowInit();
    ImGui_ImplSdlGL3_Init(Context::SDLWindow());
    Context::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);
    glEnable(GL_DEPTH_TEST);

    Camera * pcam = new Camera(); 
    CameraController controller(*pcam);
    controller.Update();

    glm::mat4 model();
    glm::vec3 light_position(1000.0f, 100.0f, -1000.0f); 
    vec3 clear_color(0.0f, 0.0f, 0.0f);

    Framer::Lock(60);

    Simulation water_sim;
    water_sim.Initialize(false);


// NOTES
// 373

    while (Context::KeepOpen()) 
    {
      Framer::Start();
      InitialUpdate();
      controller.Update();

      water_sim.Run(pcam);

      EditorUpdate();
      ImGui::Render();
      OpenGLContext::Swap();
      glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      Framer::End();
    }
    OpenGLContext::Purge();
    Context::Purge();
  }
  catch (const RootError & error) {
    ErrorLog::Write(error);
  }
  catch (Error & error) {
    error.Add("> UNCAUGHT ERROR");
    ErrorLog::Write(error);
  }
  return 0;
}
