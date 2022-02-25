#include <virtuabotixRTC.h>
#include <HX711_ADC.h>
#include <Servo.h>

#define timePin A1
#define weightPin A0
#define APin A4
#define BPin A3
#define CPin A2
#define D0 13
#define D1 12
#define D2 11
#define D3 10
#define mode 8

virtuabotixRTC myRTC(2, 3, 4); // Creation of the Real Time Clock Object

HX711_ADC LoadCell(5, 6); // dt pin, sck pin

Servo motor;

int tlimVal;
int wlimVal;
int temptLim;
int timeLimit = 24;
int weightLimit;
int tempTime = 0;
int timeCtr = 0;

double lightCtr = 0;

void setup()  {
  Serial.begin(9600);

  Serial.println("Setting up values...");
  motor.attach(7);
  
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(1000); // load cells gets 1000ms of time to stabilize
  LoadCell.setCalFactor(350); // Calibarate your LOAD CELL with 100g weight, and change the value according to readings

  pinMode(APin, OUTPUT);
  pinMode(BPin, OUTPUT);
  pinMode(CPin, OUTPUT);

  pinMode(mode,INPUT);
  pinMode(D0,INPUT);
  pinMode(D1,INPUT);
  pinMode(D2,INPUT);
  pinMode(D3,INPUT);
  
  getSwitchVal();
  resetSwitchVal();
  Serial.println("Setup complete!");
}
void loop()  {
  
  getSwitchVal();
  checkTimeLimit();
  myRTC.updateTime();
  //Serial.println(getTimeLimit(tlimVal));
  incTime();
  
  //Serial.print("Weight: ");
  //Serial.println(getWeight());
}

//Function to print the time in the RTC module
void printTime(){
  myRTC.updateTime();
  Serial.print("Current Date / Time: ");
  Serial.print(myRTC.dayofmonth);
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print("  ");
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.print(myRTC.minutes);
  Serial.print(":");
  Serial.println(myRTC.seconds);
}

void dispenseFood(){
  motor.write(0);
  delay(500);
  motor.write(180);
  delay(500);
  Serial.println("Food Dispensed!");
}

void getSwitchVal(){
  tlimVal = analogRead(timePin);
  Serial.println(tlimVal);
  wlimVal = analogRead(weightPin); 
  //Serial.println(wlimVal);
  delay(500);
}

void resetSwitchVal(){
  temptLim = tlimVal;
}

void checkTimeLimit(){
  if(getTimeLimit(tlimVal) != temptLim){
    timeCtr = 0;
    temptLim = getTimeLimit(tlimVal);
    setTimeLimit();
    delay(500);
  }
  if(timeCtr == timeLimit){
    dispenseFood();
    timeCtr = 0;
  }
}

void setTimeLimit(){
  switch (getTimeLimit(tlimVal)) {
  case 1:
    timeLimit = 24;
    lightCtr = 0.2916666667;

    break;
  case 2:
    timeLimit = 12;
    lightCtr = 0.5833333333;
    break;
  case 3:
    timeLimit = 8;
    lightCtr = 0.875;
    break;
  case 4:
    timeLimit = 6;
    lightCtr = 1.166666667;
    break;
  case 5:
    timeLimit = 4;
    lightCtr = 1.75;
    break;
  default:
    break;
  }
}

void incTime(){
  if(tempTime != myRTC.seconds){
    timeCtr++;
    tempTime = myRTC.seconds;
    printTest();
  }
}

float getWeight(){
  LoadCell.update(); // retrieves data from the load cell
  return LoadCell.getData(); // get output value
}

int getTimeLimit(int t){
  if(t >= 1 && t < 204)
    return 1;
  else if(t >= 204 && t < 408)
    return 2;
  else if(t >= 408 && t < 612)
    return 3;
  else if(t >= 612 && t < 816)
    return 4;  
  else if(t >= 816)
    return 5;
  else
    return 0;  
}

int getWeightLimit(int w){
  if(2 >= 1 && w < 102)
    return 1;
  else if(w >= 102 && w < 204)
    return 2;
  else if(w >= 204 && w < 306)
    return 3;
  else if(w >= 408 && w < 510)
    return 4;  
  else if(w >= 510 && w < 612)
    return 5;  
  else if(w >= 612 && w < 714)
    return 6;
  else if(w >= 714 && w < 816)
    return 7;  
  else if(w >= 816 && w < 918)
    return 8;  
  else if(w >= 918 && w < 1020)
    return 9;  
  else if(w >= 1020)
    return 10;
  else
    return 0;  
}

void printTest(){
  Serial.println("=========================================================");
  Serial.print("Time: ");
  Serial.println(getTimeLimit(tlimVal));
  Serial.print("Weight: ");
  Serial.println(getWeight());
  Serial.print("TimeLimit: ");
  Serial.println(timeLimit);
  //Serial.print("Test :");
  //Serial.println(getlightCount());
  convertDTB(getlightCount());
  outputIoT();
  Serial.print("TimeCtr: ");
  Serial.println(timeCtr);
}

int getlightCount(){
  int lim = timeCtr * lightCtr;
  return lim;
}

void outputIoT(){
  Serial.print(digitalRead(D0));
  Serial.print(" ");
  Serial.print(digitalRead(D1));
  Serial.print(" ");
  Serial.print(digitalRead(D2));
  Serial.print(" ");
  Serial.print(digitalRead(D3));
  Serial.print(" ");
  Serial.println("");
}

void convertDTB(int x){
 int a[3] = {0,0,0}, i;
 for(i = 0; x > 0; i++){
  a[i]=x%2;
  x = x/2;
  }

 digitalWrite(APin,a[2]);
 digitalWrite(BPin,a[1]);
 digitalWrite(CPin,a[0]);
 
 Serial.print("Binary: ");
 Serial.print(a[2]);
 Serial.print(a[1]);
 Serial.print(a[0]);
 Serial.println("");
}
