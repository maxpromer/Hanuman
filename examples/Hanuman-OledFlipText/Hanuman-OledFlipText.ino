#include <Hanuman.h>

void setup() {
  oled.begin();
  oled.clear();
}

void loop()  {
  for(int x=1;x<3;x++) {
    oled.textSize(x);
    for(int m=1;m<4;m++) {
      oled.clear();
      oled.mode(m);
      oled.text(0,0,"%dX",x);
      oled.text(1,0,"M=%d",m);
      oled.show();
      delay(5000);
    }
  } 
}
