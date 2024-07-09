#include <Hanuman.h>

void setup()  {
  Serial.begin(9600);
}

void loop()  {
  int val;
  val = in(9); // 9 = pin D9 
  Serial.printf("D9= %d\t", val);
  val = in(_A0); // _A0 = pin A0
  Serial.printf("A0= %d\t", val);
  val = in(_A1); // _A1 = pin A1
  Serial.printf("A1= %d\t", val);
  val = in(_A2); // _A2 = pin A2
  Serial.printf("A2= %d\t", val);
  val = in(_A3); // _A3 = pin A3
  Serial.printf("A3= %d\t", val);
  val = in(_A4); // _A4 = pin A4
  Serial.printf("A4= %d\t", val);
  val = in(_A5); // _A5 = pin A5
  Serial.printf("A5= %d\t", val);
  val = in(_A6); // _A6 = pin A6
  Serial.printf("A6= %d\t", val);
  val = in(_A7); // _A7 = pin A7
  Serial.printf("A7= %d\t", val);
  val = in(_A8); // _A8 = pin A8
  Serial.printf("A8= %d\n", val);
  delay(100);
}
