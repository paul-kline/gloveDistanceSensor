#include <math.h>
// Pins
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;
const int HAND_PIN = 6;
// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

const int pins[] = {3, 5, 9};//, 6, 10,11};
const int resolution= sizeof(pins)/sizeof(int);
const int range = MAX_DIST / resolution;

void setup() {

  // The Trigger pin will tell the sensor to range find
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(HAND_PIN,OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  for (int i = 0; i < resolution; i++) {
    pinMode(pins[i],OUTPUT);
  }
  
  // We'll use the serial monitor to view the sensor output
//  Serial.begin(115200);
//  Serial.print("array Length:");
//  Serial.println(resolution);
}

float transpose(float inmin, float inmax, float outmin, float outmax, float val){
  float diff = inmax - inmin;
  float addthis = val * (outmax - outmin) /diff;
  return outmin + addthis;
}

float invertedTranspose(float x, float y, float a, float b, float v){
  return b - transpose(x,y,a,b,v);
}

float maxRange = 300; //centimeters

void handleHaptic(float centimeters){

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
  delay((int)(round( distanceInRange/interval* 500 )));
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

//-----------------------------------------------
unsigned long measureDistance(){
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH,20000); 
}

//-----------------------------------------------
unsigned long getAvePulse(int count){
  unsigned long sum = 0;
  int successes = 0;
  for(int i =0; i < count; i++){
    unsigned long x = measureDistance();
    if(x != 0){
      successes++;
      sum += x;
    }
  }
  return sum/successes;
}

//-----------------------------------------------
void loop() {
  unsigned long pulse_width;
  float cm;
  float inches;
  unsigned long begins = micros();
  pulse_width = getAvePulse(3);
  // Calculate distance in centimeters and inches. The constants
  // are found in the datasheet, and calculated from the assumed speed 
  //of sound in air at sea level (~340 m/s).
  cm = pulse_width / 58.0;
  inches = pulse_width / 148.0;

  // Print out results
  if ( pulse_width > MAX_DIST  || cm > maxRange) {
//    Serial.println();
//    Serial.println("Out of range");
    //analogWrite(HAND_PIN,0);
    //handleOutOfRange();
  } else {
//    Serial.print(cm);
//    Serial.print(" cm \t");
//    Serial.print(inches);
//    Serial.println(" in");

    handleHaptic(cm);
//    Serial.println((int)invertedTranspose(0, MAX_DIST, 0, 255, pulse_width));

  
  // Wait at least 60ms before next measurement
  //delay(90);

  }
  }



