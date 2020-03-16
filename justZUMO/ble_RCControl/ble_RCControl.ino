
#include <ZumoShield.h>

#define LED_PIN       13 // user LED pin

#define THROTTLE_MAX_SPEED             100 // max motor speed
#define STEERING_MAX_SPEED  300
#define PULSE_WIDTH_DEADBAND  35 // pulse width difference from 1500 us (microseconds) to ignore (to compensate for control centering offset)
#define PULSE_WIDTH_RANGE     200 // pulse width difference from 1500 us to be treated as full scale input (for example, a value of 350 means
                                  //   any pulse width <= 1150 us or >= 1850 us is considered full scale)


void setup()
{
//  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  // uncomment one or both of the following lines if your motors' directions need to be flipped
//  motors.flipLeftMotor(true);
//  motors.flipRightMotor(true);
}
byte buf[4];
byte th[2];
byte st[2];
int throttle=0,steering=0;

void loop()
{
 
  if(Serial.available()){
    if(Serial.read()=='H'){
      Serial.readBytes(buf,3);

//      Serial.println((int)buf[1]);
//      Serial.println((int)buf[2]);
      if(buf[2]=='0')
        buf[2]=0;
      if(buf[1]=='0')
        buf[1]=0;
//      Serial.println("==");
     
    if(buf[0]=='T'){
      th[0]=buf[1];
      th[1]=buf[2];
      
     // throttle=map(throttle,0,1023,1150,1850);
      Serial.println(throttle);
    }else if(buf[0]=='S'){
      st[0]=buf[1];
      st[1]=buf[2];
   //   steering=map(steering,0,1023,1150,1850);
      Serial.println(steering);
    }else{
      Serial.println("not defined packet!!");
    }
   }
    for(int i=0;i<4;i++){
        buf[i]=0;
    }
  Serial.println("=======");
  }

  throttle=th[1]*256+th[0];
  steering=st[1]*256+st[0];

  int left_speed, right_speed;

  if (throttle > 0 && steering > 0)
  {
    // both RC signals are good; turn on LED
//    digitalWrite(LED_PIN, HIGH);

    throttle -=515;
    steering -=515;
    // RC signals encode information in pulse width centered on 1500 us (microseconds); subtract 1500 to get a value centered on 0

    // apply deadband
    if (abs(throttle) <= PULSE_WIDTH_DEADBAND)
      throttle = 0;
    if (abs(steering) <= PULSE_WIDTH_DEADBAND)
      steering = 0;

    // mix throttle and steering inputs to obtain left & right motor speeds
    left_speed = (throttle * THROTTLE_MAX_SPEED / PULSE_WIDTH_RANGE) - (steering * STEERING_MAX_SPEED / PULSE_WIDTH_RANGE);
    right_speed = (throttle * THROTTLE_MAX_SPEED / PULSE_WIDTH_RANGE) + (steering * STEERING_MAX_SPEED / PULSE_WIDTH_RANGE);

    // cap speeds to max
    left_speed = min(max(left_speed, -STEERING_MAX_SPEED), STEERING_MAX_SPEED);
    right_speed = min(max(right_speed, -STEERING_MAX_SPEED), STEERINGx_MAX_SPEED);
    
  }
  else
  {
    // at least one RC signal is not good; turn off LED and stop motors
//    digitalWrite(LED_PIN, LOW);

//    left_speed = 0;
//   right_speed = 0;
  }

  ZumoMotors::setSpeeds(left_speed, right_speed);
}
