///////////////////////////////////////////////////////////////////////////////
/// @file WaterFFT.h
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-10-05
///
/// @brief Contains the interface for the fast fourier transform water
/// simulation.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <FFTW\fftw3.h>
#include <functional>
#include <GL\glew.h>
#include <GLM\glm\vec3.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "Complex.h"
#include "FFT.h"
#include "Shader.h"
#include "ThreadUtils.h"

typedef unsigned int uint;
typedef unsigned char uchar;

// MATH HELPERS ///////////////////////////////////////////////////////////////

float Lerp(float a, float b, float t);
float QuadLerp(float a, float b, float c, float d, float tx, float ty);
int Clamp(int min, int max, int value);

// WATERFFT ///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief 
/// A fast fourier transform water simulation. This will create a mesh that 
/// can with a vertex buffer and index buffer that can be rendered with any
/// rendering system.
///
/// Important Notes
/// - Call Update each frame before uploading the new water data to the GPU.
/// - Update takes the total time as the argument.
///////////////////////////////////////////////////////////////////////////////
class WaterFFT
{
private:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// A single vertex that will make up the entire water vertex buffer.
  /// 
  /// Important Notes
  /// - There is w component for position and normal so data is sent to the
  ///   GPU as four floats
  /////////////////////////////////////////////////////////////////////////////
  struct Vertex
  {
    Vertex(float x, float y, float z, float w, 
      float nx, float ny, float nz, float nw) :
      m_Px(x), m_Py(y), m_Pz(z), m_Pw(w), 
      m_Nx(nx), m_Ny(ny), m_Nz(nz), m_Nw(nw)
      {}
    // W components create proper byte alignment for rendering pipeline
    //! The position of the vertex.
    float m_Px, m_Py, m_Pz, m_Pw;
    //! The normal of the vertex.
    float m_Nx, m_Ny, m_Nz, m_Nw;
  };
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// Stores extra information about each vertex that is needed to perform the
  /// FFT simulation.
  /////////////////////////////////////////////////////////////////////////////
  struct VertexExtra
  {
    VertexExtra(float ox, float oy, float oz, const Complex & htilde0,
      const Complex & htilde0_conjugate) : m_Ox(ox), m_Oy(oy), m_Oz(oz),
      m_HTilde0(htilde0), m_HTilde0Conjugate(htilde0_conjugate) {}
    //! Original vertex position.
    float m_Ox, m_Oy, m_Oz;
    //! Complex HTilde0(k) value for a vertex.
    Complex m_HTilde0;
    //! Complex HTilde0(-k) conjugate value for a vertex.
    Complex m_HTilde0Conjugate;
  };

