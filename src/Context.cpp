/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Context.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 30/09/2016
\brief
  Contains the implementation of the static Context class. Look at the 
  documentation of the Context class for more info on this code.
*/
/*****************************************************************************/

// S_CONTEXT //////////////////////////////////////////////////////////////////

#include <SDL\SDL.h>
// Undefine the main define by SDL
#undef main
#include "Error.h"

#include "Context.h"


// static initializations
bool Context::_close = true;
bool Context::_created = false;
void(*Context::_adjustViewport)() = nullptr;
void(*Context::_processEvent)(SDL_Event * event) = nullptr;
SDL_Window * Context::_window = nullptr;

/*****************************************************************************/
/*!
\brief
  Constructor for the Context class. If all of the parameters are not given,
  the following the defaults will be applied. If the given width and height
  are zero, the Context will instantly fullscreen.

\par
  Defaults
   - name: "Context"
   - opengl: false
   - adjust_viewport: nullptr
   - width:  600
   - height: 600
   - xposition: 0
   - yposition: 0

\param name
  The name that will be seen in the top left of the window.
\param opengl
  Set to true if this Context is being used for an OpenGLContext.
\param adjust_viewport
  Callback function for adjusting the graphics viewport after window resizing.
\param width
  The width of the window.
\param height
  The height of the window.
\param xposition
  The X position of the top left corner of the window.
\param yposition
  The Y position of the top left cornter of the window.
*/
/*****************************************************************************/
void Context::Create(const char * name, bool opengl, void(*adjust_viewport)(),
                     int width, int height,
                     int xposition, int yposition)
{
  // throwing error if context already exists
  if (_created)
  {
    RootError root_error("Context.cpp", "Init");
    root_error.Add("A Context already exists.");
    root_error.Add("Only one context can exist at a time.");
    throw(root_error);
  }
  // initialize sdl
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
  // create window flags
  Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS |
    SDL_WINDOW_MOUSE_FOCUS;
  if (width == 0 && height == 0)
    flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
  if (opengl)
    flags = flags | SDL_WINDOW_OPENGL;
  // creating window
  _window = SDL_CreateWindow(name, xposition, yposition, width, height, flags);
  // making sure windo was created
  if (!_window)
  {
    SDL_Quit();
    RootError root_error("Context.cpp", "CreateContext");
    root_error.Add("SDL window creation failed.");
    throw(root_error);
  }
  // setting callback
  _adjustViewport = adjust_viewport;
  // done
  _close = false;
  _created = true;
}

/*****************************************************************************/
/*!
\brief
  Destructor for the Context class. This will clean up all the Context
  resources.
*/
/*****************************************************************************/
void Context::Purge()
{
  SDL_DestroyWindow(_window);
  SDL_Quit();
  _created = false;
}

/*****************************************************************************/
/*!
\brief Add a function that will process events sent from SDL. This will
 typically be used if events that are not checked by the Context class and
 you want those events or, alternatively, an external library needs access to
 the SDL events.

\param process_event The function that will be used for processing the SDL
  events.
*/
/*****************************************************************************/
void Context::AddEventProcessor(void (*process_event)(SDL_Event * event))
{
  _processEvent = process_event;
}

/*****************************************************************************/
/*!
\brief
  Checks for any SDL Events that exits within SDL's event queue and acts
  according to the type of the event.
*/
/*****************************************************************************/           
void Context::CheckEvents()
{
  // Clearing pressed keys and other values
  // from previous frame
  Input::Reset();
  SDL_Event event;
  // reading all events in queue
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_WINDOWEVENT: OnWindowEvent(event); break;
      case SDL_KEYDOWN: Input::OnKeyDownEvent(event); break;
      case SDL_KEYUP: Input::OnKeyUpEvent(event); break;
      case SDL_MOUSEBUTTONDOWN: Input::OnMouseButtonDownEvent(event); break;
      case SDL_MOUSEBUTTONUP: Input::OnMouseButtonUpEvent(event); break;
      case SDL_MOUSEWHEEL: Input::OnMouseWheelEvent(event); break;
      case SDL_MOUSEMOTION: Input::OnMouseMotionEvent(event); break;
      case SDL_CONTROLLERDEVICEADDED: Input::OnControllerAdded(event); break;
      case SDL_CONTROLLERDEVICEREMOVED: Input::OnControllerRemoved(event); break;
      case SDL_CONTROLLERBUTTONDOWN: Input::OnControllerDown(event); break;
      case SDL_CONTROLLERBUTTONUP: Input::OnControllerUp(event); break;
      case SDL_CONTROLLERAXISMOTION: Input::OnControllerAxis(event); break;
    }
  }
  // use external event processor
  if (_processEvent)
    _processEvent(&event);
}

