#include "XboxController.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <Xinput.h>

XboxController *controllers[XUSER_MAX_COUNT];

void XboxController_Init()
{
  for (int i = 0; i < XUSER_MAX_COUNT; i++)
  {
    controllers[i] = new XboxController();
    memset(controllers[i], 0, sizeof(XboxController));
  }
}

void XboxController_Update(float dt)
{
  DWORD dwResult;
  for (DWORD i = 0; i< XUSER_MAX_COUNT; i++)
  {
    XINPUT_STATE state;
    memset(&state, 0, sizeof(XINPUT_STATE));

    // Simply get the state of the controller from XInput.
    dwResult = XInputGetState(i, &state);

    if (dwResult == ERROR_SUCCESS)
    {
      // Controller is connected 
      controllers[i]->isConnected = true;

      controllers[i]->thumbpadX = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) * -1.f + (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) * 1.f;
      controllers[i]->thumbpadY = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) * -1.f + (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) * 1.f;
      controllers[i]->a = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) == XINPUT_GAMEPAD_A;
      controllers[i]->b = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) == XINPUT_GAMEPAD_B;
      controllers[i]->x = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) == XINPUT_GAMEPAD_X;
      controllers[i]->y = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y;
      controllers[i]->back = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) == XINPUT_GAMEPAD_BACK;
      controllers[i]->start = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) == XINPUT_GAMEPAD_START;
      controllers[i]->leftBumper = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == XINPUT_GAMEPAD_LEFT_SHOULDER;
      controllers[i]->rightBumper = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == XINPUT_GAMEPAD_RIGHT_SHOULDER;
      controllers[i]->leftStick = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) == XINPUT_GAMEPAD_LEFT_THUMB;
      controllers[i]->rightStick = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) == XINPUT_GAMEPAD_RIGHT_THUMB;


      if (abs(state.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
        abs(state.Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE){
        controllers[i]->leftThumbX = state.Gamepad.sThumbLX / 32768.0f;
        controllers[i]->leftThumbY = state.Gamepad.sThumbLY / 32768.0f;
      }
      else {
        controllers[i]->leftThumbX = 0.0f;
        controllers[i]->leftThumbY = 0.0f;
      }

      if (abs(state.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
        abs(state.Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE){
        controllers[i]->rightThumbX = state.Gamepad.sThumbRX / 32768.0f;
        controllers[i]->rightThumbY = state.Gamepad.sThumbRY / 32768.0f;
      }
      else {
        controllers[i]->rightThumbX = 0.0f;
        controllers[i]->rightThumbY = 0.0f;
      }

      if (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
        controllers[i]->leftTrigger = state.Gamepad.bLeftTrigger / 255.f;
      else
        controllers[i]->leftTrigger = 0;

      if (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
        controllers[i]->rightTrigger = state.Gamepad.bRightTrigger / 255.f;
      else
        controllers[i]->rightTrigger = 0;
    }
    else
    {
      // Controller is not connected 
      controllers[i]->isConnected = false;
    }
  }
}

void XboxController_Destroy()
{
  for (int i = 0; i < XUSER_MAX_COUNT; i++)
  {
    delete controllers[i];
  }
}

void XboxController_GetController(XboxController** pController, int player)
{
  *pController = controllers[player];
}

void XboxController_Rumble(XboxController* pController, float intensity /*= 1.f*/, int millisecondsToRumble /*= 2000*/)
{
  // TODO: Code
}