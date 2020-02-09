#include "vex.h"

using namespace vex;

void deployTray()
{
  forwardFunction(0, 6, 5, 20, 5);
  tray.rotateTo(180, deg, 100, rpm);
  tray.spinFor(3, sec, -20, rpm);
  tray.resetPosition();
  // forwardFunction(6, -24, 5, 50, 5);
  // forwardFunction(-12, 1, 100, 100, 100);

  // intake.spin(reverse, 200, rpm);
  // wait(1, sec);
  // intake.spin(fwd, 200, rpm);
  // wait(1, sec);
  // intake.stop();
}

void auton5()
{
  forwardFunction(0, 20, 5, 50, 5);
  forwardFunction(20, -20, 5, 50, 5);
  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("5");
 
}

void auton11()
{
  forwardFunction(0, 2, 5, 50, 5);

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(2, 1);
  Controller1.Screen.print("11");
 
}

void autonRun(int auton)
{
  switch(auton)
  {
    case 0:
    deployTray();
    break;
    case 5:
    auton5();
    break;
    case 11:
    auton11();
    break;
  }
}