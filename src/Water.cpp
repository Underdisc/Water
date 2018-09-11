//////////////////////////////////////////////////////////////////////////////
/// @file Water.cpp
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-07-15
///
/// @brief Contains the implementation of everything to do with water. This
/// includes the water itself, the rendering of the water, and the editing of
/// the water.
///////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <GL\glew.h>
#include <GLM\glm\mat4x4.hpp>
#include <GLM\glm\gtc\matrix_transform.hpp>
#include <GLM\glm\gtc\type_ptr.hpp>

#include "ext/imgui.h"
#include "ext/json.h"
#include "OpenGLError.h"
#include "Context.h"
#include "OpenGLContext.h"
#include "Error.h"
#include "Time.h"

#include "Water.h"

// S_WATER ///////////////////////////////////////////////////////////////////

// serialization names //
#define WATERFILEEXTENSION ".water"
#define XSTRIDEID "x_stride"
#define ZSTRIDEID "z_stride"
#define WAVESID "waves"
#define AMPID "amplitude"
#define STEEPID "steepness"
#define LENID "length"
#define SPEEDID "speed"
#define DIRID "direction"

// math constants //
#define EPSILON 1.0e-8
#define DELTA 1.0e-1
#define PI 3.14159265358979323846264338f
#define TWOPI 6.28318530718f

// static initializations //
unsigned Water::Wave::m_WavesCreated = 0;