  struct Offset
  {
    Offset(float x, float y, float z, float w) :
      m_Ox(x), m_Oy(y), m_Oz(z), m_Ow(w)
    {}
    float m_Ox, m_Oy, m_Oz, m_Ow;
  };
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// Stores a postion on the mesh using a vertex index and linear
  /// interpolation parameters in the positive x and z directions.
  /////////////////////////////////////////////////////////////////////////////
  struct MeshPosition
  {
    MeshPosition(unsigned vertex_index, float xt, float zt) :
      m_VertexIndex(vertex_index), m_Xt(xt), m_Zt(zt) {}
    //! The vertex index on the mesh.
    unsigned m_VertexIndex;
    //! The linear interpolation parameter on the positive x axis.
    float m_Xt;
    //! The linear interpolation parameter on the positive z axis.
    float m_Zt;
  };
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// Used for loading and storing an intensity map. An intensity map is used
  /// to adjust the how much the water simulation takes affect on parts of the
  /// mesh. Any entirely white texutre will result in max intensity across
  /// the entire water surface. Darker shades indicate that the water will be
  /// less intense on those areas of the surface.
  /////////////////////////////////////////////////////////////////////////////
  struct IntensityMap
  {
    IntensityMap(const std::string & filename);
    ~IntensityMap();
    float GetIntensity(float x, float z);
    std::string m_InstensityFile;
    uchar * m_Data;
    int m_Width;
    int m_Height;
    uint m_MaxX;
    uint m_MaxY;
    int m_Channels;
  };
public:
  WaterFFT(unsigned grid_dimension, float meter_dimension, unsigned expansion,
    bool use_fft);
  ~WaterFFT();
  bool UseIntensityMap(const std::string & filename);
  bool RemoveIntensityMap();
  std::pair<float, glm::vec3> HeightNormalAtLocation(
    const glm::vec2 & location, float time);
  float HeightAtLocation(const glm::vec2 & location);
  void Update(float time);
  void SwapBuffers();
  const void * VertexBuffer();
  const void * IndexBuffer();
  const void * OffsetBuffer();
  unsigned VertexBufferSizeBytes();
  unsigned IndexBufferSizeBytes();
  unsigned IndexBufferSize();
  unsigned OffsetBufferSizeBytes();
  unsigned OffsetBufferSize();
  // Scaler for the height of verts
  float m_HeightScale;
  // Scaler for the displace of verts
  float m_DisplaceScale;
private:
  void UpdateFFT(float time);
  std::pair<float, glm::vec3> GetLocationHeightNormalFFT(
    const glm::vec2 & location);
  float GetLocationHeightFFT(const MeshPosition & mesh_position);
  glm::vec3 GetLocationNormalFFT(const MeshPosition & mesh_position);
  MeshPosition LocationToMeshPosition(glm::vec2 location);
  Complex HTilde(const Complex & htilde0,
    const Complex & htilde0_conjugate, const glm::vec2 & k, float time);
  float DispersionRelation(const glm::vec2 & k);
  Complex HTilde0(const glm::vec2 & k);
  float PhillipsSpectrum(const glm::vec2 & k);
  void InitializeVertexBuffer();
  void InitializeIndexBuffer();
  void InitializeOffsetBuffer(unsigned expansion);
  // Vertex information
  //! Number of vertices on the x axis.
  unsigned m_XStride;
  //! Number of vertices on the z axis.
  unsigned m_ZStride;
  //! The total number of verts on the mesh.
  unsigned m_NumVerts;
  //! The front and back buffers. m_ReadBuffer is the buffer that is currently
  // being treated as the front buffer.
  std::vector<Vertex> m_VertexBufferA;
  std::vector<Vertex> m_VertexBufferB;
  std::vector<Vertex> * m_ReadBuffer;
  std::vector<Vertex> * m_WriteBuffer;
  //! The index buffer used for rendering the mesh.
  std::vector<unsigned int> m_IndexBuffer;
  //! The positional offsets for each instance. The size of this is number
  // of instances to be drawn.
  std::vector<Offset> m_OffsetBuffer;
  // Buffer for all of the extra vertex information (not needed for rendering)
  std::vector<VertexExtra> m_VertexExtrasBuffer;
  // Used for computing FFT
  // Input arrays
  Complex * m_HTildeIn;
  Complex * m_HTildeSlopeXIn;
  Complex * m_HTildeSlopeZIn;
  Complex * m_HTildeDisplaceXIn;
  Complex * m_HTildeDisplaceZIn;
  // Ouput arrays
  Complex * m_HTildeOut;
  Complex * m_HTildeSlopeXOut;
  Complex * m_HTildeSlopeZOut;
  Complex * m_HTildeDisplaceXOut;
  Complex * m_HTildeDisplaceZOut;
  // The plans for computing FFT.
  fftwf_plan m_HTildeFFTWPlan;
  fftwf_plan m_HTildeSlopeXPlan;
  fftwf_plan m_HTildeSlopeZPlan;
  fftwf_plan m_HTildeDisplaceXPlan;
  fftwf_plan m_HTildeDisplaceZPlan;
  //! The intensity map used for scaling sections of the simulation.
  IntensityMap * m_IMap;
  //! The length of the mesh in the x direction in meters.
  float m_XLength;
  //! The length of the mesh in the z direction in meters.
  float m_ZLength;
  //! This will affect amplitude, but it is not directly
  // representitive of wave heights.
  float m_Amplitude;
  //! The gravitional constant.
  float m_Gravity;
  //! The wind direction and magnitude.
  glm::vec2 m_Wind;
};

// WATERFFTHOLDER /////////////////////////////////////////////////////////////

class WaterFFTHolder
{
  public:
    static void Initialize();
    static void PrepBuffers();
    static void Update(float time);
    static void Purge();
  public:
    static WaterFFT * GetWaterFFT();
  private:
    static WaterFFT * m_Water;
};

