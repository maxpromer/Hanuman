#include <Hanuman.h>

void setup() {

}

void loop()  {
  if(SW_OK()) {
    motor(1, 100);
    delay(2000);
    motor_stop(1);
  }
}
