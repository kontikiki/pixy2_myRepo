import controlP5.*;

ControlP5 cp5;
float PIDvalue=400.0;


void setup(){
  size(400,400);
  noStroke();
  
  cp5=new ControlP5(this);
  
  cp5.addSlider("sliderPgain",0,1000,400,100,140,100,30);
  cp5.addSlider("sliderIgain",0,1000,0,100,190,100,30);
  cp5.addSlider("sliderDgain",0,1000,400,100,240,100,30);
}

void draw(){
  background(100);
  fill(200);
  
}


public void sliderPgain(float value){
  Textfield txt=((Textfield)cp5.getController("pGain"));
  txt.setValue(""+value);
}

public void pGain(String value) {
  println("### got an event from pGain : "+value);
}

public void sliderIgain(float value){
  Textfield txt=((Textfield)cp5.getController("iGain"));
  txt.setValue(""+value);
}

public void iGain(String value) {
  println("### got an event from iGain : "+value);
}

public void sliderDgain(float value){
  Textfield txt=((Textfield)cp5.getController("dGain"));
  txt.setValue(""+value);
}

public void dGain(String value) {
  println("### got an event from dGain : "+value);
}


public void controlEvent(ControlEvent theEvent) {
  println("got a control event from controller with id "+theEvent.getId());
     PIDvalue= (float)(theEvent.getController().getValue());
     println("### got an event from dGain : "+PIDvalue);
}
