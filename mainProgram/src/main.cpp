// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// intakeLeft           motor         12              
// intakeRight          motor         13              
// Controller2          controller                    
// tray                 motor         21              
// arms                 motor         14              
// goalButton           bumper        A               
// Inertial             inertial      2               
// trayButton           bumper        E               
// backLeft             motor         7               
// frontLeft            motor         8               
// backRight            motor         9               
// frontRight           motor         10              
// Vision               vision        15              
// ---- END VEXCODE CONFIGURED DEVICES ----



#include "vex.h"

using namespace vex;
vex::competition Competition;
vex::timer loopTime;
motor_group intake ( intakeLeft, intakeRight );

// vision::signature CUBE_ORANGE (1, 6343, 7765, 7054, -2843, -1635, -2239, 3.000, 0);
// vision::signature CUBE_GREEN (2, -6065, -3709, -4887, -3385, -1461, -2423, 2.500, 0);
// vision::signature CUBE_PURPLE (3, 1401, 2161, 1781, 7625, 9415, 8520, 3.000, 0);
// vision::signature SIG_4 (4, 0, 0, 0, 0, 0, 0, 2.500, 0);
// vision::signature SIG_5 (5, 0, 0, 0, 0, 0, 0, 2.500, 0);
// vision::signature SIG_6 (6, 0, 0, 0, 0, 0, 0, 2.500, 0);
// vision::signature SIG_7 (7, 0, 0, 0, 0, 0, 0, 2.500, 0);
// vex::vision vision1 ( vex::PORT1, 60, CUBE_ORANGE, CUBE_GREEN, CUBE_PURPLE, SIG_4, SIG_5, SIG_6, SIG_7 );

#define down 0
#define low 1
#define high 2

double forwardDistanceP(28.647889757);
double strafeDistanceP(32.243767313);
double strafeSpeedP(1.12551980572);

double stickForward(0);
double stickStrafe(0);
double stickTurn(0);

double autoTray(0);
double autoArms(0);
double autoDrive(0);
double autoTurn(0);
double autoIntake(0);
double autoIntakeSpin(0);

double buttonIntake(0);

double rampDrive(0);

double rampTurn(0);

double visionSteer(0);

double lastTime(0);

int armsPos(down);

bool trayMoving(false);

bool armsMoving(false);

bool turboTurn(false);

bool autoAbort(false);

double trayStick (0);

double armsStick (0);


/*===========================================================================*/

int drivetrainTask()
{
  while(1)
  {
  double moveForward = forwardOutput + stickForward + autoDrive;
  double moveStrafe  = strafeOutput  + stickStrafe;
  double moveTurn    = turnOutput    + stickTurn + visionSteer;

  frontRight.spin(fwd, moveForward - moveStrafe - moveTurn, pct);
  frontLeft.spin(fwd,  moveForward + moveStrafe + moveTurn, pct);
  backRight.spin(fwd,  moveForward + moveStrafe - moveTurn, pct);
  backLeft.spin(fwd,   moveForward - moveStrafe + moveTurn, pct);
  }

  return(0);
}


void intakeSpin(double degrees, double percent)
{
  double pos = fabs(intake.position(deg));
  autoIntakeSpin = percent;
  waitUntil(autoAbort || fabs(intake.position(deg)) - pos > fabs(degrees));
  autoIntakeSpin = 0;
}


void intakeIn()
{
  if(buttonIntake == 100) buttonIntake = 0;
  else buttonIntake = 100;
}

void intakeOut()
{
  if(buttonIntake == -100) buttonIntake = 0;
  else buttonIntake = -100;
}

void intakeInSlow()
{
  if(buttonIntake == 20) buttonIntake = 0;
  else buttonIntake = 20;
}

void intakeOutSlow()
{
  if(buttonIntake == -20) buttonIntake = 0;
  else buttonIntake = -20;

}

