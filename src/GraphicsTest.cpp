#include <GLM\glm\mat4x4.hpp>
#include <GLM\glm\gtc\matrix_transform.hpp>
#include <GLM\glm\gtc\type_ptr.hpp>

#include "OpenGLError.h"
#include "OpenGLContext.h"
#include "Context.h"
#include "Time.h"

#include "GraphicsTest.h"

namespace GraphicsTest
{
  GLuint light_vao_id;
  GLuint object_vao_id;
  GLuint vbo_id;
  GLuint ebo_id;

  Shader * object_shader;
  GLuint oa_position;
  GLuint oa_normal;
  GLuint ou_projection;
  GLuint ou_view;
  GLuint ou_model;
  GLuint ou_objectColor;
  GLuint ou_lightColor;
  GLuint ou_lightPosition;
  GLuint ou_cameraPosition;

  Shader * light_shader;
  GLuint la_position;
  GLuint lu_transform;
  GLuint lu_lightColor;

  glm::vec3 light_position;
  float light_move_speed;
  Camera camera;
  CameraController c_controller(camera);
} // !GraphicsTest


#include <iostream>
namespace GraphicsTest
{
void Initialize()
{
  object_shader = new Shader("Shader/object.vert", "Shader/object.frag");
  light_shader = new Shader("Shader/light.vert", "Shader/light.frag");
  
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};
  
  /*float vertices[24] = { -0.5f,  0.5f, -0.5f,
                          0.5f,  0.5f, -0.5f,
                         -0.5f,  0.5f,  0.5f,
                          0.5f,  0.5f,  0.5f,
                         -0.5f, -0.5f, -0.5f,
                          0.5f, -0.5f, -0.5f,
                         -0.5f, -0.5f,  0.5f,
                          0.5f, -0.5f,  0.5f };

  unsigned elements[36] = { 0, 2, 1,
                            1, 2, 3,
                            2, 6, 3,
                            3, 6, 7,
                            6, 4, 7,
                            4, 5, 7,
                            4, 0, 5,
                            5, 0, 1,
                            3, 7, 1,
                            7, 5, 1,
                            0, 4, 2,
                            4, 6, 2 };*/


  object_shader->Use();
  oa_position = glGetAttribLocation(object_shader->ID(), "APosition");
  oa_normal = glGetAttribLocation(object_shader->ID(), "ANormal");
  ou_projection = glGetUniformLocation(object_shader->ID(), "UProjection");
  ou_view = glGetUniformLocation(object_shader->ID(), "UView");
  ou_model = glGetUniformLocation(object_shader->ID(), "UModel");
  ou_objectColor = glGetUniformLocation(object_shader->ID(), "UObjectColor");
  ou_lightColor = glGetUniformLocation(object_shader->ID(), "ULightColor");
  ou_lightPosition = glGetUniformLocation(object_shader->ID(), "ULightPosition");
  ou_cameraPosition = glGetUniformLocation(object_shader->ID(), "UCameraPosition");

  light_shader->Use();
  la_position = glGetAttribLocation(light_shader->ID(), "APosition");
  lu_transform = glGetUniformLocation(light_shader->ID(), "UTransform");
  lu_lightColor = glGetUniformLocation(light_shader->ID(), "ULightColor");


  // object
  object_shader->Use();
  glGenVertexArrays(1, &object_vao_id);
  glGenBuffers(1, &vbo_id);
  glGenBuffers(1, &ebo_id);
  glBindVertexArray(object_vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  glVertexAttribPointer(oa_position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(oa_position);
  glVertexAttribPointer(oa_normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(oa_normal);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  //light
  light_shader->Use();
  glGenVertexArrays(1, &light_vao_id);
  glBindVertexArray(light_vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
  glVertexAttribPointer(la_position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(la_position);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  /*glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/

  glm::vec3 lc(1.0f, 1.0f, 1.0f);

  object_shader->Use();
  glUniform3f(ou_objectColor, 0.101, 0.815, 0.878);
  glUniform3f(ou_lightColor, lc.r, lc.g, lc.b);

  light_shader->Use();
  glUniform3f(lu_lightColor, lc.r, lc.g, lc.b);

  light_position = glm::vec3(1.2f, 1.0f, -2.0f);
  light_move_speed = 1.0f;

  GLenum gl_error = glGetError();
  try {
    OPENGLERRORCHECK("GraphicsTest.cpp", "Initialize", "Upload", gl_error);
  }
  catch (const Error & error) {
    ErrorLog::Write(error);
  }

}

void Input()
{
  if (Input::KeyDown(Key::O))
    light_position.y += light_move_speed * Time::DTScaled();
  if (Input::KeyDown(Key::U))
    light_position.y -= light_move_speed * Time::DTScaled();
  if (Input::KeyDown(Key::L))
    light_position.x += light_move_speed * Time::DTScaled();
  if (Input::KeyDown(Key::J))
    light_position.x -= light_move_speed * Time::DTScaled();
  if (Input::KeyDown(Key::I))
    light_position.z -= light_move_speed * Time::DTScaled();
  if (Input::KeyDown(Key::K))
    light_position.z += light_move_speed * Time::DTScaled();
}

void Update()
{
  c_controller.Update();
  Input();


  glm::mat4 projection = glm::perspective(glm::radians(120.0f), OpenGLContext::AspectRatio(), 0.1f, 100.0f);
  glm::mat4 camtondc(projection * camera.WorldToCamera());

  glm::mat4 light_tran = glm::translate(glm::mat4(), light_position);
  glm::mat4 transformation1(camtondc * light_tran);

  object_shader->Use();
  glUniformMatrix4fv(ou_projection, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(ou_view, 1, GL_FALSE, glm::value_ptr(camera.WorldToCamera()));
  glUniform3f(ou_lightPosition, light_position.x, light_position.y, light_position.z);
  const glm::vec3 & cam_loc = camera.Location();
  glUniform3f(ou_cameraPosition, cam_loc.x, cam_loc.y, cam_loc.z);
  glBindVertexArray(object_vao_id);
  //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);

  light_shader->Use();
  glUniformMatrix4fv(lu_transform, 1, GL_FALSE, glm::value_ptr(transformation1));
  glBindVertexArray(light_vao_id);
  //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);

}

} // !GraphicsTest
