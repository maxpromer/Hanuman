#include <Hanuman.h>

void setup()  {
  Serial.begin(9600);
}

void loop()  {
  int val = analog(0);
  float volts = (val * 3.3) / 4096.0;
  Serial.printf("Adc: %d\n", val);
  Serial.printf("Volts: %f\n", volts);
  delay(100);
}
