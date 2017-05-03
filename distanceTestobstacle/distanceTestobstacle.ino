/**
 * HC-SR04 Demo
 * Demonstration of the HC-SR04 Ultrasonic Sensor
 * Date: August 3, 2016
 * 
 * Description:
 *  Connect the ultrasonic sensor to the Arduino as per the
 *  hardware connections below. Run the sketch and open a serial
 *  monitor. The distance read from the sensor will be displayed
 *  in centimeters and inches.
 * 
 * Hardware Connections:
 *  Arduino | HC-SR04 
 *  -------------------
 *    5V    |   VCC     
 *    7     |   Trig     
 *    8     |   Echo     
 *    GND   |   GND
 *  
 * License:
 *  Public Domain
 */

#include <math.h>
// Pins for distance sensor
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;
const int HAND_PIN = 6;
// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

//pins for haptic
const int pins[] = {3, 5, 9};//, 6, 10,11};
const int resolution= sizeof(pins)/sizeof(int);
const int range = MAX_DIST / resolution;

//max number of measurements in a 'go'
#define MLENGTH 50

#define TOUCH_PIN 2

void setup() {

  // The Trigger pin will tell the sensor to range find
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(HAND_PIN,OUTPUT);
  pinMode(TOUCH_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
 for (int i = 0; i < resolution; i++) {
    pinMode(pins[i],OUTPUT);
  }
  
  // We'll use the serial monitor to view the sensor output
  Serial.begin(115200);
  Serial.print("array Length:");
  Serial.println(resolution);
}

float transpose(float inmin, float inmax, float outmin, float outmax, float val){
  float diff = inmax - inmin;
  float addthis = val * (outmax - outmin) /diff;
  return outmin + addthis;



}

float invertedTranspose(float x, float y, float a, float b, float v){
  return b - transpose(x,y,a,b,v);
  
}
void handleHaptic(int pw){
 
Serial.print("pw: ");
    Serial.print(pw);
    Serial.print("\t");
  
//  Serial.print("range:");
//  Serial.println(range);
  int order = pw / range;
 Serial.print("order: ");
 Serial.print(order);
 Serial.print("\t");
//  Serial.print("resolution:");
  Serial.print( ((float) (pw % range )) / range);
  Serial.print("%\t");
  for(int i =0; i < resolution; i++){
      digitalWrite(pins[i], i==order);
  }
  Serial.print("hand val:");
  Serial.print((int)invertedTranspose(0, range, 0, 255, pw % range));
  Serial.print("\t");
  analogWrite(HAND_PIN,(int)invertedTranspose(0, range, 0, 255, pw % range));

  //speed of the current one.
  digitalWrite(pins[order],HIGH);
  //how close are we? zero is as close as possible
  int delayVal =  (int) ((((float) (pw % range )) / range ) * 700) ; 
  delay(60);
  digitalWrite(pins[order],LOW);
  delay(delayVal);  
}
float maxRange = 300; //centimeters
void handleHaptic2(float centimeters){
 
 if(centimeters> maxRange){
  return;
 }
//which one vibrates?
float interval = maxRange / resolution;
int p = (int)( centimeters/ interval);
//turn all pins off
for(int i =0; i < resolution; i++){
      digitalWrite(pins[i],LOW);
}
//turn the interesting one on.
digitalWrite(pins[p],HIGH);
delay(60);
digitalWrite(pins[p],LOW);
//be quiet for the percentage in this range.
float distanceInRange = centimeters - p*interval;
delay((int)(round( distanceInRange/interval* 500 )   ));
 

}

void handleOutOfRange(){
   for(int i =0; i < resolution-1; i++){
      digitalWrite(pins[i], LOW);
   }
  digitalWrite(pins[resolution-1],HIGH);
  delay(150);
  digitalWrite(pins[resolution-1],LOW);
  delay(150);
}

void filter (float* input, float* output, int size){
  
}
unsigned long measureDistance(){
//  unsigned long begin = micros();
  unsigned long t1;
  unsigned long t2;
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for pulse on echo pin
  while ( digitalRead(ECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  t1 = micros();
  while ( digitalRead(ECHO_PIN) == 1);
  t2 = micros();
  return t2 - t1;
  }
float last_cmmax = 0;
unsigned long last_cmmaxtime =0;
float last_cmmin = maxRange;
unsigned long last_cmmintime = 0;
unsigned long timediff = 500;

float last_cm =0;
float cmthreshold=5;

void handleRelative(float cm){
  float difference = cm - last_cm;
  if(abs(difference) < cmthreshold){
    return; 
  }
  float percentchange = difference/maxRange;
  Serial.print("difference great enough:");
  Serial.println(difference);
  int minDuration = 10;
  int maxDuration = 50;
  if(percentchange < 0){
    //it got closer!
    for(int i =0; i < resolution; i++){
      digitalWrite(pins[i], HIGH);
      delay(50);
      digitalWrite(pins[i], LOW);
      delay(50);
   }
  }else{
    for(int i =resolution-1; i >= 0; i--){
      digitalWrite(pins[i], HIGH);
      delay(50);
      digitalWrite(pins[i], LOW);
      delay(50);
   }
  }
  
}
float measurements[MLENGTH];
//-------------------------------------------------------------------------------------
void loop() {
  unsigned long pulse_width;
  float cm;
  pulseIn(0,0);
  if(!digitalRead(TOUCH_PIN)){
    return;
  }
  Serial.println("\ntouch");
  int index =0;
  while(digitalRead(TOUCH_PIN)){
    if(index >= MLENGTH){
       break;
       Serial.println("\nout of array space");
    }
    pulse_width = (measureDistance() + measureDistance() + measureDistance() ) / 3;
    // Calculate distance in centimeters and inches. The constants
    // are found in the datasheet, and calculated from the assumed speed 
    //of sound in air at sea level (~340 m/s).
    cm = pulse_width / 58.0;
    if(cm > 800){
      continue;
    }
    measurements[index++] = cm;
    delay(10);
  
  }
  Serial.println("touremoved");
  for(int i =0; i < index; i++){
    Serial.print(measurements[i]);
    Serial.print(", ");
  }
  return;
  float inches;
  unsigned long begin = micros();
  Serial.println("go!");
  delay(300);
  for(int i =0; i < 50; i++){
    pulse_width = (measureDistance() + measureDistance() + measureDistance() ) / 3;
    // Calculate distance in centimeters and inches. The constants
    // are found in the datasheet, and calculated from the assumed speed 
    //of sound in air at sea level (~340 m/s).
    cm = pulse_width / 58.0;
    measurements[i] = cm;
  }
  for(int i = 0; i < MLENGTH; i++){
    Serial.print(measurements[i]);
    Serial.print(", ");
  }
  Serial.println("");
  delay(5000);
  return;
  
//  Serial.print(cm);
//  Serial.println(" cm \t");
//  return;

//  if(cm > last_cmmax){
//    last_cmmax = cm;
//    last_cmmaxtime = millis();
//  }
//  if(cm < last_cmmin){
//    last_cmmin = cm;
//    last_cmmintime = millis();
//  }
  //if the measurement is outdated, 
//  if(millis() - last_cmmaxtime > timediff ) 
  inches = pulse_width / 148.0;

  // Print out results
  if ( pulse_width > MAX_DIST ) {
    Serial.println();
    Serial.println("Out of range");
    //analogWrite(HAND_PIN,0);
    //handleOutOfRange();
  } else {
    if(cm < maxRange){
      handleRelative(cm);
      last_cm = cm;
      delay(60);
    }
      
    Serial.print(cm);
    Serial.println(" cm \t\n\r");
//    Serial.print(inches);
//    Serial.println(" in");

  //  handleHaptic2(cm);
//    Serial.println((int)invertedTranspose(0, MAX_DIST, 0, 255, pulse_width));

  
  // Wait at least 60ms before next measurement
  //delay(90);

  }
  }



