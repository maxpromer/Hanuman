#include <Hanuman.h>

void setup()  {
  Serial.begin(9600);
}

void loop()  {
  int val = analog(10);
  if(val > 2048) {
    Serial.println("0");
  } else {
    Serial.println("1");
  }
  delay(100);
}