/*****************************************************************************/
/*!
\brief
  If called, both the contex will be made fullscreen. Update
  does not have to be called after this function. The function will handle the
  Context change.
*/
/*****************************************************************************/
void Context::Fullscreen()
{
  SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  if(_adjustViewport)
    _adjustViewport();
}

/*****************************************************************************/
/*!
\brief
  Used to show or hide the cursor when it is hovering over the Context.

\param hide
  If true, the cursor will be hidden when hovering over the context. If false,
  the cursor will always be visible.
*/
/*****************************************************************************/
void Context::HideCursor(bool hide)
{
  if(hide)
    SDL_ShowCursor(SDL_DISABLE);
  else
    SDL_ShowCursor(SDL_ENABLE);
}

/*****************************************************************************/
/*!
\brief
  Gets a pointer to the SDL window for this context.

\return A pointer to the SDL window for this context.
*/
/*****************************************************************************/
SDL_Window * Context::SDLWindow()
{
  return _window;
}

/*****************************************************************************/
/*!
\brief
  Identifies whether the SDL context has been created or not. 

\return If the SDL Context has been created, true.
*/
/*****************************************************************************/
bool Context::Created()
{
  return _created;
}

/*****************************************************************************/
/*!
\brief
  Get the current Context status. If it is to remain open, this will return
  true. Otherwise, this will return false.

\return The current status of the Context.
*/
/*****************************************************************************/
bool Context::KeepOpen()
{
  if(_close)
    return false;
  return true;
}

/*****************************************************************************/
/*!
\brief
  Calling this signifies that the context should close. It will not actually
  close when this function called. The close bool in the class will simply be
  set to true. It is your job to check this value using the KeepOpen function.
  This function and KeepOpen are used to maintain a game loop.
*/
/*****************************************************************************/
void Context::Close()
{
  _close = true;
}

/*****************************************************************************/
/*!
\brief
  This will be called from CheckEvents() when SDL sends a window event. This
  includes evetns such as maximizing, minimizing, resizing, closing, etc.

\param event
  The window event that was pulled off the event queue from SDL_PollEvents.
*/
/*****************************************************************************/
void Context::OnWindowEvent(const SDL_Event & event)
{
  switch (event.window.event)
  {
    case SDL_WINDOWEVENT_RESIZED: if(_adjustViewport) _adjustViewport(); break;
    case SDL_WINDOWEVENT_CLOSE: Close(); break;
  }
}

// S_INPUT ////////////////////////////////////////////////////////////////////

// static initializations
bool Input::_keysDown[NUMKEYS] = { false };
std::vector<Key> Input::_keysPressed;
bool Input::_mouseButtonsDown[NUMMBUTTONS] = { false };
std::vector<MButton> Input::_mouseButtonsPressed;
std::pair<int, int> Input::_mouseMotion(0, 0);
std::pair<int, int> Input::_mouseLocation(0, 0);
int Input::_mouseWheelMotion = 0;
std::vector<Sint32> Input::_inactiveController;
std::vector<Input::Controller> Input::_activeController;
float Input::_analogThreshold = 0.1f;


/*****************************************************************************/
/*!
\brief
  Used to find out if a key was down during the previous frame.
  
\param key
  The key that is being checked.

\return If the key was down, true.
*/
/*****************************************************************************/
bool Input::KeyDown(Key key)
{
  return _keysDown[key];
}

