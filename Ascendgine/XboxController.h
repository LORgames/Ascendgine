#pragma once

struct XboxController
{
  float rightThumbX;
  float rightThumbY;
  float leftThumbX;
  float leftThumbY;
  float thumbpadX;
  float thumbpadY;

  bool isConnected;
  bool leftStick;
  bool rightStick;
  bool a;
  bool b;
  bool x;
  bool y;
  bool start;
  bool back;
  bool home;

  bool rightBumper;
  bool leftBumper;
  float rightTrigger;
  float leftTrigger;

  int controllerID;
  float hapticFeedbackTimeRemaining;
};

void XboxController_Init();
void XboxController_Update(float dt);
void XboxController_Destroy();

void XboxController_GetController(XboxController** pController, int player);
void XboxController_Rumble(int player, float intensity = 1.f, int millisecondsToRumble = 2000);