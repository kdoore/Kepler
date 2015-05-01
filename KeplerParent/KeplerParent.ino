#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIN 8

#define NUMPLANETS 4

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, NEOPIN, NEO_GRB + NEO_KHZ800);

int mins[] = {180, 180, 180, 180};
int maxs[] = {580, 580, 580, 580};
int motors[] = {0,1,4, 7};
int indices[] = {2,3,4,5};

unsigned long time;
unsigned long time2;
int flashIndex;
int flashWait;
int i2cI = 0;
boolean notFlashed = true;

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pwm.begin();
  pwm.setPWMFreq(60);
  strip.begin();
  for (int i = 0; i < 25; i ++){
      strip.setPixelColor(i, strip.Color(80,80,80));
  }
  strip.show();
  time = millis();
  time2 = millis();
  flashWait = random(100,150);

}

void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

void loop()
{

  if (millis() - time > 50){
  //for (int i2cI = 0; i2cI < sizeof(indices)/sizeof(int); i2cI ++){
    Wire.requestFrom(indices[i2cI], 1);    // request 6 bytes from slave device #2

    while (Wire.available())   // slave may send less than requested
    {
      char c = Wire.read(); // receive a byte as character
      //2, 3, 4, 5
      //0, 1, 4, 7
      pwm.setPWM(motors[i2cI], 0, (map((int) c, 1, 100, mins[i2cI], maxs[i2cI])));
    }
    (i2cI < (sizeof(indices)/sizeof(int) - 1)) ? i2cI ++ : i2cI = 0;
    time = millis();
  }
  if (millis() - time2 > flashWait && notFlashed){
    flashIndex = random(25);
    strip.setPixelColor(flashIndex,strip.Color(255,255,255));
    strip.show();
    notFlashed = false;
  }
  if (millis() - time2 > flashWait+20){
    for (int i = 0; i < 25; i ++){
      strip.setPixelColor(i, strip.Color(80,80,80));
      }
    strip.setPixelColor(flashIndex,strip.Color(80,80,80));
    strip.show();
    time2 = millis();
    notFlashed = true;
  }
}
