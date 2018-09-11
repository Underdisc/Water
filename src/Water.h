//////////////////////////////////////////////////////////////////////////////
/// @file Water.h
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-07-12
///
/// @brief Interface for the Water class and various other classes that
/// interact with the Water.
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <utility>

#include "Shader.h"
#include "Camera.h"
#include "CameraController.h"

// Pre-declarations
class WaterGerstnerRenderer;
class WaterEditor;

// Defines
#define NAMEBUFFERSIZE 50
#define WAVEFILEBUFFERSIZE 50

// S_WATER ///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/// @brief Simulates a water surface using a gerstner wave implementation.
/// Typical water surfaces will be made up of multiple gerstner waves. The
/// interface and implementation code for a gerstner wave can be found in the
/// Wave class within the Water class.
///
/// @par Important Notes
/// - The x_stride and z_stride parameters for the water are the number of
///   vertices in the x and z direction. For example, (1, 1) will give you one
///   vertex, so the water would just be a point.
///
//////////////////////////////////////////////////////////////////////////////
class Water
{
public:
  ////////////////////////////////////////////////////////////////////////////
  /// @brief A wave that can exist on a water surface. The water surface will
  /// typically be composed of multipe waves fading in and out in order to
  /// give the proper effect.
  ////////////////////////////////////////////////////////////////////////////
  class Wave
  {
  public:
    void SetWaveLength(float new_length);
    void SetWaveSpeed(float new_speed);
    void SetWaveDirection(float x, float y);
    void SetWaveDirection(float radians);
    //! Detemines if the wave is active or not. If it is not active, the
    // values returned by Offset will all be 0.
    bool m_Active;
    //! The name of the wave.
    std::string m_Name;
    //! The amplitude of the wave.
    float m_Amplitude;
    //! The steepness of the wave (a.k.a. peak sharpness)
    float m_Steepness;
  private:
    Wave();
    Wave(const std::string & name,float amplitude, float steepness,
         float length, float speed, const glm::vec2 & direction);
    std::pair<glm::vec3, glm::vec3> OffsetNormal(float x, float y, float t);
    //! The wave's lenght.
    float m_Wavelength;
    //! The wave's speed.
    float m_WaveSpeed;
    //! The direction the wave travels in.
    glm::vec2 m_WaveDirection;
    //! The direction of the wave in radians.
    float m_WaveDirectionRadians;
    //! The frequency of the wave.
    float m_Frequency;
    //! The wave phase constant.
    float m_PhaseConstant;
    //! The number of waves that have been created.
    static unsigned m_WavesCreated;
    //! Friending the water class so Water can modify waves.
    friend Water;
    //! Friending the WaterEditor so it can also modify waves.
    friend WaterEditor;
  };
private:
  ////////////////////////////////////////////////////////////////////////////
  /// @brief A tightly packed structure for storing vertex data that will be
  /// sent to the gpu. This will also store the normal vectors for each 
  /// vertex.
  ////////////////////////////////////////////////////////////////////////////
  struct Vertex
  {
    Vertex(float x, float y, float z) : m_X(x), m_Y(y), m_Z(z) {}
    void Zero() { m_X = 0.0f; m_Y = 0.0f; m_Z = 0.0f; }
    //! X value.
    float m_X;
    //! Y value.
    float m_Y;
    //! Z value.
    float m_Z;
  };
  ////////////////////////////////////////////////////////////////////////////
  /// @brief A tightly packed structure used for storing the vertex indicies
  /// for every triangle that exists on the water surface. The indicies 1, 2,
  /// and 3 will be stored in a counter clockwise order respectively.
  ////////////////////////////////////////////////////////////////////////////
  struct Triangle
  {
    Triangle(unsigned int index1, unsigned int index2, unsigned int index3):
    m_Index1(index1), m_Index2(index2), m_Index3(index3) {}
    //! First vertex index.
    unsigned int m_Index1;
    //! Second vertex index.
    unsigned int m_Index2;
    //! Third vertex index.
    unsigned int m_Index3;
  };
public:
  Water(unsigned x_stride, unsigned z_stride);
  Water(const std::string & config_file);
  void OpenConfig(const std::string & config_file);
  void ExportConfig(const std::string & config_file);
  Wave * AddWave();
  bool RemoveWave(Wave * wave);
  void Update();
private:
  std::pair<glm::vec3, glm::vec3> GetFullOffset(float x, float z, float t);
  void UpdateGerstner();
  void PrepareVertexData();
  //! The number of vertices in the x direction.
  unsigned m_XStride;
  //! The number of vertices in the z direction.
  unsigned m_ZStride;
  //! The total number of vertices in the water surface.
  unsigned int m_NumVerts;
  //! The vertex data. The first half of this are the vertex locations. The
  // second half are the normal vectors. The first normal vector corresponds
  // to the first vertex location and so forth. The number of vertices stored
  // is equivalent to m_NumVerts. The same is true for the number of normals.
  // Therefore the size of this is 2 * m_NumVerts.
  std::vector<Vertex> m_VertexData;
  //! All of the waves that are currently being simulated on the water 
  // surface.
  std::vector<Wave> m_Waves;
  //! Giving the WaterRenderer access to the Water.
  friend WaterGerstnerRenderer;
  //! Giving the WaterEditor access to the Water.
  friend WaterEditor;
};

