//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <Pixy2.h>
#include <PIDLoop.h>
#include <ZumoMotors.h>

// this limits how fast Zumo travels forward (400 is max possible for Zumo)
#define MAX_TRANSLATE_VELOCITY  150

Pixy2 pixy;
ZumoMotors motors;

PIDLoop panLoop(350, 0, 600, true);
PIDLoop tiltLoop(500, 0, 700, true);
PIDLoop rotateLoop(300, 600, 300, false);
PIDLoop translateLoop(400, 800, 300, false);

void setup()
{
  Serial.begin(115200);
  Serial.print("Starting...\n");
  
  // initialize motor objects
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
  
  // need to initialize pixy object
  pixy.init();
  // user color connected components program
  pixy.changeProg("color_connected_components");
}

// Take the biggest block (blocks[0]) that's been around for at least 30 frames (1/2 second)
// and return its index, otherwise return -1
int16_t acquireBlock()
{
  if (pixy.ccc.numBlocks && pixy.ccc.blocks[0].m_age>30){
    return pixy.ccc.blocks[0].m_index;
  }
  return -1;
}

// Find the block with the given index.  In other words, find the same object in the current
// frame -- not the biggest object, but he object we've locked onto in acquireBlock()
// If it's not in the current frame, return NULL
Block *trackBlock(uint8_t index)
{
  uint8_t i;

  for (i=0; i<pixy.ccc.numBlocks; i++)
  {
    if (index==pixy.ccc.blocks[i].m_index){
      //Serial.println("block[i]: "+String(i));
      return &pixy.ccc.blocks[i];
    }
  }
  return NULL;
}


void loop()
{ 
  
  static int16_t index = -1;
  int32_t panOffset, tiltOffset, headingOffset, left, right;
  Block *block=NULL;
  pixy.ccc.getBlocks();

  if (index==-1) // search....
  {
    Serial.println("Searching_for_block...");
    index = acquireBlock();
    if (index>=0)
      Serial.println("Found_block!");
 }
  // If we've found a block, find it, track it
  if (index>=0){
     block = trackBlock(index);
     block->print();
  }
     

  // If we're able to track it, move motors
  if (block)
  {
    // calculate pan and tilt errors
    panOffset = (int32_t)pixy.frameWidth/2 - (int32_t)block->m_x;
    tiltOffset = (int32_t)block->m_y - (int32_t)pixy.frameHeight/2;  

//    Serial.println("panOffset:"+String(panOffset));
//    Serial.println("tiltOffset:"+String(tiltOffset));

    // calculate how to move pan and tilt servos
    panLoop.update(panOffset);
    tiltLoop.update(tiltOffset);

    // move servos
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);
//    Serial.println("panLoop.m_command:"+String(panLoop.m_command));
//    Serial.println("tiltLoop.m_command:"+String(tiltLoop.m_command));

    // calculate translate and rotate errors
    panOffset += panLoop.m_command - PIXY_RCS_CENTER_POS;
    tiltOffset += tiltLoop.m_command - PIXY_RCS_CENTER_POS - PIXY_RCS_CENTER_POS/2 + PIXY_RCS_CENTER_POS/4;

    rotateLoop.update(panOffset);
    translateLoop.update(-tiltOffset);
//    Serial.println("rotateLoop.m_command:"+String(rotateLoop.m_command));
//    Serial.println("translateLoop.m_command:"+String(translateLoop.m_command));

    // keep translation velocity below maximum
    if (translateLoop.m_command>MAX_TRANSLATE_VELOCITY)
      translateLoop.m_command = MAX_TRANSLATE_VELOCITY;

    // calculate left and right wheel velocities based on rotation and translation velocities
    left = -rotateLoop.m_command + translateLoop.m_command;
    right = rotateLoop.m_command + translateLoop.m_command;
//    Serial.println("leftSpeed:"+String(left));
//    Serial.println("rightSpeed:"+String(right));

    // set wheel velocities
    motors.setLeftSpeed(left);
    motors.setRightSpeed(right);

    // print the block we're tracking -- wait until end of loop to reduce latency
   
  }  
  else // no object detected, stop motors, go into search state
  {
    rotateLoop.reset();
    translateLoop.reset();
    motors.setLeftSpeed(0);
    motors.setRightSpeed(0);
    index = -1; // set search state
    Serial.println("no_object_detected");
  }
  
  String buf;
  
  while(Serial.available()){
    buf=Serial.readStringUntil('\0');
  }

  if(buf[0]=='H'){
    int32_t pgain=0, igain=0, dgain=0;
    int len=buf.length();
    buf=buf.substring(2,len);
    len=len-2;
    String pidLoop=buf.substring(0,buf.indexOf(":"));
    String value=buf.substring(buf.indexOf(":")+1,len);
    int pIndex=value.indexOf(",");
    int iIndex=value.indexOf(pIndex,",");
    String p=value.substring(0,pIndex);
    String i=value.substring(pIndex+1,iIndex);
    String d=value.substring(iIndex+1,value.length());
      pgain=p.toInt();
      igain=i.toInt();
      dgain=d.toInt();
    if(pidLoop.equals("PAN")){
      panLoop.setPIDgain(pgain,igain,dgain);
      Serial.println("set panLoop:"+value);
    }
    else if(pidLoop.equals("TILT")){
      tiltLoop.setPIDgain(pgain,igain,dgain);
      Serial.println("set tiltLoop:"+value);
    }
    else if (pidLoop.equals("ROTATE")){
      rotateLoop.setPIDgain(pgain,igain,dgain);
      Serial.println("set rotateLoop:"+value);
    }
    else if(pidLoop.equals("TRANS")){
      translateLoop.setPIDgain(pgain,igain,dgain);
      Serial.println("set translateLoop:"+value);
    }
    else{
      Serial.println("Wrong command!");
    }
    buf[0]='\0';
  }
}
