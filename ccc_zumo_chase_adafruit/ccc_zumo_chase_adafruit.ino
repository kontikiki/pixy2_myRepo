#include <SPI.h>  
#include <Pixy2.h>
 
#include <ZumoMotors.h>
 
#define X_CENTER    ((pixy.frameWidth)/2)
#define Y_CENTER    ((pixy.frameHeight)/2)
#define RCS_MIN_POS     0L
#define RCS_MAX_POS     1000L
#define RCS_CENTER_POS ((RCS_MAX_POS-RCS_MIN_POS)/2)
 
//---------------------------------------
// Servo Loop Class
// A Proportional/Derivative feedback
// loop for pan/tilt servo tracking of
// blocks.
// (Based on Pixy CMUcam5 example code)
//---------------------------------------
class ServoLoop
{
public:
 ServoLoop(int32_t proportionalGain, int32_t derivativeGain);
 
 void update(int32_t error);
 
 int32_t m_command;
 int32_t m_prevError;
 int32_t m_proportionalGain;
 int32_t m_derivativeGain;
};
 
// ServoLoop Constructor
ServoLoop::ServoLoop(int32_t proportionalGain, int32_t derivativeGain)
{
 m_command = RCS_CENTER_POS;
 m_proportionalGain = proportionalGain;
 m_derivativeGain = derivativeGain;
 m_prevError = 0x80000000L;
}
 
// ServoLoop Update 
// Calculates new output based on the measured
// error and the current state.
void ServoLoop::update(int32_t error)
{
 long int velocity;
 char buf[32];
 if (m_prevError!=0x80000000)
 { 
  velocity = (error*m_proportionalGain + (error - m_prevError)*m_derivativeGain)>>10;
 
  m_command += velocity;
  if (m_command>RCS_MAX_POS) 
  {
   m_command = RCS_MAX_POS; 
  }
  else if (m_command<RCS_MIN_POS) 
  {
   m_command = RCS_MIN_POS;
  }
 }
 m_prevError = error;
}
// End Servo Loop Class
//---------------------------------------
 
Pixy2 pixy;  // Declare the camera object
 
ServoLoop panLoop(200, 200);  // Servo loop for pan
ServoLoop tiltLoop(150, 200); // Servo loop for tilt
 
ZumoMotors motors;  // declare the motors on the zumo
 
//---------------------------------------
// Setup - runs once at startup
//---------------------------------------
void setup()
{
 Serial.begin(115200);
 Serial.print("Starting...\n");
 
 pixy.init();
}
 
uint32_t lastBlockTime = 0;
 
//---------------------------------------
// Main loop - runs continuously after setup
//---------------------------------------
void loop()
{ 
 uint16_t blocks;
 pixy.ccc.getBlocks();
 blocks=pixy.ccc.numBlocks;
 // If we have blocks in sight, track and follow them
 if (blocks)
 {
  int trackedBlock = TrackBlock(blocks);
  FollowBlock(trackedBlock);
  lastBlockTime = millis();
 }  
 else if (millis() - lastBlockTime > 100)
 {
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
  ScanForBlocks();
 }
}
 
int oldX, oldY, oldSignature;
 
//---------------------------------------
// Track blocks via the Pixy pan/tilt mech
// (based in part on Pixy CMUcam5 pantilt example)
//---------------------------------------
int TrackBlock(int blockCount)
{
 int trackedBlock = 0;
 long maxSize = 0;
 
 Serial.print("blocks =");
 Serial.println(blockCount);
 
 for (int i = 0; i < blockCount; i++)
 {
  if ((oldSignature == 0) || (pixy.ccc.blocks[i].m_signature == oldSignature))
  {
   long newSize = pixy.ccc.blocks[i].m_height * pixy.ccc.blocks[i].m_width;
   if (newSize > maxSize)
   {
    trackedBlock = i;
    maxSize = newSize;
   }
  }
 }
 
 int32_t panError = X_CENTER - pixy.ccc.blocks[trackedBlock].m_x;
 int32_t tiltError = pixy.ccc.blocks[trackedBlock].m_y - Y_CENTER;
 
 panLoop.update(panError);
 tiltLoop.update(tiltError);
 
 pixy.setServos(panLoop.m_command, tiltLoop.m_command);
 
 oldX = pixy.ccc.blocks[trackedBlock].m_x;
 oldY = pixy.ccc.blocks[trackedBlock].m_y;
 oldSignature = pixy.ccc.blocks[trackedBlock].m_signature;
 return trackedBlock;
}
 
//---------------------------------------
// Follow blocks via the Zumo robot drive
//
// This code makes the robot base turn 
// and move to follow the pan/tilt tracking
// of the head.
//---------------------------------------
int32_t size = 400;
void FollowBlock(int trackedBlock)
{
 int32_t followError = RCS_CENTER_POS - panLoop.m_command;  // How far off-center are we looking now?
 
 // Size is the area of the object.
 // We keep a running average of the last 8.
 size += pixy.ccc.blocks[trackedBlock].m_width * pixy.ccc.blocks[trackedBlock].m_height; 
 size -= size >> 3;
 
 // Forward speed decreases as we approach the object (size is larger)
 int forwardSpeed = constrain(400 - (size/256), -100, 400);  
 
 // Steering differential is proportional to the error times the forward speed
 int32_t differential = (followError + (followError * forwardSpeed))>>8;
 
 // Adjust the left and right speeds by the steering differential.
 int leftSpeed = constrain(forwardSpeed + differential, -400, 400);
 int rightSpeed = constrain(forwardSpeed - differential, -400, 400);
 
 // And set the motor speeds
 motors.setLeftSpeed(leftSpeed);
 motors.setRightSpeed(rightSpeed);
}
 
//---------------------------------------
// Random search for blocks
//
// This code pans back and forth at random
// until a block is detected
//---------------------------------------
int scanIncrement = (RCS_MAX_POS - RCS_MIN_POS) / 150;
uint32_t lastMove = 0;
 
void ScanForBlocks()
{
 if (millis() - lastMove > 20)
 {
  lastMove = millis();
  panLoop.m_command += scanIncrement;
  if ((panLoop.m_command >= RCS_MAX_POS)||(panLoop.m_command <= RCS_MIN_POS))
  {
   tiltLoop.m_command = random(RCS_MAX_POS * 0.6, RCS_MAX_POS);
   scanIncrement = -scanIncrement;
   if (scanIncrement < 0)
   {
    motors.setLeftSpeed(-250);
    motors.setRightSpeed(250);
   }
   else
   {
    motors.setLeftSpeed(+180);
    motors.setRightSpeed(-180);
   }
   delay(random(250, 500));
  }
 
  pixy.setServos(panLoop.m_command, tiltLoop.m_command);
 }
}