// WATERFFTTHREAD /////////////////////////////////////////////////////////////

class WaterFFTThread
{
  public:
    static void Execute(float (* fetch_time)(void));
    static void Wait(bool kill = false);
    static void Terminate();
  private:
    static void RunWater();
    static bool m_Running;
    static std::function<float()> m_FetchTime;
    static std::thread * m_Water;
    static Barrier m_Barrier;
};

// WATERFFTERROR //////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// The type that is thrown when a WaterFFT instance encounters an error.
///////////////////////////////////////////////////////////////////////////////
class WaterFFTError
{
public:
  //! The types of WaterFFT errors that can occur.
  enum Type
  {
    INVALID_GRID_DIM,
    SMALL_DX_DZ
  };
  Type GetType() const;
  const std::string & GetDescription() const;
private:
  WaterFFTError(Type type, const std::string & description);
  //! The type of the error.
  Type m_Type;
  //! A description of the error.
  std::string m_Description;
  friend WaterFFT;
};

// WATERRENDERER /////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// An OpenGL renderer used to show a WaterFFT instance. It can be abstracted
/// to show any mesh.
///////////////////////////////////////////////////////////////////////////////
class WaterRenderer
{
private:
  ////////////////////////////////////////////////////////////////////////////
  /// @brief 
  /// The GLSL shader that is used for drawing the Water surface.
  ////////////////////////////////////////////////////////////////////////////
  class WaterShader : public Shader
  {
  public:
    WaterShader();
    // Attribute locations
    GLuint m_APosition;
    GLuint m_ANormal;
    GLuint m_AOffset;
    // Uniform locations
    GLuint m_UTransform;
    GLuint m_UWaterColor;
    GLuint m_UAmbientFactor;
    GLuint m_UAmbientColor;
    GLuint m_UDiffuseColor;
    GLuint m_USpecularFactor;
    GLuint m_USpecularExponent;
    GLuint m_USpecularColor;
    GLuint m_ULightDirection;
    GLuint m_UCameraPosition;
    GLuint m_UTime;
  };
public:
  static void SetBuffers(const GLfloat * buff_vertex, 
    const GLuint * buff_index, const GLfloat * buff_offset, 
    GLuint vertex_size_bytes, GLuint index_size_bytes, 
    unsigned index_size, GLuint offset_size_bytes,
    unsigned num_instances);
  static void SetVertexBuffer(const GLfloat * buff_vertex);
  static void Render(const glm::vec3 & location,const glm::mat4 & projection, 
    const glm::mat4 & world_to_camera);
  // The water color.
  static glm::vec3 m_WaterColor;
  // Material values used for phong rendering.
  static float m_AmbientFactor;
  static float m_DiffuseFactor;
  static float m_SpecularFactor;
  static int m_SpecularExponent;
  // Color values used for phong rendering.
  static glm::vec3 m_AmbientColor;
  static glm::vec3 m_DiffuseColor;
  static glm::vec3 m_SpecularColor;
private:
  WaterRenderer() {}
  static void DeleteBuffers();
  static void PrepareBuffers();
  static void ManageInput();
  // The shader being used by the WaterRenderer.
  static WaterShader * m_WaterShader;
  // The vertex buffer ID.
  static GLuint m_WaterVBOID;
  // The element buffer ID.
  static GLuint m_WaterEBOID;
  // The vertex attribute ID.
  static GLuint m_WaterVAOID;
  // The offset buffer ID
  static GLuint m_OffsetVBOID;
  // Pointers to the vertex and index buffers.
  static const GLfloat * m_VertexBuffer;
  static const GLuint * m_IndexBuffer;
  static const GLfloat * m_OffsetBuffer;
  // Sizes of the vertex, index and offset buffers.
  static GLuint m_VertexBufferSizeBytes;
  static GLuint m_IndexBufferSizeBytes;
  static GLuint m_OffsetBufferSizeBytes;
  // The number of values in the index buffer.
  static unsigned m_NumIndices;
  // The number of instances that will be drawn.
  static unsigned m_NumInstances;
  // Determines whether the water is drawn with LINE or FILL.
  static bool m_LineDraw;
};