// S_WATERRENDERER ///////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/// @brief Responsible for rendering the water created from an instance of the
/// water class. This is specifically for OpenGL use.
//////////////////////////////////////////////////////////////////////////////
class WaterGerstnerRenderer
{
private:
  ////////////////////////////////////////////////////////////////////////////
  /// @brief The GLSL shader that is used for drawing the Water surface.
  ////////////////////////////////////////////////////////////////////////////
  class WaterShader : public Shader
  {
  public:
    WaterShader();
    //! The Position attribute location.
    GLuint m_APosition;
    //! The Normal attribute location.
    GLuint m_ANormal;
    //! The Transformation uniform location.
    GLuint m_UTransform;
    //! The WaterColor uniform location.
    GLuint m_UWaterColor;

    GLuint m_UAmbientFactor;
    GLuint m_UAmbientColor;
    GLuint m_UDiffuseColor;
    GLuint m_USpecularFactor;
    GLuint m_USpecularExponent;
    GLuint m_USpecularColor;
    GLuint m_UAlphaMinuend;
    //! The LightDirection uniform location.
    GLuint m_ULightDirection;
    //! The CameraPosition uniform location.
    GLuint m_UCameraPosition;
    //! The Time uniform location.
    GLuint m_UTime;
  };
  ////////////////////////////////////////////////////////////////////////////
  /// @brief The GLSL shader that is used for drawing a line.
  ////////////////////////////////////////////////////////////////////////////
  class LineShader : public Shader
  {
  public:
    LineShader();
    //! The APosition attribute location.
    GLuint m_APosition;
    //! The UTransform uniform location.
    GLuint m_UTransform;
    //! The UCollor uniform location.
    GLuint m_UColor;
  };
  ////////////////////////////////////////////////////////////////////////////
  /// @brief This will store the data that is sent to the gpu for line
  /// drawing.
  ////////////////////////////////////////////////////////////////////////////
  struct Line
  {
  public:
    Line(const glm::vec3 & a, const glm::vec3 & b) :
      m_Ax(a.x), m_Ay(a.y), m_Az(a.z), m_Bx(b.x), m_By(b.y), m_Bz(b.z) {}
    //! The location of the start of the line.
    float m_Ax, m_Ay, m_Az;
    //! The location of the end of the line,
    float m_Bx, m_By, m_Bz;
  };
public:
  static void SetWater(Water * water);
  static void Render();
  static glm::vec3 m_WaterColor;
  static float m_AmbientFactor;
  static glm::vec3 m_AmbientColor;
  static glm::vec3 m_DiffuseColor;
  static float m_SpecularFactor;
  static int m_SpecularExponent;
  static glm::vec3 m_SpecularColor;
  static float m_AlphaMinuend;
private:
  WaterGerstnerRenderer() {}
  static void ResetBuffers();
  static void PrepareBuffers();
  static void AddLine(const glm::vec3 & start, const glm::vec3 & end);
  static void ClearLines();
  static void ManageInput();
  //! Tracks whether a Water object has been sent to the WaterRenderer.
  static bool m_WaterSet;
  //! The water being drawn by the WaterRenderer.
  static Water * m_Water;
  //! The shader being used by the WaterRenderer.
  static WaterShader * m_WaterShader;
  //! The shader used for drawing lines.
  static LineShader * m_LineShader;
  //! The vertex buffer ID.
  static GLuint m_VBOID;
  //! The element buffer ID.
  static GLuint m_EBOID;
  //! The vertex attribute ID.
  static GLuint m_VAOID;
  //! The vbo used for line drawing.
  static GLuint m_LineVBOID;
  //! The number of vertex indicies in the element buffer.
  static unsigned m_NumIndices;
  //! Determines whether the water is drawn with LINE or FILL.
  static bool m_LineDraw;
  //! All of the lines that will be drawn during the Render call.
  static std::vector<Line> m_Lines;
  //! The camera used for viewing the water.
  static Camera m_Camera;
  //! The controller used to move the camera.
  static CameraController m_Controller;
  //! Giving the water the ability to have complete access to the renderer.
  friend Water;
};

// S_WATEREDITOR /////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/// @brief A static class that is used to edit the waves that are within an
/// instance of a Water simulation.
///////////////////////////////////////////////////////////////////////////////
class WaterEditor
{
public:
  static void SetWater(Water * water);
  static void DisplayEditor();
  //! Whether to show the water editor or not.
  static bool m_Show;
private:
  WaterEditor() {}
  static void MenuBar();
  static void DisplayWave(Water::Wave & wave);
  static void ChangeWaterColor();
  static void ChangeLightColor();
  static void ChangeColor(glm::vec3 * color);
  static void EditWaveName();
  static void OpenWindow();
  static void ExportWindow();
  //! The Water that the editor is being used on.
  static Water * m_Water;
  static bool m_ChangingWaterColor;
  static bool m_ChangingLightColor;
  //! The Wave whose name is being edited.
  static Water::Wave * m_EditingName;
  //! The buffer for storing a Wave's new name.
  static char m_NewWaveName[NAMEBUFFERSIZE];
  //! Tracks whether the user is currently Exporting a file.
  static bool m_Exporting;
  //! Tracks whether the user is currentlly opening a file.
  static bool m_Opening;
  //! Stores the text input for a fille name.
  static char m_FileName[WAVEFILEBUFFERSIZE];
};