/*****************************************************************************/
/*!
\brief
  Used to find out if a key was pressed during the previous frame.

\param key
  The key that is being checked.

\return If the key was pressed, true.
*/
/*****************************************************************************/
bool Input::KeyPressed(Key key)
{
  unsigned size = _keysPressed.size();
  for (unsigned i = 0; i < size; ++i){
    if (_keysPressed[i] == key)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
  Used to find out if any key on the keyboard was pressed during the previous
  frame.

\return If a key was pressed, true.
*/
/*****************************************************************************/
bool Input::AnyKeyPressed()
{
  if (_keysPressed.size() > 0)
    return true;
  return false;
}

/*****************************************************************************/
/*!
\brief
  Used to find out if a mouse button was down during the previous frame.

\param mouse_button
  The mouse button that is being checked.

\return If the mouse button was down, true.
*/
/*****************************************************************************/
bool Input::MouseButtonDown(MButton mouse_button)
{
  return _mouseButtonsDown[mouse_button];
}

/*****************************************************************************/
/*!
\brief
  Used to find out if a mouse button was pressed during the previous frame.

\param mouse_button
  The mouse button that is being checked.

\return If the mouse button was pressed, true.
*/
/*****************************************************************************/
bool Input::MouseButtonPressed(MButton mouse_button)
{
  unsigned size = _mouseButtonsPressed.size();
  for (unsigned i = 0; i < size; ++i) {
    if (_mouseButtonsPressed[i] == mouse_button)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
  Used to check if any mouse button was pressed during the previous frame.

\return If a mouse button was pressed, true.
*/
/*****************************************************************************/
bool Input::AnyMouseButtonPressed()
{
  if (_mouseButtonsPressed.size() > 0)
    return true;
  return false;
}

/*****************************************************************************/
/*!
\brief
  Returns the motion undergone by the mouse during the previous frame. The 
  first value in the pair is the X motion and the second value is the Y motion.

\return A pair containing the motion of the mouse during the previous frame.
*/
/*****************************************************************************/
const std::pair<int, int> & Input::MouseMotion()
{
  return _mouseMotion;
}

/*****************************************************************************/
/*!
\brief
  Returns the location of the mouse during the previous frame. The first value
  in the pair is the X location and the second value is the Y location.

\return A pair containing the location of the mouse during the previous frame.
*/
/*****************************************************************************/
const std::pair<int, int> & Input::MouseLocation()
{
  return _mouseLocation;
}

/*****************************************************************************/
/*!
\brief
  Returns the motion undergone by the mouse wheel during the previous frame.
  Motion away from the user will result in a positive one and motion towards
  the user will result in a negative one.

\return Described in the brief.
*/
/*****************************************************************************/
int Input::MouseWheelMotion()
{
  return _mouseWheelMotion;
}

/*****************************************************************************/
/*!
\brief
  Returns the lowest inactive controller. For example, if controllers 1, 2,
  and 3 are inactive, controller 1 will be returned. If there are no inactive
  controllers, -1 will be returned.

\return The id of the inactive controller.
*/
/*****************************************************************************/
Sint32 Input::GetInactiveController()
{
  // making sure there are inactive controllers.
  if (_inactiveController.size() == 0)
    return -1;
  const Sint32 max_controllers = 4;
  Sint32 id = max_controllers;
  for (Sint32 c_id : _inactiveController) {
    if (c_id < id)
      id = c_id;
  }
  return id;
}

/*****************************************************************************/
/*!
\brief 
  Find out whether a controller is active or not through an id. This id
  will be a number from 0 to 3.

\param id The id of the controller.

\return If the controller is active, true, otherwise, false.
*/
/*****************************************************************************/
bool Input::IsActiveControlller(Sint32 id)
{
  for (const Controller & controller : _activeController) {
    if (controller._id == id)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief 
  Activates then specified controller. Throws an error if the controller
  id did not exist among the current inactive controllers. 

\param id The id that was given by GetInactiveController.
*/
/*****************************************************************************/
void Input::ActivateController(Sint32 id)
{
  std::vector<Sint32>::iterator it = _inactiveController.begin();
  std::vector<Sint32>::iterator it_e = _inactiveController.end();
  for (; it != it_e; ++it) {
    if (*it == id) {
      SDL_GameController * controller = SDL_GameControllerOpen(id);
      _inactiveController.erase(it);
      _activeController.push_back(Controller(id, controller));
      return;
    }
  }
  Error error("Context.cpp", "ActivateController");
  error.Add("The controller ID did not exist among inactive controllers.");
  throw(error);
}

/*****************************************************************************/
/*!
\brief 
  Deactivates a specified controller. An error is thrown if the given id is
  not found among the active controllers.

\param id The id of the active controller that is to be deactivated.
*/
/*****************************************************************************/
void Input::DeactiveateController(Sint32 id)
{
  std::vector<Controller>::iterator it = _activeController.begin();
  std::vector<Controller>::iterator it_e = _activeController.end();
  for (; it != it_e; ++it) {
    if (id == (*it)._id) {
      SDL_GameControllerClose((*it)._sdlController);
      _activeController.erase(it);
      _inactiveController.push_back(id);
      return;
    }
  }
  Error error("Context.cpp", "DeactivateController");
  error.Add("The controller ID did not exist among the active controllers.");
  throw(error);
}


/*****************************************************************************/
/*!
\brief
  Returns the Controller instance of a controller given a specific id for that
  controller. An error will be thrown if the id does not exist among the active
  controllers.

\param id The id of the controller to be fetched.

\return The controller instance.
*/
/*****************************************************************************/
Input::Controller * Input::GetController(Sint32 id)
{
  for (Controller & controller : _activeController) {
    if (id == controller._id)
      return &controller;
  }
  Error error("Context.cpp", "GetController");
  error.Add("The requested controller is not active.");
  throw(error);
}

/*****************************************************************************/
/*!
\brief
  Identfies whether a button was down during the previous frame or not.

\param button
  The button being evaluated.

\return If the button was down, true.
*/
/*****************************************************************************/
bool Input::Controller::ButtonDown(CButton button) const
{
  return _buttonsDown[button];
}

/*****************************************************************************/
/*!
\brief
  Identifies whether a button was pressed during the previous frame.

\param button
  The button being evaluated.

\return If the button was pressed. true
*/
/*****************************************************************************/
bool Input::Controller::ButtonPressed(CButton button) const
{
  for (CButton pressed_button : _buttonsPressed) {
    if (pressed_button == button)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
  Gets the raw analog value of an analog during the previous frame.

\param analog The analog whose value will be returned.

\return The analog's raw value.
*/
/*****************************************************************************/
Sint16 Input::Controller::AnalogValue(CAnalog analog) const
{
  return _analogs[analog];
}

/*****************************************************************************/
/*!
\brief
  Identifies whether an anlog is beyond the analog threshold, therefore
  making the analog a valid input.

\param analog The analog being checked for validity.

\return If the analog value is above the analog threshold, true.
*/
/*****************************************************************************/
bool Input::Controller::AnalogActive(CAnalog analog) const
{
  return _activeAnalogs[analog];
}

/*****************************************************************************/
/*!
\brief
  The analog value of the left trigger. Will be given as a number from 0 to 1.

\return That floating analog value of the left trigger.
*/
/*****************************************************************************/
float Input::Controller::TriggerLeft() const
{
  return _triggerLeft;
}

/*****************************************************************************/
/*!
\brief
  The analog value of the right trigger. Will be given as a number from 0 to 1.

\return That floating analog value of the right trigger.
*/
/*****************************************************************************/
float Input::Controller::TriggerRight() const
{
  return _triggerRight;
}

/*****************************************************************************/
/*!
\brief
  Returns the floating point values associated with the left stick. These
  values will be between 0 and 1.

\return The floating analog values. The first value is x, the second is y.
*/
/*****************************************************************************/
const std::pair<float, float> & Input::Controller::StickLeft() const
{
  return _stickLeft;
}

/*****************************************************************************/
/*!
\brief
  Returns the floating point values associated with the right stick. These
  values will be between 0 and 1.

\return The floating analog values. The first value is x, the second is y.
*/
/*****************************************************************************/
const std::pair<float, float> & Input::Controller::StickRight() const
{
  return _stickRight;
}



/*!
\brief Controller instance constuctor.
\param id The SDL id of the controller.
\param controller The SDL_GameController value.
*/
Input::Controller::Controller(Sint32 id, SDL_GameController * controller) :
  _id(id), _sdlController(controller), _buttonsDown{ false }, _analogs{ 0 },
  _triggerLeft(0.0f), _triggerRight(0.0f), _stickLeft(0.0f, 0.0f),
  _stickRight(0.0f, 0.0f), _activeAnalogs{ false }
{}


#include <iostream>

/*!
\brief Updates an analog value of the controller.
\param analog The analog being updated.
\param value The analog's new value.
*/
inline void Input::Controller::UpdateAnalog(CAnalog analog, Sint16 value)
{
  const float max_analog = 32767.0f;
  float norm_value = (float)value / max_analog;
  _analogs[analog] = value;
  switch (analog)
  {
  case TRIGGERLEFT: _triggerLeft = norm_value; break;
  case TRIGGERRIGHT: _triggerRight = norm_value; break;
  case STICKLEFTX: _stickLeft.first = norm_value; break;
  case STICKLEFTY: _stickLeft.second = -norm_value; break;
  case STICKRIGHTX: _stickRight.first = norm_value; break;
  case STICKRIGHTY: _stickRight.second = -norm_value; break;
  default: break;
  }
  if (norm_value > _analogThreshold || -norm_value > _analogThreshold)
    _activeAnalogs[analog] = true;
  else
    _activeAnalogs[analog] = false;
}

/*!
\brief Converts an SDL scancode to a Key.
\param value The SDL_Scancode value.
\return The SDL_Scancode's corresponding Key value.
*/
Key Input::ScancodeToKey(SDL_Scancode value)
{
  // not proud of this, but I also don't want to type
  // SDL_SCANCODE_ again.
  switch (value)
  {
    case SDL_SCANCODE_ESCAPE: return ESCAPE;
    case SDL_SCANCODE_F1: return F1;
    case SDL_SCANCODE_F2: return F2;
    case SDL_SCANCODE_F3: return F3;
    case SDL_SCANCODE_F4: return F4;
    case SDL_SCANCODE_F5: return F5;
    case SDL_SCANCODE_F6: return F6;
    case SDL_SCANCODE_F7: return F7;
    case SDL_SCANCODE_F8: return F8;
    case SDL_SCANCODE_F9: return F9;
    case SDL_SCANCODE_F10: return F10;
    case SDL_SCANCODE_F11: return F11;
    case SDL_SCANCODE_F12: return F12;
    case SDL_SCANCODE_GRAVE: return TILDE;
    case SDL_SCANCODE_1: return N1;
    case SDL_SCANCODE_2: return N2;
    case SDL_SCANCODE_3: return N3;
    case SDL_SCANCODE_4: return N4;
    case SDL_SCANCODE_5: return N5;
    case SDL_SCANCODE_6: return N6;
    case SDL_SCANCODE_7: return N7;
    case SDL_SCANCODE_8: return N8;
    case SDL_SCANCODE_9: return N9;
    case SDL_SCANCODE_0: return N0;
    case SDL_SCANCODE_MINUS: return MINUS;
    case SDL_SCANCODE_EQUALS: return EQUAL;
    case SDL_SCANCODE_BACKSPACE: return BACKSPACE;
    case SDL_SCANCODE_TAB: return TAB;
    case SDL_SCANCODE_Q: return Q;
    case SDL_SCANCODE_W: return W;
    case SDL_SCANCODE_E: return E;
    case SDL_SCANCODE_R: return R;
    case SDL_SCANCODE_T: return T;
    case SDL_SCANCODE_Y: return Y;
    case SDL_SCANCODE_U: return U;
    case SDL_SCANCODE_I: return I;
    case SDL_SCANCODE_O: return O;
    case SDL_SCANCODE_P: return P;
    case SDL_SCANCODE_LEFTBRACKET: return BRACKETLEFT;
    case SDL_SCANCODE_RIGHTBRACKET: return BRACKETRIGHT;
    case SDL_SCANCODE_BACKSLASH: return BACKSLASH;
    case SDL_SCANCODE_CAPSLOCK: return CAPSLOCK;
    case SDL_SCANCODE_A: return A;
    case SDL_SCANCODE_S: return S;
    case SDL_SCANCODE_D: return D;
    case SDL_SCANCODE_F: return F;
    case SDL_SCANCODE_G: return G;
    case SDL_SCANCODE_H: return H;
    case SDL_SCANCODE_J: return J;
    case SDL_SCANCODE_K: return K;
    case SDL_SCANCODE_L: return L;
    case SDL_SCANCODE_SEMICOLON: return SEMICOLON;
    case SDL_SCANCODE_APOSTROPHE: return APOSTROPHE;
    case SDL_SCANCODE_RETURN: return RETURN;
    case SDL_SCANCODE_LSHIFT: return SHIFTLEFT;
    case SDL_SCANCODE_Z: return Z;
    case SDL_SCANCODE_X: return X;
    case SDL_SCANCODE_C: return C;
    case SDL_SCANCODE_V: return V;
    case SDL_SCANCODE_B: return B;
    case SDL_SCANCODE_N: return N;
    case SDL_SCANCODE_M: return M;
    case SDL_SCANCODE_COMMA: return COMMA;
    case SDL_SCANCODE_PERIOD: return PERIOD;
    case SDL_SCANCODE_SLASH: return FOWARDSLASH;
    case SDL_SCANCODE_RSHIFT: return SHIFTRIGHT;
    case SDL_SCANCODE_LCTRL: return CTRLLEFT;
    case SDL_SCANCODE_LGUI: return GUILEFT;
    case SDL_SCANCODE_LALT: return ALTLEFT;
    case SDL_SCANCODE_SPACE: return SPACE;
    case SDL_SCANCODE_RALT: return ALTRIGHT;
    case SDL_SCANCODE_SELECT: return SELECT;
    case SDL_SCANCODE_RCTRL: return CTRLRIGHT;
    case SDL_SCANCODE_PRINTSCREEN: return PRINTSCREEN;
    case SDL_SCANCODE_SCROLLLOCK: return SCROLLLOCK;
    case SDL_SCANCODE_PAUSE: return PAUSE;
    case SDL_SCANCODE_INSERT: return INSERT;
    case SDL_SCANCODE_HOME: return HOME;
    case SDL_SCANCODE_PAGEUP: return PAGEUP;
    case SDL_SCANCODE_DELETE: return Key::DELETE;
    case SDL_SCANCODE_END: return END;
    case SDL_SCANCODE_PAGEDOWN: return PAGEDOWN;
    case SDL_SCANCODE_UP: return ARROWUP;
    case SDL_SCANCODE_DOWN: return ARROWDOWN;
    case SDL_SCANCODE_LEFT: return ARROWLEFT;
    case SDL_SCANCODE_RIGHT: return ARROWRIGHT;
    default: return Key::OTHERKEY;
  }
}

/*!
\brief Converts an SDL unsigned value to an MButton.
\param value The unsigned int SDL uses to refer to a mouse button.
\return The corresponding MButton value.
*/
MButton Input::UnsignedToMButton(unsigned value)
{
  switch (value)
  {
    case SDL_BUTTON_LEFT: return LEFT;
    case SDL_BUTTON_MIDDLE: return MIDDLE;
    case SDL_BUTTON_RIGHT: return RIGHT;
    default: return MButton::OTHERMBUTTON;
  }
}

/*!
\brief Convert an SDL unsigned value to a CButton.
\param value The SDL unsigned value.
\return The corresponding CButton.
*/
CButton Input::UnsignedToCButton(unsigned value)
{
  switch (value)
  {
    case SDL_CONTROLLER_BUTTON_A: return EX;  break;
    case SDL_CONTROLLER_BUTTON_B: return CIRCLE; break;
    case SDL_CONTROLLER_BUTTON_X: return SQUARE; break;
    case SDL_CONTROLLER_BUTTON_Y: return TRIANGLE; break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return DPADUP; break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return DPADDOWN; break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return DPADLEFT; break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return DPADRIGHT; break;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return BUMPERLEFT; break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return BUMPERRIGHT; break;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return STICKLEFT; break;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return STICKRIGHT; break;
    case SDL_CONTROLLER_BUTTON_BACK: return RESET; break;
    case SDL_CONTROLLER_BUTTON_START: return START; break;
    default: return OTHERCBUTTON; break;
  }
  return CButton::EX;
}

/*!
\brief Convert an SDL unsigned value to a CAnalog.
\param value The SDL unsigned value.
\return The corresponding CAnalog.
*/
CAnalog Input::UnsignedToCAnalgo(unsigned analog)
{
  switch (analog)
  {
  case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return TRIGGERLEFT; break;
  case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return TRIGGERRIGHT; break;
  case SDL_CONTROLLER_AXIS_LEFTX: return STICKLEFTX; break;
  case SDL_CONTROLLER_AXIS_LEFTY: return STICKLEFTY; break;
  case SDL_CONTROLLER_AXIS_RIGHTX: return STICKRIGHTX; break;
  case SDL_CONTROLLER_AXIS_RIGHTY: return STICKRIGHTY; break;
  default: return OTHERCANALOG; break;
  }
}

/*****************************************************************************/
/*!
\brief
  Resets values that should only be present for a single frame. This is called
  but Context::CheckEvents before events are checked for.
*/
/*****************************************************************************/
inline void Input::Reset()
{
  _keysPressed.clear();
  _mouseButtonsPressed.clear();
  _mouseMotion.first = 0;
  _mouseMotion.second = 0;
  _mouseWheelMotion = 0;
  for (Controller & controller : _activeController){
    controller._buttonsPressed.clear();
  }
}


/*!
\brief Updates Input values for a corresponding SDL_KEYDOWN event.
\param event The SDL event.
*/
void Input::OnKeyDownEvent(const SDL_Event & event)
{
  if (event.key.repeat)
    return;
  Key value = ScancodeToKey(event.key.keysym.scancode);
  _keysDown[value] = true;
  _keysPressed.push_back(value);
}


/*!
\brief Updates Input values for a corresponding SDL_KEYUP event.
\param event The SDL event.
*/
void Input::OnKeyUpEvent(const SDL_Event & event)
{
  Key value = ScancodeToKey(event.key.keysym.scancode);
  _keysDown[value] = false;
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEBUTTONDOWN event.
\param event The SDL event.
*/
void Input::OnMouseButtonDownEvent(const SDL_Event & event)
{
  MButton value = UnsignedToMButton(event.button.button);
  _mouseButtonsDown[value] = true;
  _mouseButtonsPressed.push_back(value);
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEBUTTONUP event.
\param event The SDL event.
*/
void Input::OnMouseButtonUpEvent(const SDL_Event & event)
{
  MButton value = UnsignedToMButton(event.button.button);
  _mouseButtonsDown[value] = false;
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEMOTION event.
\param event The SDL event.
*/
void Input::OnMouseMotionEvent(const SDL_Event & event)
{
  _mouseMotion.first = event.motion.xrel;
  _mouseMotion.second = event.motion.yrel;
  _mouseLocation.first = event.motion.x;
  _mouseLocation.second = event.motion.y;
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEWHEEL event.
\param event The SDL event.
*/
void Input::OnMouseWheelEvent(const SDL_Event & event)
{
  _mouseWheelMotion = static_cast<int>(event.wheel.y);
}

/*!
\brief Adds a controller to the inactive controllers vector when it is added.
\param The SDL event.
*/
void Input::OnControllerAdded(const SDL_Event & event)
{
  _inactiveController.push_back(event.cdevice.which);
}

/*!
\brief Removes a controller from the input class on a remove controller event.
\param event The SDL event.
*/
void Input::OnControllerRemoved(const SDL_Event & event)
{
  if (IsActiveControlller(event.cdevice.which))
    DeactiveateController(event.cdevice.which);
  std::vector<Sint32>::iterator it = _inactiveController.begin();
  std::vector<Sint32>::iterator it_e = _inactiveController.end();
  for (; it != it_e; ++it) {
    if (*it == event.cdevice.which) {
      _inactiveController.erase(it);
      return;
    }
  }
}

/*!
\brief Updates a controllers instance's button states on a button down event.
\param event The SDL event.
*/
void Input::OnControllerDown(const SDL_Event & event)
{
  CButton button = UnsignedToCButton(event.cbutton.button);
  for (Controller & controller : _activeController) {
    if (event.cbutton.which == controller._id) {
      controller._buttonsDown[button] = true;
      controller._buttonsPressed.push_back(button);
      return;
    }
  }
}

/*!
\brief Called when a controller button is lifted.
\param event The SDL event.
*/
void Input::OnControllerUp(const SDL_Event & event)
{
  CButton button = UnsignedToCButton(event.cbutton.button);
  for (Controller & controller : _activeController) {
    if (event.cbutton.which == controller._id) {
      controller._buttonsDown[button] = false;
      return;
    }
  }
}

/*!
\brief Updates a changed controller analog.
\param event The SDL event.
*/
void Input::OnControllerAxis(const SDL_Event & event)
{
  CAnalog analog = UnsignedToCAnalgo(event.caxis.axis);
  for (Controller & controller : _activeController) {
    if (event.caxis.which == controller._id) {
      CAnalog analog = UnsignedToCAnalgo(event.caxis.axis);
      controller.UpdateAnalog(analog, event.caxis.value);
      return;
    }
  }
}
