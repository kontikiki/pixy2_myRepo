
import processing.serial.*;
import java.util.List;
import java.util.ArrayList;
import controlP5.*;
ControlP5 cp5;

Serial myPort;  // Create object from Serial class
String message;
String all;
String param;
String pid;
int panOffset,tiltOffset;
int panLoop_command,tiltLoop_command;
int rotateLoop_command,translateLoop_command;
int leftSpeed,rightSpeed;
int sig,x,y,wid,hei,index,age;
int panPgain=350,panIgain=0,panDgain=600;
int tiltPgain=500,tiltIgain=0,tiltDgain=700;
int rotatePgain=300,rotateIgain=600,rotateDgain=300;
int transPgain=400,transIgain=800,transDgain=300;
List<String> dataList;
void setup() 
{
  dataList=new ArrayList<String>();
  dataList.add("panOffset");
  dataList.add("tiltOffset");
  dataList.add("panLoop.m_command");
  dataList.add("tiltLoop.m_command");
  dataList.add("rotateLoop.m_command");
  dataList.add("translateLoop.m_command");
  dataList.add("leftSpeed");
  dataList.add("rightSpeed");
  dataList.add("sig");
  dataList.add("x");
  dataList.add("y");
  dataList.add("width");
  dataList.add("height");
  dataList.add("index");
  dataList.add("age");
  
  size(1264,1000);
  noStroke();
  
  cp5=new ControlP5(this);
  cp5.addSlider("PAN_Pgain",0,1000,350,50,852,200,30).setId(1);
  cp5.addSlider("PAN_Igain",0,1000,0,50,902,200,30).setId(2);
  cp5.addSlider("PAN_Dgain",0,1000,600,50,952,200,30).setId(3);
  
  cp5.addSlider("TILT_Pgain",0,1000,500,320,852,200,30).setId(4);
  cp5.addSlider("TILT_Igain",0,1000,0,320,902,200,30).setId(5);
  cp5.addSlider("TILT_Dgain",0,1000,700,320,952,200,30).setId(6);
  
  cp5.addSlider("ROTATE_Pgain",0,1000,300,590,852,200,30).setId(7);
  cp5.addSlider("ROTATE_Igain",0,1000,600,590,902,200,30).setId(8);
  cp5.addSlider("ROTATE_Dgain",0,1000,300,590,952,200,30).setId(9);
  
  cp5.addSlider("TRANSLATE_Pgain",0,1000,400,860,852,200,30).setId(10);
  cp5.addSlider("TRANSLATE_Igain",0,1000,800,860,902,200,30).setId(11);
  cp5.addSlider("TRANSLATE_Dgain",0,1000,300,860,952,200,30).setId(12);
  
  
  String portName = Serial.list()[1];
  println(portName);
  myPort = new Serial(this, portName, 115200);
  
  param="PAN";
  pid=Integer.toString(panPgain)+","+Integer.toString(panIgain)+","+Integer.toString(panDgain);
  all='H'+" "+param+":"+pid;
  myPort.write(all);
  
  param="TILT";
  pid=Integer.toString(tiltPgain)+","+Integer.toString(tiltIgain)+","+Integer.toString(tiltDgain);
  all='H'+" "+param+":"+pid;
  myPort.write(all);
  
  param="ROTATE";
  pid=Integer.toString(rotatePgain)+","+Integer.toString(rotateIgain)+","+Integer.toString(rotateDgain);
  all='H'+" "+param+":"+pid;
  myPort.write(all);
  
  param="TRANS";
  pid=Integer.toString(transPgain)+","+Integer.toString(transIgain)+","+Integer.toString(transDgain);
  all='H'+" "+param+":"+pid;
  myPort.write(all);
}

void draw()
{
  processMessages();
  updateUI();
}

public void PAN_Pgain(int value){
  panPgain=value;
}

public void PAN_Igain(int value){
  panIgain=value;
}

public void PAN_Dgain(int value){
  panDgain=value;
}


public void TILT_Pgain(int value){
  tiltPgain=value;
}

public void TILT_Igain(int value){
  tiltIgain=value;
}

public void TILT_Dgain(int value){
  tiltDgain=value;
}


public void ROTATE_Pgain(int value){
  rotatePgain=value;
}

public void ROTATE_Igain(int value){
  rotateIgain=value;
}

public void ROTATE_Dgain(int value){
  rotateDgain=value;
}


