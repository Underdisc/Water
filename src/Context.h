/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Context.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 30/09/2016
\brief
  Contains the interface to the Context class. Look at the Context class for
  more info. Make sure to pay attention to the important notes.
*/
/*****************************************************************************/


#ifndef CONTEXT_H
#define CONTEXT_H

// S_CONTEXT //////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>

// Pre-Declarations (Defined in other files)
class OpenGLContext;

/*****************************************************************************/
/*!
\class Context
\brief
  This is used for creating an SDL context that can be used by openGL. Once a
  context is created, a new  context cannot be created until the old one is
  destroyed. This is a static class, so an instance of this class will never be
  constructed.

\par
    Important Notes
    - Specify both the width and height to be zero if you want the context to
        instantly go fullscreen.
    - If you are including this with a project, make sure that your main
        function is defined as the following.
        (int main(int argc, char * argv[])). SDL requires this for some reason.
*/
/*****************************************************************************/
class Context
{
public:
  static void Create(const char * name = "Context", bool opengl = false,
                     void (* adjust_viewport)() = nullptr,
                     int width = 600, int height = 600,
                     int xposition = 30, int yposition = 30);
  static void Purge();
  static void AddEventProcessor(void(*process_event)(SDL_Event * event));
  static void CheckEvents();
  static void Fullscreen();
  static void HideCursor(bool hide);
  static bool Created();
  static bool KeepOpen();
  static void Close();
  static SDL_Window * SDLWindow();
private:
  static void OnWindowEvent(const SDL_Event & event);
  //! Tracks whether a context has been created or not. Once a context is
  // created, a new one cannot be created.
  static bool _created;
  //! Tracks the status of the Context. If the context is open and should
  // remain open, this will be true. If the Context needs to be closed, this
  // will be false. This value is accessed through the keep open function
  static bool _close;
  //! Callback function for adjusting the viewport for OpenGL or DirectX.
  // Anytime the window sizes this function will be called given that you
  // have given this function pointer to the Context class.
  static void(*_adjustViewport)();
  //! Callback function for external libraries that need to process events.
  static void(*_processEvent)(SDL_Event * event);
  //! A pointer to the SDL window.
  static SDL_Window * _window;
  friend OpenGLContext;
  Context() {}
};

// S_INPUT ////////////////////////////////////////////////////////////////////

#include <vector>
#include <utility>

//! All of the main keys that can be detected by the input class. 
// If a key is not listed  here, it will be registered as OTHERKEY.
enum Key
{
  ESCAPE, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  TILDE, N1, N2, N3, N4, N5, N6, N7, N8, N9, N0, MINUS, EQUAL, BACKSPACE,
  TAB, Q, W, E, R, T, Y, U, I, O, P, BRACKETLEFT, BRACKETRIGHT, BACKSLASH,
  CAPSLOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, APOSTROPHE, RETURN,
  SHIFTLEFT, Z, X, C, V, B, N, M, COMMA, PERIOD, FOWARDSLASH, SHIFTRIGHT,
  CTRLLEFT, GUILEFT, ALTLEFT, SPACE, ALTRIGHT, SELECT,  CTRLRIGHT,
  PRINTSCREEN, SCROLLLOCK, PAUSE,
  INSERT, HOME, PAGEUP, DELETE, END, PAGEDOWN,
  ARROWUP, ARROWDOWN, ARROWLEFT, ARROWRIGHT,
  OTHERKEY,
  NUMKEYS
};

//! All of the MouseButtons that can be detected by the input class. 
// If a button is not listed here, it will be registered as OTHERMBUTTON.
enum MButton
{
  LEFT,
  MIDDLE,
  RIGHT,
  OTHERMBUTTON,
  NUMMBUTTONS
};

//! All Controller buttons that can be detected by the input class.
enum CButton
{
  EX, CIRCLE, TRIANGLE, SQUARE, 
  DPADUP, DPADDOWN, DPADLEFT, DPADRIGHT,
  BUMPERLEFT, BUMPERRIGHT,
  STICKLEFT, STICKRIGHT,
  RESET, START,
  OTHERCBUTTON,
  NUMCBUTTONS
};

//! All controller analogs that can be detected by the input class.
enum CAnalog
{
  TRIGGERLEFT, TRIGGERRIGHT,
  STICKLEFTX, STICKLEFTY, STICKRIGHTX, STICKRIGHTY,
  OTHERCANALOG,
  NUMCANALOGS
};

