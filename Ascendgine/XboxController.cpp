#include "XboxController.h"
#include <SDL\SDL_gamecontroller.h>
#include <SDL\SDL_haptic.h>
#include <stdio.h>
#include <string.h>

#define DEADZONE_LEFT_X 10000
#define DEADZONE_LEFT_Y 10000
#define DEADZONE_RIGHT_X 10000
#define DEADZONE_RIGHT_Y 10000
#define DEADZONE_LEFT_TRIGGER -20000
#define DEADZONE_RIGHT_TRIGGER -20000

SDL_Joystick* controllers[4];
SDL_Haptic* haptics[4];

int XboxController_GetNumberOfControllers()
{
  return SDL_NumJoysticks();
}

void XboxController_GetController(XboxController* pController, int player, float dt)
{
  pController->controllerID = player;

  controllers[player] = SDL_JoystickOpen(player);
  if (controllers[player])
  {
    SDL_JoystickGUID guid = SDL_JoystickGetGUID(controllers[player]);
    char guid_str[1024];
    SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
    const char* name = SDL_JoystickName(controllers[player]);

    int num_axes = SDL_JoystickNumAxes(controllers[player]);
    int num_buttons = SDL_JoystickNumButtons(controllers[player]);
    int num_hats = SDL_JoystickNumHats(controllers[player]);
    int num_balls = SDL_JoystickNumBalls(controllers[player]);

    int16_t leftThumbStickX = SDL_JoystickGetAxis(controllers[player], 0);
    int16_t leftThumbStickY = SDL_JoystickGetAxis(controllers[player], 1);
    int16_t rightThumbStickX = SDL_JoystickGetAxis(controllers[player], 2);
    int16_t rightThumbStickY = SDL_JoystickGetAxis(controllers[player], 3);
    int16_t leftTrigger = SDL_JoystickGetAxis(controllers[player], 4);
    int16_t rightTrigger = SDL_JoystickGetAxis(controllers[player], 5);

    if (leftThumbStickX < DEADZONE_LEFT_X && leftThumbStickX > -DEADZONE_LEFT_X)
      leftThumbStickX = 0;
    if (leftThumbStickY < DEADZONE_LEFT_Y && leftThumbStickY > -DEADZONE_LEFT_Y)
      leftThumbStickY = 0;
    if (rightThumbStickX < DEADZONE_RIGHT_X && rightThumbStickX > -DEADZONE_RIGHT_X)
      rightThumbStickX = 0;
    if (rightThumbStickY < DEADZONE_RIGHT_Y && rightThumbStickY > -DEADZONE_RIGHT_Y)
      rightThumbStickY = 0;
    if (leftTrigger < DEADZONE_LEFT_TRIGGER)
      leftTrigger = 0;
    if (rightTrigger < DEADZONE_LEFT_TRIGGER)
      rightTrigger = 0;

    // 32768 range either side of 0
    pController->leftThumbX = leftThumbStickX / 32768;
    pController->leftThumbY = leftThumbStickY / 32768;
    pController->rightThumbX = rightThumbStickX / 32768;
    pController->rightThumbY = rightThumbStickY / 32768;
    pController->leftTrigger = leftTrigger / 32768;
    pController->rightTrigger = rightTrigger / 32768;

    pController->thumbpadX = SDL_JoystickGetButton(controllers[player], 2) * -1.f + SDL_JoystickGetButton(controllers[player], 3);
    pController->thumbpadY = SDL_JoystickGetButton(controllers[player], 1) * -1.f + SDL_JoystickGetButton(controllers[player], 0);
    pController->start = SDL_JoystickGetButton(controllers[player], 4) == 1;
    pController->back = SDL_JoystickGetButton(controllers[player], 5) == 1;
    pController->leftStick = SDL_JoystickGetButton(controllers[player], 6) == 1;
    pController->rightStick = SDL_JoystickGetButton(controllers[player], 7) == 1;
    pController->leftBumper = SDL_JoystickGetButton(controllers[player], 8) == 1;
    pController->rightBumper = SDL_JoystickGetButton(controllers[player], 9) == 1;
    pController->a = SDL_JoystickGetButton(controllers[player], 10) == 1;
    pController->b = SDL_JoystickGetButton(controllers[player], 11) == 1;
    pController->x = SDL_JoystickGetButton(controllers[player], 12) == 1;
    pController->y = SDL_JoystickGetButton(controllers[player], 13) == 1;
    pController->home = SDL_JoystickGetButton(controllers[player], 14) == 1;
    pController->isConnected = true;

    SDL_JoystickClose(controllers[player]);
  }
  else
    pController->isConnected = false;


  if (haptics[player] && pController->hapticFeedbackTimeRemaining > 0.f)
  {
    pController->hapticFeedbackTimeRemaining -= dt;
  }
}

void XboxController_Rumble(XboxController* pController, float intensity /*= 1.f*/, int millisecondsToRumble /*= 2000*/)
{
  if (!haptics[pController->controllerID])
  {
    haptics[pController->controllerID] = SDL_HapticOpenFromJoystick(controllers[pController->controllerID]);
    if (SDL_HapticRumbleInit(haptics[pController->controllerID]) != 0)
      haptics[pController->controllerID] = nullptr;
  }

  if (haptics[pController->controllerID])
  {
    pController->hapticFeedbackTimeRemaining = millisecondsToRumble;
    SDL_HapticRumblePlay(haptics[pController->controllerID], intensity, millisecondsToRumble);
  }
}