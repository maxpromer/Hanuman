#include <Hanuman.h>

void setup()  {
  
}

void loop()  {
  motor(1, 30);
  motor(2, 30);
  delay(3000);
  motor_stop(1);
  motor_stop(2);
  delay(3000);
}
