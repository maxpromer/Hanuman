#include <Hanuman.h>

void setup() {
  oled.begin();
  oled.clear();
  
  oled.textColor(BLACK,WHITE);
  for(int i=0;i<8;i++) {
    oled.text(i, 0, "Row %d ", i);
  } 
  oled.show(); 
}

void loop() {

}