/*****************************************************************************/
/*!
\class Input
\brief
  This is used to interface with the mouse, keyboard, and controller input 
  from the SDL context created by the Context class. All of the keys, mouse 
  buttons, controller buttons, and controller analogs registered by this class 
  can be found in the Key, MButton, CButton, and CAnalog enums,
  respectively.
*/
/*****************************************************************************/
class Input
{
public:
  static bool KeyDown(Key key);
  static bool KeyPressed(Key key);
  static bool AnyKeyPressed();
  static bool MouseButtonDown(MButton mouse_button);
  static bool MouseButtonPressed(MButton mouse_button);
  static bool AnyMouseButtonPressed();
  static const std::pair<int, int> & MouseMotion();
  static const std::pair<int, int> & MouseLocation();
  static int MouseWheelMotion();
public:
  /***************************************************************************/
  /*!
  \class Controller
  \brief
    Used for tracking the analog and input values on a single controller.
    This will also be the structer that is used to retrieve information about
    the inputs on a specific controller. Simply use the GetController function
    from the Input class.
  */
  /***************************************************************************/
  class Controller
  {
  public:
    bool ButtonDown(CButton button) const;
    bool ButtonPressed(CButton button) const;
    Sint16 AnalogValue(CAnalog analog) const;
    bool AnalogActive(CAnalog analog) const;
    float TriggerLeft() const;
    float TriggerRight() const;
    const std::pair<float, float> & StickLeft() const;
    const std::pair<float, float> & StickRight() const;
  private:
    Controller(Sint32 id, SDL_GameController * controller);
    void UpdateAnalog(CAnalog analog, Sint16 value);
    //! The SDL id for the controller.
    Sint32 _id;
    //! A pointer to the SDL game controller.
    SDL_GameController * _sdlController;
    //! Tracks which buttons are currently down on the controller.
    bool _buttonsDown[NUMCBUTTONS];
    //! Tracks which buttons were pressed during the previous frame.
    std::vector<CButton> _buttonsPressed;
    //! Tracks the analog values of all analogs on the controller.
    Sint16 _analogs[NUMCANALOGS];
    //! The analog value of the left trigger given as a value between 0 and 1.
    float _triggerLeft;
    //! The analog value of the right trigger given as a value between 0 and 1.
    float _triggerRight;
    //! Analog values of the left stick given as values between 0 and 1.
    std::pair<float, float> _stickLeft;
    //! Analog values of the right sitck given as values between 0 and 1.
    std::pair<float, float> _stickRight;
    //! Tracks which analogs are beyond the analog threshold (ANALOG_EPSILON).
    bool _activeAnalogs[NUMCANALOGS];
    friend Input;
  };
  static Sint32 GetInactiveController();
  static bool IsActiveControlller(Sint32 id);
  static void ActivateController(Sint32 id);
  static void DeactiveateController(Sint32 id);
  static Controller * GetController(Sint32 id);
private:
  Input();
  static Key ScancodeToKey(SDL_Scancode value);
  static MButton UnsignedToMButton(unsigned value);
  static CButton UnsignedToCButton(unsigned value);
  static CAnalog UnsignedToCAnalgo(unsigned sld_axis);
  static void Reset();
  static void OnKeyDownEvent(const SDL_Event & event);
  static void OnKeyUpEvent(const SDL_Event & event);
  static void OnMouseButtonDownEvent(const SDL_Event & event);
  static void OnMouseButtonUpEvent(const SDL_Event & event);
  static void OnMouseMotionEvent(const SDL_Event & event);
  static void OnMouseWheelEvent(const SDL_Event & event);
  static void OnControllerAdded(const SDL_Event & event);
  static void OnControllerRemoved(const SDL_Event & event);
  static void OnControllerDown(const SDL_Event & event);
  static void OnControllerUp(const SDL_Event & event);
  static void OnControllerAxis(const SDL_Event & event);
  //! Tracks which keys are down and which are not. True means that a key is
  // down.
  static bool _keysDown[NUMKEYS];
  //! Stores all of the keys that were pressed during the previous frame.
  // These values will be cleared and re-evaluated each time SDL events are
  // checked for.
  static std::vector<Key> _keysPressed;
  //! Tracks which mosue buttons are down. True means that the button is down.
  static bool _mouseButtonsDown[NUMMBUTTONS];
  //! This is the same as the _keyPressed vector, but is instead used for mouse
  // buttons.
  static std::vector<MButton> _mouseButtonsPressed;
  //! Stores the motion of the mouse during the previous frame.
  static std::pair<int, int> _mouseMotion;
  //! Stores the location of the mouse during the previous frame.
  static std::pair<int, int> _mouseLocation;
  //! Stores the motion undergone by the mouse wheel during the previous frame.
  static int _mouseWheelMotion;
  //! The ids of controllers that are available but have not yet been activate.
  static std::vector<Sint32> _inactiveController;
  //! The controllers that are active.
  static std::vector<Controller> _activeController;
  //! The value that an analog should be above in order to be considered 
  // active.
  static float _analogThreshold;
  //! When events are checked for, the CheckEvents function will first call the Reset
  // function and then proceed to grab SDL events.
  friend Context;
};

#endif // !CONTEXT_H