void trayUp() {
    if(!trayMoving) {
      trayMoving = true;
      buttonIntake = 0;
      while (tray.position(deg) < 750 && !autoAbort) {
        autoTray = ( tray.position(deg) * -0.1 + 100 );
        if (tray.position(deg) < 300) {
          autoIntake = -5;
        }
        vex::task::sleep(20);
      }
    autoTray = 0;
    autoIntake = 0;
    trayMoving = false;
  }
}

void trayDown()
{
  if(!trayMoving) {
    trayMoving = true;
    buttonIntake = 0;
    while (tray.position(deg) > 0 && !autoAbort){
      autoTray = ( ( tray.position(deg) * -0.07 + 100 ) * -1);
      vex::task::sleep(20);
    }
    autoIntake = -50;
    autoDrive = -25;
    double time = Brain.Timer;
    waitUntil(autoAbort || Brain.Timer - time > 400);
  
    autoTray = 0;
    autoIntake = 0;
    autoDrive = 0;
    trayMoving = false;
  }
}

void armsHigh ()
{
  if(!armsMoving) {
      armsMoving = true;
      buttonIntake = 0;
      while (arms.position(deg) < 900 && !autoAbort) 
      {
        autoArms = ( arms.position(deg) * -0.1 + 150 );
        vex::task::sleep(20);
      }
    autoArms = 0;
    armsMoving = false;
    arms.stop(hold);
    armsPos = high;
  }
}

void armsLow ()
{
  if(!armsMoving) {
      armsMoving = true;
      buttonIntake = 0;
      while (arms.position(deg) < 750 && !autoAbort) 
      {
        autoArms = ( arms.position(deg) * -0.1 + 150 );
        vex::task::sleep(20);
      }
    autoArms = 0;
    armsMoving = false;
    arms.stop(hold);
    armsPos = low;
  }
}

void armsDown ()
{
    if(!armsMoving) {
    armsMoving = true;
    buttonIntake = 0;
    while (arms.position(deg) > 0 && !autoAbort)
    {
      autoArms = ( ( arms.position(deg) * -0.07 + 100 ) * -1);
      vex::task::sleep(20);
    }
    autoArms = 0;
    autoIntake = 0;
    autoDrive = 0;
    armsMoving = false;
  arms.stop(brake);
  armsPos = down;
  }
}

void intakeAdvance ()
{
  switch(armsPos) {
    case down:
      intakeSpin(180, -100);
    break;
    case low:
      intakeSpin(360, -100);
    break;
    case high:
      intakeSpin(360, -100);
      autoArms = -30;
      waitUntil(autoAbort || arms.position(deg) < 750);
    break;
  }
}

void auton()
{
  autonInitialize();

  forwardFunction( 0,  34, 5, 50, 3, true);
  forwardFunction(34, -15, 5, 50, 3, true);
  forwardFunction(19, -15, 5, 50, 3, false);
  strafeFunction(  0, -24, 5, 50, 3, true);
  forwardFunction( 4,  30, 5, 50, 3, true);


  autonStop();
}

void visionSteerFunction ()
{
  Inertial.resetRotation();
  Vision.takeSnapshot(Vision__SIG_1);
  while (Controller1.ButtonL2.pressing()) {
    Vision.takeSnapshot(Vision__SIG_1);
    if (/*Inertial.yaw() < 25 && Inertial.yaw() > -25 && */ Vision.largestObject.height < 100) {
      visionSteer = -(Vision.largestObject.centerX - 165) * 0.5;
      autoIntake = 100;
    } else {
      visionSteer = 0;
      autoIntake = 0;
    }
  }
  visionSteer = 0;
  autoIntake = 0;

}


/*===========================================================================*/