public void TRANSLATE_Pgain(int value){
  transPgain=value;
}

public void TRANSLATE_Igain(int value){
  transIgain=value;
}

public void TRANSLATE_Dgain(int value){
  transDgain=value;
}

public void controlEvent(ControlEvent theEvent) {
  
     switch(theEvent.getId()) {
    case(1): // numberboxA is registered with id 1
      param="PAN";
      pid=Integer.toString(panPgain)+","+Integer.toString(panIgain)+","+Integer.toString(panDgain);
    break;
    case(2):  // numberboxB is registered with id 2
    param="PAN";
    pid=Integer.toString(panPgain)+","+Integer.toString(panIgain)+","+Integer.toString(panDgain);
    break;
    case(3):
    param="PAN";
    pid=Integer.toString(panPgain)+","+Integer.toString(panIgain)+","+Integer.toString(panDgain);
    break;
    case(4):
    param="TILT";
    pid=Integer.toString(tiltPgain)+","+Integer.toString(tiltIgain)+","+Integer.toString(tiltDgain);
    break;
    case(5):
    param="TILT";
    pid=Integer.toString(tiltPgain)+","+Integer.toString(tiltIgain)+","+Integer.toString(tiltDgain);
    break;
    case(6):
    param="TILT";
    pid=Integer.toString(tiltPgain)+","+Integer.toString(tiltIgain)+","+Integer.toString(tiltDgain);
    break;
    case(7):
    param="ROTATE";
    pid=Integer.toString(rotatePgain)+","+Integer.toString(rotateIgain)+","+Integer.toString(rotateDgain);
    break;
    case(8):
    param="ROTATE";
    pid=Integer.toString(rotatePgain)+","+Integer.toString(rotateIgain)+","+Integer.toString(rotateDgain);
    break;
    case(9):
    param="ROTATE";
    pid=Integer.toString(rotatePgain)+","+Integer.toString(rotateIgain)+","+Integer.toString(rotateDgain);
    break;
    case(10):
    param="TRANS";
    pid=Integer.toString(transPgain)+","+Integer.toString(transIgain)+","+Integer.toString(transDgain);
    break;
    case(11):
    param="TRANS";
    pid=Integer.toString(transPgain)+","+Integer.toString(transIgain)+","+Integer.toString(transDgain);
    break;
    case(12):
    param="TRANS";
    pid=Integer.toString(transPgain)+","+Integer.toString(transIgain)+","+Integer.toString(transDgain);
    break;
    
    default:
    println("got a control event from controller with "+theEvent.getValue());
  }  
all="H"+" "+param+":"+pid;
  myPort.write(all);
}

void processMessages(){
  while(myPort.available()>0){
    message=myPort.readStringUntil('\n');
    if(message!=null){
      //print(message);
      
       message=message.substring(0,(message.length()-1));
       if(message.contains(":")){
         String value;
         int label=0;
          String [] data=message.split(":");
          value=data[1].trim();
          label=dataList.indexOf(data[0]);
            switch(label){
              case 0:
                panOffset=Integer.parseInt(value);
                break;
              case 1:
              tiltOffset=Integer.parseInt(value);
              break;
              case 2:
              panLoop_command=Integer.parseInt(value);
              break; 
              case 3:
              tiltLoop_command=Integer.parseInt(value);
              break;
              case 4:
              rotateLoop_command=Integer.parseInt(value);
              break;
              case 5:
              translateLoop_command=Integer.parseInt(value);
              break;
              case 6:
              leftSpeed=Integer.parseInt(value);
              break;
              case 7:
              rightSpeed=Integer.parseInt(value);
              break;
              case 8:
              sig=Integer.parseInt(value);
              break;
              case 9:
              println("x:"+value);
              x=Integer.parseInt(value);
              break;
              case 10:
              println("y:"+value);
        y=Integer.parseInt(value);
        break;
              case 11:
              println("width:"+value);
        wid=Integer.parseInt(value);
        break;
              case 12:
              println("height:"+value);
        hei=Integer.parseInt(value);
        break;
              case 13:
              index=Integer.parseInt(value);
              break;
              case 14:
              age=Integer.parseInt(value);
              break;
              default:
              break;
            }
       }
       else{
        print(message);
      }//end of if(contains(":"))
    }//end of if(message!=null)
  }//end of while
}

void updateUI(){
  background(100);
  fill(200);
  rectMode(CENTER);
  rect(x*4,y*4,wid*4,hei*4);
}
