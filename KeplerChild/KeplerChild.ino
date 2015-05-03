
#define I2CADDR 5 //2 will map to the first servo
                  //3 will map to the second servo, etc.
#include <math.h>
#include <Wire.h>

float eccentricity;
float semimajorAxis;
float Ra;
float Rp;
float velocity = 1.0f;
float velocity_MAX;
float velocity_MIN;

float rVals[250];

int Rindex;

unsigned long time;

boolean goingDown = false;

void setup()
{
  Wire.begin(I2CADDR);                // join i2c bus with I2CADDR
  Wire.onRequest(requestEvent); // register event
  //really random seed.
  randomSeed(analogRead(0));
  for (int i = 0; i < 100; i++){
    randomSeed(analogRead(0) * random(1024));
  }
  eccentricity = (float) 2/PI * atan((double)random(100)/100); //gets a nice distribution.
  semimajorAxis = random(50,100); //some values from 50-100.
  Ra = semimajorAxis*(1+eccentricity);
  Rp = semimajorAxis*(1-eccentricity);
  for (int i = 0; i < 250; i ++){//precompute distances from the sun and corresponding velocities
      //at 1/250 resolution -- could probably change this 
    rVals[i] = map(i,0,250,Rp,Ra);

    //tVals[i] = i;
    //Serial.println(2.0f / rVals[i] - 1.0f/semimajorAxis);
    //vVals[i] = sqrt(2.0f / rVals[i]) - (1.0f/semimajorAxis);
    //Serial.print("completed ");Serial.print(i/250);Serial.println("%");
 }
 velocity_MAX = sqrt((2.0f / Rp) - (1.0f/semimajorAxis));
 velocity_MIN = sqrt((2.0f / Ra) - (1.0f/semimajorAxis));
  
  Rindex = random(250); //start at a random position on the orbit
    
  //set up output pins for display (if we're limited by pins, it would be good to use a shift register instead)
  for (int i =0; i < 10; i ++){
    pinMode(i, OUTPUT);
  }
  
  time = millis();
}

void loop()
{

  //we only square velocity here to accentuate the differences. realistically, you only want to use velocity.
  if (millis() - time > (5 / sq(velocity))){ //after we spend long enough at one phase, we advance to the next.
    if (Rindex <= 0 || Rindex >= (249)){ //at each end, we reverse direction.
      goingDown = !goingDown;
    }
    goingDown ? Rindex-- : Rindex++; //then we advance the Rindex.
    time = millis(); //and restart the timer for this upcoming phase.
    velocity = sqrt((2.0f / rVals[Rindex]) - (1.0f/semimajorAxis));
    float speedBar = map((int)1000*velocity,(int)1000*velocity_MIN,(int)1000*velocity_MAX,1,10);
    for (int i = 0; i < 10; i ++){
      if (i<speedBar){
        digitalWrite(i, HIGH);
      } else {
        digitalWrite(i, LOW);
      }
    } 
  }

}

void requestEvent()
{
  //TODO - we reduce the servo resolution to only .1 here. Instead we need to rewrite master to handle
  // 0-255 and interp on that.
  Wire.write((byte) map(Rindex,0,250,1,100)); // respond with message of 1 byte
  // as expected by master
}