//////////////////////////////////////////////////////////////////////////////
/// @brief Set the wave length of a specific wave on a water surface.
///
/// @param new_length The new length of the wave.
///////////////////////////////////////////////////////////////////////////////
inline void Water::Wave::SetWaveLength(float new_length)
{
  m_Wavelength = new_length;
  m_Frequency = TWOPI / m_Wavelength;
  m_PhaseConstant = m_WaveSpeed * m_Frequency;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Set the speed at which the wave propegates at.
///
/// @param new_speed The new speed of the wave.
///////////////////////////////////////////////////////////////////////////////
inline void Water::Wave::SetWaveSpeed(float new_speed)
{
  m_WaveSpeed = new_speed;
  m_PhaseConstant = m_WaveSpeed * m_Frequency;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Set the direction the wave is heading using a 2D vector.
///
/// @param x Magnitude in the x direction.
/// @param y Magnitude in the y direction.
///////////////////////////////////////////////////////////////////////////////
inline void Water::Wave::SetWaveDirection(float x, float y)
{
  m_WaveDirection = glm::normalize(glm::vec2(x, y));
  m_WaveDirectionRadians = acos(m_WaveDirection.x);
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Sets the wave direction given a value in radians.
///
/// @param radians The direction of the wave in radians.
///////////////////////////////////////////////////////////////////////////////
inline void Water::Wave::SetWaveDirection(float radians)
{
  m_WaveDirection.x = cos(radians);
  m_WaveDirection.y = -sin(radians);
  m_WaveDirectionRadians = radians;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Constuctor for a Wave. The default values for waves can be found
/// in the initializer list.
///////////////////////////////////////////////////////////////////////////////
Water::Wave::Wave() :
  m_Active(true), m_Amplitude(1.0f), m_Steepness(0.5f),
  m_Wavelength(10.0f), m_WaveSpeed(1.0f), m_WaveDirection(1.0f, 0.0f),
  m_WaveDirectionRadians(0.0f), m_Frequency(TWOPI / m_Wavelength),
  m_PhaseConstant(m_WaveSpeed * m_Frequency)
{
  // setting unique wave name
  ++m_WavesCreated;
  char initial_name[NAMEBUFFERSIZE];
  #ifdef _WIN32
    sprintf_s(initial_name, "Wave %d", m_WavesCreated);
  #else
    sprintf(initial_name, "Wave %d", m_WavesCreated);
  #endif
  m_Name = initial_name;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Wave constructor for a precomputed wave. This is used to load in
/// the serialized waves.
///
/// @param name The name of the wave.
/// @param amplitude The amplitude of the wave.
/// @param steepness The steepness or pinchiness of the wave.
/// @param length The wave length.
/// @param speed The wave speed.
/// @param direction The direction the wave travels in.
///////////////////////////////////////////////////////////////////////////////
Water::Wave::Wave(const std::string & name, float amplitude, float steepness,
  float length, float speed, const glm::vec2 & direction) :
  m_Active(true), m_Name(name), m_Amplitude(amplitude),
  m_Steepness(steepness), m_Wavelength(length), m_WaveSpeed(speed),
  m_WaveDirection(direction), m_WaveDirectionRadians(acos(m_WaveDirection.x)),
  m_Frequency(TWOPI / m_Wavelength),
  m_PhaseConstant(m_WaveSpeed * m_Frequency)
{}

//////////////////////////////////////////////////////////////////////////////
/// @brief The most complex part of all of this. This will calculate a
/// vertices offset and normal for its base position (x, y, 0). In the water
/// class, the z value is used as the horizontal value instead of y. The
/// first value in the pair is the offset, and the second value is the normal
/// at that vertex. The math behind this can be found in the nVidia Gerstner
/// waves gpu gems article.
///
/// @param x The vertices base x position.
/// @param y The vertices base y position.
/// @param t The time which the offset is being calculated for.
///
/// @return The offset and normal for a given vertex.
///////////////////////////////////////////////////////////////////////////////
inline std::pair<glm::vec3,glm::vec3> Water::Wave::OffsetNormal(float x,
                                                          float y, float t)
{
  // declarations
  std::pair<glm::vec3, glm::vec3> offset_normal;
  glm::vec2 grid_position(x, y);
  // finding repeated values
  float dot_result = glm::dot(m_WaveDirection, grid_position);
  float trig_eval = m_Frequency * dot_result+ m_PhaseConstant * t;
  float sin_result = sin(trig_eval);
  float cos_result = cos(trig_eval);
  float horizontal_product = m_Steepness * m_Amplitude * cos_result;
  // calculation offsets
  offset_normal.first.x = horizontal_product * m_WaveDirection.x;
  offset_normal.first.y = m_Amplitude * sin_result;
  offset_normal.first.z = horizontal_product * m_WaveDirection.y;
  // removing repeated multiplication
  float freq_amp = m_Frequency * m_Amplitude;
  // calculating vertex normals
  offset_normal.second.x = m_WaveDirection.x * freq_amp * cos_result;
  offset_normal.second.y = m_Steepness * freq_amp * sin_result;
  offset_normal.second.z = m_WaveDirection.y * freq_amp * cos_result;
  return offset_normal;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Creates a Water surface with a given x and z stride. The base
/// vertices will be located at positive interger values on the x and z
/// axis.
///
/// @param x_stride The number of vertices along the x axis.
/// @tparam z_stride The number of vertices along the y axis.
///////////////////////////////////////////////////////////////////////////////
Water::Water(unsigned x_stride, unsigned z_stride) :
  m_XStride(x_stride), m_ZStride(z_stride),
  m_NumVerts(x_stride * z_stride)
{
  PrepareVertexData();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Creates a Water surface using the given config file.
///
/// @param config_file The name of the Water file that is being loaded.
///////////////////////////////////////////////////////////////////////////////
Water::Water(const std::string & config_file) :
  m_XStride(0), m_ZStride(0), m_NumVerts(0)
{
  OpenConfig(config_file);
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Opens a Water configuration file and sets the Water's wave 
/// parameters according to what is in the configuration file.
///
/// @param config_file The name of the Water configuration file being opened.
///////////////////////////////////////////////////////////////////////////////
void Water::OpenConfig(const std::string & config_file)
{
  Json::Value json_water;
  std::ifstream water_stream(config_file.c_str());
  if (!water_stream.is_open()) {
    // failed to open
    Error error("Water.cpp", "OpenConfig");
    error.Add("File would not open");
    error.Add("> FILENAME");
    error.Add(config_file);
    throw(error);
  }
  water_stream >> json_water;
  // Preparing the vertex data.
  m_XStride = json_water[XSTRIDEID].asUInt();
  m_ZStride = json_water[ZSTRIDEID].asUInt();
  m_NumVerts = m_XStride * m_ZStride;
  PrepareVertexData();
  // Filling GPU buffers with new water data
  WaterGerstnerRenderer::ResetBuffers();
  // Clearing old waves
  m_Waves.clear();
  // Adding waves from configuration file
  const Json::Value & json_waves = json_water["waves"];
  const std::vector<std::string> & wave_names = json_waves.getMemberNames();
  for (const std::string & wave_name : wave_names) {
    const Json::Value & json_wave = json_waves[wave_name];
    float amplitude = json_wave[AMPID].asFloat();
    float steepness = json_wave[STEEPID].asFloat();
    float length = json_wave[LENID].asFloat();
    float speed = json_wave[SPEEDID].asFloat();
    glm::vec2 direction;
    direction.x = json_wave[DIRID][0].asFloat();
    direction.y = json_wave[DIRID][1].asFloat();
    direction = glm::normalize(direction);
    m_Waves.push_back(Wave(wave_name, amplitude, steepness,
                           length, speed, direction));
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Exports a configuration file using the settings that are currently
/// being used on the water.
///
/// @param filename The name of the file that the configuration will be
/// written to.
///////////////////////////////////////////////////////////////////////////////
void Water::ExportConfig(const std::string & filename)
{
  Json::Value water;
  // exporting water dimensions
  water[XSTRIDEID] = m_XStride;
  water[ZSTRIDEID] = m_ZStride;
  // exporting waves
  Json::Value & waves = water["waves"];
  for (const Wave & wave : m_Waves) {
    Json::Value json_wave;
    json_wave[AMPID] = wave.m_Amplitude;
    json_wave[STEEPID] = wave.m_Steepness;
    json_wave[LENID] = wave.m_Wavelength;
    json_wave[SPEEDID] = wave.m_WaveSpeed;
    Json::Value direction(Json::arrayValue);
    direction.append(Json::Value(wave.m_WaveDirection.x));
    direction.append(Json::Value(wave.m_WaveDirection.y));
    json_wave[DIRID] = direction;
    waves[wave.m_Name] = json_wave;
  }
  std::ofstream out_stream(filename.c_str());
  out_stream << water;
  out_stream.close();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Adds a wave to the water surface.
///
/// @return A pointer to the Wave that was created.
///////////////////////////////////////////////////////////////////////////////
Water::Wave * Water::AddWave()
{
  m_Waves.push_back(Wave());
  return &m_Waves.back();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Removes a Wave from the water surface.
///
/// @param wave The wave that is to be removed from the water surface.
///
/// @return If the wave was successfully found and removed, true.
///////////////////////////////////////////////////////////////////////////////
bool Water::RemoveWave(Wave * wave)
{
  std::vector<Wave>::iterator it = m_Waves.begin();
  std::vector<Wave>::iterator it_e = m_Waves.end();
  for (; it != it_e; ++it) {
    if (&(*it) == wave) {
      m_Waves.erase(it);
      return true;
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates the Water surface.
///////////////////////////////////////////////////////////////////////////////
void Water::Update()
{
  UpdateGerstner();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Gets the full summated offset values for a specific vertex
/// location and time.
///
/// @param x The base x location.
/// @param z The base z location.
/// @param t The current time the offset is being calculated for.
///////////////////////////////////////////////////////////////////////////////
std::pair<glm::vec3, glm::vec3> Water::GetFullOffset(float x, float z, float t)
{
  std::pair<glm::vec3, glm::vec3> full_offset((0.0f, 0.0f, 0.0f),
    (0.0f, 0.0f, 0.0f));
  // performing a summation on the offsets for all waves
  for (Wave & wave : m_Waves) {
    if (wave.m_Active) {
      std::pair<glm::vec3, glm::vec3> offset =
        wave.OffsetNormal(x, z, t);
      full_offset.first += offset.first;
      full_offset.second += offset.second;
    }
  }
  return full_offset;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates the vertex locations and normals in the Water simulation.
/// This also includes some of the math from the NVidia article on Gerstner
/// waves.
///////////////////////////////////////////////////////////////////////////////
inline void Water::UpdateGerstner()
{
  // normal drawing
  //WaterRenderer::ClearLines();
  unsigned vertex_index = 0;
  for (unsigned z = 0; z < m_ZStride; ++z) {
    for (unsigned x = 0; x < m_XStride; ++x) {
      std::pair<glm::vec3, glm::vec3> offset;
      offset = GetFullOffset((float)x, (float)z, Time::TotalTimeScaled());
      // setting the new vertex location
      m_VertexData[vertex_index].m_X = (float)x + offset.first.x;
      m_VertexData[vertex_index].m_Y = offset.first.y;
      m_VertexData[vertex_index].m_Z = (float)z + offset.first.z;
      // setting the new normal value
      unsigned normal_index = vertex_index + m_NumVerts;
      m_VertexData[normal_index].m_X = -offset.second.x;
      m_VertexData[normal_index].m_Y = 1.0f - offset.second.y;
      m_VertexData[normal_index].m_Z = -offset.second.z;
      ++vertex_index;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Initializes the vector that will be used as the vertex buffer for
/// the water surface.
///
/// @par Important Notes
/// - Make sure that m_XStride, m_ZStride, and m_NumVerts are set before
///    calling this function
///////////////////////////////////////////////////////////////////////////////
void Water::PrepareVertexData()
{
  // clearing vertex data
  m_VertexData.clear();
  // finding vertices
  m_VertexData.reserve(m_NumVerts);
  // constraints
  float x_min = 0.0f;
  float x_max = m_XStride - 1.0f;
  float z_min = 0.0f;
  float z_max = m_ZStride - 1.0f;
  // starting values
  float x = x_min;
  float y = 0.0;
  float z = z_min;
  for (unsigned i = 0; i < m_NumVerts; ++i) {
    // Epsilon to avoid floating point error
    if (x >(x_max + EPSILON)) {
      x = x_min;
      z += 1.0f;
    }
    m_VertexData.push_back(Vertex(x, y, z));
    x += 1.0f;
  }
  // initial normal values
  for (unsigned i = 0; i < m_NumVerts; ++i) {
    m_VertexData.push_back(Vertex(0.0f, 1.0f, 0.0f));
  }
}

// S_WATERRENDERER ///////////////////////////////////////////////////////////

// static initializations
glm::vec3 WaterGerstnerRenderer::m_WaterColor = glm::vec3(0.0f, 0.5f, 1.0f);
float WaterGerstnerRenderer::m_AmbientFactor = 0.2f;
glm::vec3 WaterGerstnerRenderer::m_AmbientColor = glm::vec3(0.160f, 0.909f, 0.960f);
glm::vec3 WaterGerstnerRenderer::m_DiffuseColor = glm::vec3(0.160f, 0.909f, 0.960f);
float WaterGerstnerRenderer::m_SpecularFactor = 1.0f;
int WaterGerstnerRenderer::m_SpecularExponent = 20;
glm::vec3 WaterGerstnerRenderer::m_SpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
float WaterGerstnerRenderer::m_AlphaMinuend = 1.0f;
bool WaterGerstnerRenderer::m_WaterSet = false;
Water * WaterGerstnerRenderer::m_Water = nullptr;
WaterGerstnerRenderer::WaterShader * WaterGerstnerRenderer::m_WaterShader = nullptr;
WaterGerstnerRenderer::LineShader * WaterGerstnerRenderer::m_LineShader = nullptr;
GLuint WaterGerstnerRenderer::m_VBOID = -1;
GLuint WaterGerstnerRenderer::m_EBOID = -1;
GLuint WaterGerstnerRenderer::m_VAOID = -1;
GLuint WaterGerstnerRenderer::m_LineVBOID = -1;
unsigned WaterGerstnerRenderer::m_NumIndices = 0;
bool WaterGerstnerRenderer::m_LineDraw = false;
std::vector<WaterGerstnerRenderer::Line> WaterGerstnerRenderer::m_Lines = std::vector<Line>();
Camera WaterGerstnerRenderer::m_Camera = Camera();
CameraController WaterGerstnerRenderer::m_Controller = CameraController(m_Camera);


//////////////////////////////////////////////////////////////////////////////
/// @brief Creates the WaterShader shader type.
///////////////////////////////////////////////////////////////////////////////
WaterGerstnerRenderer::WaterShader::WaterShader() :
  Shader("Shader/water.vert", "Shader/water.frag")
{
  // finding attribute and uniform locations
  this->Use();
  m_APosition = GetAttribLocation("APosition");
  m_ANormal = GetAttribLocation("ANormal");
  m_UTransform = GetUniformLocation("UTransform");
  m_UWaterColor = GetUniformLocation("UWaterColor");
  m_UAmbientFactor = GetUniformLocation("UAmbientFactor");
  m_UAmbientColor = GetUniformLocation("UAmbientColor");
  m_UDiffuseColor = GetUniformLocation("UDiffuseColor");
  m_USpecularFactor = GetUniformLocation("USpecularFactor");
  m_USpecularExponent = GetUniformLocation("USpecularExponent");
  m_USpecularColor = GetUniformLocation("USpecularColor");
  m_UAlphaMinuend = GetUniformLocation("UAlphaMinuend");
  m_ULightDirection = GetUniformLocation("ULightDirection");
  m_UCameraPosition = GetUniformLocation("UCameraPosition");
  m_UTime = GetUniformLocation("UTime");
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Creates the shader that is used for drawing lines.
///////////////////////////////////////////////////////////////////////////////
WaterGerstnerRenderer::LineShader::LineShader() :
  Shader("Shader/line.vert", "Shader/line.frag")
{
  this->Use();
  // finding attribute and uniforms
  m_APosition = GetAttribLocation("APosition");
  m_UTransform = GetUniformLocation("UTransform");
  m_UColor = GetUniformLocation("UColor");
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Sets the Water that will be rendered when the Render function
/// is called.
///
/// @param water The water that will be used for rendering.
///////////////////////////////////////////////////////////////////////////////
void WaterGerstnerRenderer::SetWater(Water * water)
{
  m_Water = water;
  if (!m_WaterSet){
    // first water
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_WaterSet = true;
    m_WaterShader = new WaterShader();
    m_LineShader = new LineShader();
    PrepareBuffers();
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Renders the Water that the WaterRenderer is currently set to
/// Render.
///////////////////////////////////////////////////////////////////////////////
void WaterGerstnerRenderer::Render()
{
  // make sure water is set
  if (!m_WaterSet) {
    Error error("Water.cpp", "WaterGerstnerRenderer::Render");
    error.Add("Set the Water with WaterGerstnerRenderer::SetWater before calling"
              "WaterGerstnerRenderer::Render");
    throw(error);
  }
  ManageInput();

  // changing the water vertex data on the gpu
  glBindBuffer(GL_ARRAY_BUFFER, m_VBOID);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 
    sizeof(Water::Vertex) * m_Water->m_VertexData.size(), 
    m_Water->m_VertexData.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // finding mesh transformation
  glm::mat4 projection = glm::perspective(glm::radians(90.0f), 
    OpenGLContext::AspectRatio(), 0.1f, 100.0f);
  glm::mat4 transformation(projection * m_Camera.WorldToCamera());
  m_WaterShader->Use();
  // getting camera location
  glm::vec3 location = m_Camera.Location();
  // setting uniforms
  glUniformMatrix4fv(m_WaterShader->m_UTransform, 1, GL_FALSE, 
    glm::value_ptr(transformation));
  glUniform3f(m_WaterShader->m_UWaterColor,
    m_WaterColor.x, m_WaterColor.y, m_WaterColor.z);

  glUniform1f(m_WaterShader->m_UAmbientFactor, m_AmbientFactor);
  glUniform3f(m_WaterShader->m_UAmbientColor,
    m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z);
  glUniform3f(m_WaterShader->m_UDiffuseColor,
    m_DiffuseColor.x, m_DiffuseColor.y, m_DiffuseColor.z);
  glUniform1f(m_WaterShader->m_USpecularFactor, m_SpecularFactor);
  glUniform1i(m_WaterShader->m_USpecularExponent, m_SpecularExponent);
  glUniform3f(m_WaterShader->m_USpecularColor,
    m_SpecularColor.x, m_SpecularColor.y, m_SpecularColor.z);
  glUniform1f(m_WaterShader->m_UAlphaMinuend, m_AlphaMinuend);

  glUniform3f(m_WaterShader->m_UCameraPosition, 
    location.x, location.y, location.z);
  glUniform1f(m_WaterShader->m_UTime, Time::TotalTimeScaled());
  // rendering water
  glBindVertexArray(m_VAOID);
  if (m_LineDraw) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  else
    glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  // rendering lines
  m_LineShader->Use();
  glUniformMatrix4fv(m_LineShader->m_UTransform, 1, GL_FALSE, glm::value_ptr(transformation));
  glBindBuffer(GL_ARRAY_BUFFER, m_LineVBOID);
  std::vector<Line>::const_iterator cit = m_Lines.begin();
  std::vector<Line>::const_iterator cit_e = m_Lines.end();
  for (; cit != cit_e; ++cit) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Line), &(*cit));
    glDrawArrays(GL_LINES, 0, 2);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // performing error check
  GLenum gl_error = glGetError();
  try {
    OPENGLERRORCHECK("Water.cpp", "WaterGerstnerRenderer::Render", "Water Rendering", gl_error);
  }
  catch (const Error & error) {
    ErrorLog::Write(error);
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Deletes all of the GPU buffers that are being used to Render Water
/// and reinitializes them. This is meant for when the size of the Water's
/// grid changes in size.
///////////////////////////////////////////////////////////////////////////////
void WaterGerstnerRenderer::ResetBuffers()
{
  // deleting
  glDeleteBuffers(1, &m_VBOID);
  glDeleteBuffers(1, &m_EBOID);
  glDeleteVertexArrays(1, &m_VAOID);
  // reinitialization
  PrepareBuffers();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Initializes the GPU buffers that are used to Render the water. This
/// will also fill those buffers with the current Water mesh vertex data.
///////////////////////////////////////////////////////////////////////////////
void WaterGerstnerRenderer::PrepareBuffers()
{
  // Finding indicies
  std::vector<Water::Triangle> indices;
  unsigned limit = m_Water->m_NumVerts - m_Water->m_XStride;
  for (unsigned int i = 0; i < limit;) {
    indices.push_back(Water::Triangle(i, i + 1, i + m_Water->m_XStride));
    ++i;
    indices.push_back(Water::Triangle(i, i + m_Water->m_XStride, i + m_Water->m_XStride - 1));
    unsigned vertices_left = (i + 1) % m_Water->m_XStride;
    if (vertices_left == 0)
      ++i;
  }
  m_NumIndices = indices.size() * 3;
  // water buffers //
  glGenVertexArrays(1, &m_VAOID);
  glGenBuffers(1, &m_VBOID);
  glGenBuffers(1, &m_EBOID);
  glBindVertexArray(m_VAOID);
  // buffer data
  glBindBuffer(GL_ARRAY_BUFFER, m_VBOID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Water::Vertex) * m_Water->m_VertexData.size(), m_Water->m_VertexData.data(), GL_STREAM_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Water::Triangle) * indices.size(), indices.data(), GL_STATIC_DRAW);
  // attributes
  glVertexAttribPointer(m_WaterShader->m_APosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(m_WaterShader->m_APosition);
  glVertexAttribPointer(m_WaterShader->m_ANormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(3 * m_Water->m_NumVerts * sizeof(GL_FLOAT)));
  glEnableVertexAttribArray(m_WaterShader->m_ANormal);
  // unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // line buffer //
  glGenBuffers(1, &m_LineVBOID);
  glBindBuffer(GL_ARRAY_BUFFER, m_LineVBOID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Line), m_Lines.data(), GL_STREAM_DRAW);
  glVertexAttribPointer(m_LineShader->m_APosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(m_LineShader->m_APosition);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // error checking //
  GLenum gl_error = glGetError();
  try {
    OPENGLERRORCHECK("Water.cpp", "Water", "Vertex Construction", gl_error);
  }
  catch (const Error & error) {
    ErrorLog::Write(error);
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Adds a line to the vector of Line objects that will be drawn
/// when WaterRenderer::Render is called.
///
/// @param start The starting point of the line.
/// @tparam end The endind point of the line.
///////////////////////////////////////////////////////////////////////////////
void WaterGerstnerRenderer::AddLine(const glm::vec3 & start, const glm::vec3 & end)
{
  m_Lines.push_back(Line(start, end));
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Clears all of the Line objects that are in the vector of Lines so
/// they are not drawn on the next call to WaterRenderer::Render.
///////////////////////////////////////////////////////////////////////////////
void WaterGerstnerRenderer::ClearLines()
{
  m_Lines.clear();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Handles input that will affect the WaterRenderer.
///////////////////////////////////////////////////////////////////////////////
inline void WaterGerstnerRenderer::ManageInput()
{
  m_Controller.Update();
  if (Input::KeyPressed(Key::SPACE)) {
    m_LineDraw = !m_LineDraw;
  }
}

// S_WATEREDITOR /////////////////////////////////////////////////////////////

// static initialization
bool WaterEditor::m_Show = false;
Water * WaterEditor::m_Water = nullptr;
bool WaterEditor::m_ChangingWaterColor = false;
bool WaterEditor::m_ChangingLightColor = false;
Water::Wave * WaterEditor::m_EditingName = nullptr;
char WaterEditor::m_NewWaveName[NAMEBUFFERSIZE] = { "\0" };
bool WaterEditor::m_Exporting = false;
bool WaterEditor::m_Opening = false;
char WaterEditor::m_FileName[WAVEFILEBUFFERSIZE] = { "\0" };

//////////////////////////////////////////////////////////////////////////////
/// @brief Set the Water instance that will be used by the WaterEditor to
/// display values related to the water and edit them.
///
/// @param water The Water instance that the editor will use.
///////////////////////////////////////////////////////////////////////////////
void WaterEditor::SetWater(Water * water)
{
  m_Water = water;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Displays the WaterEditor so a user can edit the Water's properties.
///////////////////////////////////////////////////////////////////////////////
void WaterEditor::DisplayEditor()
{
  // make sure water is set
  if (!m_Water) {
    RootError error("Water.cpp", "WaterEditor::Update");
    error.Add("WaterEditor does not have a pointer to the Water"
      "being edited. Use WaterEditor::SetWater to set Water.");
    throw(error);
  }
  if (!m_Show)
    return;
  // main window //
  ImGui::Begin("Water", &m_Show, ImGuiWindowFlags_MenuBar);
  MenuBar();
  // adding and displaying waves
  if (ImGui::Button("New Wave"))
    m_Water->AddWave();
  for (Water::Wave & wave : m_Water->m_Waves)
    DisplayWave(wave);
  ImGui::End();
  // other windows //
  if (m_ChangingWaterColor)
    ChangeWaterColor();
  if (m_ChangingLightColor)
    ChangeLightColor();
  if (m_EditingName)
    EditWaveName();
  if (m_Opening)
    OpenWindow();
  if (m_Exporting)
    ExportWindow();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Displays a Menu bar for importing and exporting options.
///////////////////////////////////////////////////////////////////////////////
inline void WaterEditor::MenuBar()
{
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open"))
        m_Opening = true;
      if (ImGui::MenuItem("Export"))
        m_Exporting = true;
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Color")) {
      if (ImGui::MenuItem("Water"))
        m_ChangingWaterColor = true;
      if (ImGui::MenuItem("Light"))
        m_ChangingLightColor = true;
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Displays a wave as a tree node in the water editor. The node can be
/// expanded in order to edit specific properties of the wave.
///
/// @param wave The wave that will be displayed as a tree node.
///////////////////////////////////////////////////////////////////////////////
inline void WaterEditor::DisplayWave(Water::Wave & wave)
{
  if (ImGui::TreeNode(wave.m_Name.c_str())) {
    // main wave properties
    ImGui::Checkbox("Active", &wave.m_Active);
    ImGui::DragFloat("Amplitude", &wave.m_Amplitude, 0.05f, 0.0f);
    ImGui::DragFloat("Steepness", &wave.m_Steepness, 0.01f);
    float new_length = wave.m_Wavelength;
    ImGui::DragFloat("Length", &new_length, 0.05f, 0.0f);
    wave.SetWaveLength(new_length);
    float new_speed = wave.m_WaveSpeed;
    ImGui::DragFloat("Speed", &new_speed, 0.05f, 0.0f);
    wave.SetWaveSpeed(new_speed);
    float new_direction = wave.m_WaveDirectionRadians;
    ImGui::DragFloat("Direction", &new_direction, 0.01f);
    wave.SetWaveDirection(new_direction);
    // name a remove
    if (ImGui::Button("Change Name"))
      m_EditingName = &wave;
    if (ImGui::Button("Remove"))
      m_Water->RemoveWave(&wave);
    ImGui::TreePop();
  }
}

inline void WaterEditor::ChangeColor(glm::vec3 * color)
{
  ImGui::SliderFloat("R", &(color->r), 0.0f, 1.0f);
  ImGui::SliderFloat("G", &(color->g), 0.0f, 1.0f);
  ImGui::SliderFloat("B", &(color->b), 0.0f, 1.0f);
}

inline void WaterEditor::ChangeWaterColor()
{
  ImGui::Begin("Water Color");
  ChangeColor(&WaterGerstnerRenderer::m_WaterColor);
  //ImGui::SliderFloat("Alpha Minuend", &WaterGerstnerRenderer::m_AlphaMinuend, 0.0f, 2.0f);
  if (ImGui::Button("Done"))
    m_ChangingWaterColor = false;
  ImGui::End();
}

inline void WaterEditor::ChangeLightColor()
{
  ImGui::Begin("Light Color");
  if (ImGui::TreeNode("Ambient")) {
    ImGui::SliderFloat("Factor", &WaterGerstnerRenderer::m_AmbientFactor, 0.0f, 1.0f);
    ImGui::Text("Color");
    ChangeColor(&WaterGerstnerRenderer::m_AmbientColor);
    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Diffuse")) {
    ImGui::Text("Color");
    ChangeColor(&WaterGerstnerRenderer::m_DiffuseColor);
    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Specular")) {
    ImGui::SliderFloat("Factor", &WaterGerstnerRenderer::m_SpecularFactor, 0.0f, 1.0f);
    ImGui::SliderInt("Exponent", &WaterGerstnerRenderer::m_SpecularExponent, 0, 100);
    ImGui::Text("Color");
    ChangeColor(&WaterGerstnerRenderer::m_SpecularColor);
  }
  if (ImGui::Button("Done"))
    m_ChangingLightColor = false;
  ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Creates an Imgui window that is used for changing a Wave's name.
///////////////////////////////////////////////////////////////////////////////
inline void WaterEditor::EditWaveName()
{
  ImGui::Begin(m_EditingName->m_Name.c_str());
  ImGui::InputText("", m_NewWaveName, NAMEBUFFERSIZE);
  if (ImGui::Button("Ok")) {
    m_EditingName->m_Name = m_NewWaveName;
    m_NewWaveName[0] = '\0';
    m_EditingName = nullptr;
  }
  ImGui::TextWrapped("Warning: Same names will cause bugs in"
                     "the water editor");
  ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Creates an Impui window that is used for opening specific Water
/// configuration files.
///////////////////////////////////////////////////////////////////////////////
inline void WaterEditor::OpenWindow()
{
  ImGui::Begin("Open", &m_Opening);
  ImGui::TextWrapped("File Name");
  ImGui::InputText(WATERFILEEXTENSION, m_FileName, WAVEFILEBUFFERSIZE);
  if (ImGui::Button("Ok")) {
    std::string config_file(m_FileName);
    config_file.append(WATERFILEEXTENSION);
    m_Water->OpenConfig(config_file);
    m_FileName[0] = '\0';
    m_Opening = false;
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel"))
    m_Opening = false;
  ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Creates the Imgui window that is used when a Water configuration
/// file is being exported.
///////////////////////////////////////////////////////////////////////////////
inline void WaterEditor::ExportWindow()
{
  ImGui::Begin("Export", &m_Exporting);
  ImGui::TextWrapped("File Name");
  ImGui::InputText(".water", m_FileName, WAVEFILEBUFFERSIZE);
  if (ImGui::Button("Ok")) {
    std::string file_name(m_FileName);
    file_name.append(".water");
    m_Water->ExportConfig(file_name);
    m_Exporting = false;
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel"))
    m_Exporting = false;
  ImGui::End();
}