void pre_auton( void ) {
  // Inertial.calibrate();
  menuLcdDraw();
  controllerDraw();
  tray.setStopping(brake);
  tray.setTimeout(1, seconds);
  tray.setPosition(-66, deg);

  Controller1.ButtonX.pressed(trayUp);
  Controller1.ButtonA.pressed(trayDown);
  Controller1.ButtonB.pressed(intakeInSlow);
  Controller1.ButtonR2.pressed(intakeIn);
  Controller1.ButtonL2.pressed(visionSteerFunction);
  Controller1.ButtonR1.pressed(intakeOutSlow);
  Controller1.ButtonUp.pressed(armsHigh);
  Controller1.ButtonLeft.pressed(armsLow);
  Controller1.ButtonDown.pressed(armsDown);
  Controller1.ButtonRight.pressed(intakeAdvance);




  // Drivetrain.setTimeout(3, seconds);
  arms.setTimeout(1, seconds);
  arms.setStopping(brake);

  intake.setTimeout(1, seconds);
  intake.setStopping(hold);



  Inertial.calibrate();

  task task1 = task( drivetrainTask );


  while (!Competition.isAutonomous() || !Competition.isEnabled()) {
    menuLcdTouch();
    vex::task::sleep(20);
  }
}

/*===========================================================================*/

void autonomous( void ) {
  autonIndicator();
  autonInitialize();
  
  autonRun(currentPage);

  wait(0.5, sec);

  autonStop();

  return;
}

/*===========================================================================*/

void usercontrol( void ) {
  // Brain.loopTimer.
  loopTime.reset();
  autonStop();

  while (1) {
    // if (axis3Prop + autoDrive > Drivetrain.velocity(pct) + 20) {
    //   rampDrive = rampDrive + loopTime.time()/5;
    // } else if (axis3Prop + autoDrive < Drivetrain.velocity(pct) - 20) {
    //   rampDrive = rampDrive - loopTime.time()/5;
    // } else if (axis3Prop + autoDrive <= Drivetrain.velocity(pct) + 20 && axis3Prop + autoDrive >= Drivetrain.velocity(pct) - 20) {
    //   rampDrive = axis3Prop + autoDrive;
    // }

    // if (Controller1.ButtonUp.pressing()) 
    // {
    //   autoTurn = vision1.largestObject.centerX;
    // }



    // loopTime.reset();

    // if(leftIntakeSwitch.pressing() && rightIntakeSwitch.pressing() ){
    //   intakeJammed = 1;
    // } else {
    //   intakeJammed = -1;
    // }


    stickForward = Controller1.Axis2.position();
    stickStrafe = Controller1.Axis1.position() * strafeSpeedP;
    stickTurn = Controller1.Axis4.position() * (Controller1.Axis3.position() + 120) / 200;



    intakeLeft.spin(fwd, (-Controller2.Axis3.position() - Controller2.Axis4.position() + buttonIntake + autoIntake + autoIntakeSpin), percent);
    intakeRight.spin(fwd, (-Controller2.Axis3.position() + Controller2.Axis4.position() + buttonIntake + autoIntake + autoIntakeSpin), percent);


    if(Controller2.Axis2.position() < - 5 || Controller2.Axis2.position() > 5)
    {
      if(Controller2.ButtonR2.pressing()) 
      {
        trayStick = Controller2.Axis2.position();
      } else {
        trayStick = 0;
        armsStick = Controller2.Axis2.position();
      }
    } else {
      trayStick = 0;
      armsStick = 0;
    }

    arms.spin(fwd, armsStick + autoArms, pct);


    tray.spin(fwd, autoTray + trayStick, percent);

    if (intakeLeft.torque() > 1.05 || intakeRight.torque() > 1.05)
    {
      Controller1.rumble(".");
      Controller2.rumble(".");

    }

    autoAbort = Controller1.ButtonL1.pressing();

    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print(autoIntakeSpin);
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print(buttonIntake);
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print(Vision.largestObject.height);
    vex::task::sleep(20); //Sleep the task for a short amount of time to prevent wasted resources. 
  }
}

/*===========================================================================*/

int main() {
  vexcodeInit();
  //Set up callbacks for autonomous and driver control periods.
  Competition.autonomous( autonomous );
  
  //Run the pre-autonomous function. 
  pre_auton();
      
  //Prevent main from exiting with an infinite loop.                        
  while(1) {
    vex::task::sleep(100);//Sleep the task for a short amount of time to prevent wasted resources.
  }    
       
}