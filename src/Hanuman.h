//
//	Hanuman Library
//	Release 1.1.0
//	Date -
//
//	Update: -
//

#ifndef __HANUMAN_H__
#define __HANUMAN_H__

#include "Arduino.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <OLED_I2C_SSD1309.h>
OLED_I2C_SSD1309 oled(-1);


#define sleep(x) delay(x)
#define delay_us(x) delayMicroseconds(x)

#include <Servo.h>

#define SV1pin 13
#define SV2pin 18
#define SV3pin 19

int servo_pins[] = { SV1pin, SV2pin, SV3pin };

#define M1A 1
#define M1B 0
#define M2A 3
#define M2B 2
#define M3A 5
#define M3B 4
#define M4A 7
#define M4B 6

typedef struct {
  int a;
  int b;
} Motor_Pin_t;
Motor_Pin_t motor_pins[4] = {
  { M1A, M1B },
  { M2A, M2B },
  { M3A, M3B },
  { M4A, M4B },
};

Servo myServo[sizeof(servo_pins)];

volatile int __analogResolution = 10;

enum {
  _A0 = 100,
  _A1,
  _A2,
  _A3,
  _A4,
  _A5,
  _A6,
  _A7,
  _A8,
  _A9
};

#define ANALOG_MUX_S0_PIN (12)
#define ANALOG_MUX_S1_PIN (11)
#define ANALOG_MUX_S2_PIN (10)

void init_mux() {
  static bool init_mux_flag = false;
  if (!init_mux_flag) {
    pinMode(ANALOG_MUX_S0_PIN, OUTPUT);
    pinMode(ANALOG_MUX_S1_PIN, OUTPUT);
    pinMode(ANALOG_MUX_S2_PIN, OUTPUT);
  }
}

void set_mux(uint8_t ch) {
  init_mux();

  digitalWrite(ANALOG_MUX_S0_PIN, bitRead(ch, 0) ? HIGH : LOW);
  digitalWrite(ANALOG_MUX_S1_PIN, bitRead(ch, 1) ? HIGH : LOW);
  digitalWrite(ANALOG_MUX_S2_PIN, bitRead(ch, 2) ? HIGH : LOW);
}

//-------------------------------------------------------------
// Digital in,out
//-------------------------------------------------------------
int in(int p) {
  if (p < _A0) {
    pinMode(p, INPUT_PULLUP);
    return digitalRead(p);
  } else if (p >= _A0 && p <= _A7) {
    pinMode(A0, INPUT);
    set_mux(p - _A0);
    delayMicroseconds(50); // wait switch done
    return digitalRead(A0);
  } else {
    pinMode(p - _A8 + A1, INPUT_PULLUP);
    return digitalRead(p - _A8 + A1);
  }
}

void out(int p, int dat) {
  pinMode(p, OUTPUT);
  digitalWrite(p, dat);
}

//-------------------------------------------------------------
// End Digital in,out
//-------------------------------------------------------------

//-------------------------------------------------------------
// Analog
//-------------------------------------------------------------
int analog(int pinAN);

int analog(void)  // return 10 or 12 as resolution mode
{
  return (__analogResolution);
}

void analog10(void) {
  __analogResolution = 10;
  analogReadResolution(10);
}

void analog12(void) {
  __analogResolution = 12;
  analogReadResolution(12);
}

int analog10(int pinAN) {
  __analogResolution = 10;
  analogReadResolution(10);
  return analog(pinAN);
}

int analog12(int pinAN) {
  __analogResolution = 12;
  analogReadResolution(12);
  return analog(pinAN);
}

int analog(int pinAN) {
  //  analogReadResolution(__analogResolution);
  static bool init_analog_pin = false;
  if (!init_analog_pin) {
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
  }

  if ((pinAN >= 0) && (pinAN <= 7)) {
    set_mux(pinAN);

    return analogRead(A0);
  } else if ((pinAN >= 8) && (pinAN <= 10)) {
    return analogRead((pinAN - 8) + A1);
  }

  return 0;
}

int analog(int pinAN, int scale) {
  long value = analog(pinAN);
  if (__analogResolution == 12) {
    value = (value * (scale + 1)) / 4095;
  } else /* if (__analogResolution==10) */ {
    value = (value * (scale + 1)) / 1023;
  }

  if (value > scale) {
    value = scale;
  }

  return value;
}

int analog(int pinAN, int scaleMin, int scaleMax) {
  long value = analog(pinAN);
  if (scaleMax >= scaleMin) {
    return map(value, 0, __analogResolution == 12 ? 4095 : 1023, scaleMin, scaleMax);
  } else {
    if (__analogResolution == 12) {
      value = 4095 - value;
      value = value / (4095 / ((scaleMin + 1) - scaleMax));
    } else {
      value = 1023 - value;
      value = value / (1023 / ((scaleMin + 1) - scaleMax));
    }

    value += scaleMax;
    if (value > scaleMin) {
      value = scaleMin;
    }
  }

  return value;
}

void init_analog_A3() {
  static bool init = false;
  if (!init) {
    pinMode(A3, INPUT);
  }
}

int __knobLastValue = 0;

#define sw_ok SW_OK
#define sw_OK SW_OK
#define sw_ok_press SW_OK_press
#define sw_OK_press SW_OK_press
#define KNOB(x) knob(x)

int SW_OK(void) {
  // init_analog_A3();
  
  pinMode(A3, INPUT_PULLUP);
  int value = analogRead(A3);
  // Serial.println(value);
  if (__analogResolution == 12) {
    return analogRead(A3) < 20;
  } else {
    return analogRead(A3) <= 4;
  }

  return 0;
}

void waitSW_OK(void) {
  while (!SW_OK()) ;
  while (SW_OK()) ;
}

void SW_OK_press(void)  // backward compatible function
{
  waitSW_OK();
}

int knob(void) {
  init_analog_A3();

  int __knobValue;
  __knobValue = analogRead(A3);
/*
  if (__analogResolution == 12) {
    if (__knobValue >= 16) {
      if (__knobValue < 95)
        __knobValue = 95;
      __knobLastValue = __knobValue - 95;
    }
  } else {
    if (__knobValue >= 4) {
      if (__knobValue < 23)
        __knobValue = 23;
      __knobLastValue = __knobValue - 23;
    }
  }*/

  // return (__knobLastValue);
  int max_value = __analogResolution == 12 ? 4095 : 1023;
  __knobValue = map(__knobValue, 10, 1000, 0, max_value);
  if (__knobValue < 0) {
    __knobValue = 0;
  }
  if (__knobValue > max_value) {
    __knobValue = max_value;
  }
  return __knobValue;
}

int knob(int scale) {
  long value;
  value = knob();
  if (__analogResolution == 12) {
    value = ((value * (scale + 1)) / 4000);
  } else {
    value = ((value * (scale + 1)) / 1000);
  }
  if (value > scale) {
    value = scale;
  }
  return (value);
}

int knob(int scaleCCW, int scaleCW) {
  long value;
  //  analogRead(7);
  //  value=analogRead(7);
  value = knob();
  if (scaleCW >= scaleCCW) {
    if (__analogResolution == 12) {
      value = value / (4000 / ((scaleCW + 1) - scaleCCW));
    } else {
      value = value / (1000 / ((scaleCW + 1) - scaleCCW));
    }
    value += scaleCCW;
    if (value > scaleCW) {
      value = scaleCW;
    }
  } else {
    if (__analogResolution == 12) {
      value = 4000 - value;
      value = value / (4000 / ((scaleCCW + 1) - scaleCW));
    } else {
      value = 1000 - value;
      value = value / (1000 / ((scaleCCW + 1) - scaleCW));
    }
    value += scaleCW;
    if (value > scaleCCW) {
      value = scaleCCW;
    }
  }
  return (value);
}

//-------------------------------------------------------------
// End Andlog
//-------------------------------------------------------------

//-------------------------------------------------------------
// Start Motor
//-------------------------------------------------------------

#ifndef __MTR_ALL
#define __MTR_ALL 100
#endif

#ifndef __MTR_MR12
#define __MTR_MR12 112
#endif

#ifndef __MTR_MR21
#define __MTR_MR21 121
#endif

#define ao() motor_stop(__MTR_ALL)
#define AO() motor_stop(__MTR_ALL)

int __motorLastpow = 0;

void _motor(int ch, int pow) {
  static bool motor_pin_init = false;
  if (!motor_pin_init) {
    for (Motor_Pin_t pin : motor_pins) {
      analogWrite(pin.a, 255);
      analogWrite(pin.b, 255);
      pinMode(pin.a, OUTPUT);
      pinMode(pin.b, OUTPUT);
    }
    motor_pin_init = true;
  }

  if ((ch < 1) || (ch > 4)) {
    return; // skip invaild ch
  }

  pow = constrain(pow, -100, 100);

  int dir = pow > 0 ? 1 : 0;
  int pwm = map(abs(pow), 0, 100, 0, 255);

  if (dir) {
    analogWrite(motor_pins[ch - 1].a, 255);
    analogWrite(motor_pins[ch - 1].b, 255 - pwm);
  } else {
    analogWrite(motor_pins[ch - 1].a, 255 - pwm);
    analogWrite(motor_pins[ch - 1].b, 255);
  }
}

void motor(int pow1, int pow2, int pow3, int pow4) {
  _motor(1, pow1);
  _motor(2, pow2);
  _motor(3, pow3);
  _motor(4, pow4);
}

void motor(char ch, int pow) {
  if (ch >= 1 && ch <= 4) {
    _motor(ch, pow);
  } else if (ch == 12) {
    _motor(1, pow);
    _motor(2, pow);
  } else if (ch == 34) {
    _motor(3, pow);
    _motor(4, pow);
  } else if (ch == 100) {
    _motor(1, pow);
    _motor(2, pow);
    _motor(3, pow);
    _motor(4, pow);
  } else if (ch == 112) {
    _motor(1, pow);
    _motor(2, -pow);
  } else if (ch == 121) {
    _motor(1, -pow);
    _motor(2, pow);
  }
}

void motor(int intch, int pow) {
  char bch;
  if (intch == 1234) {
    bch = 100;
  } else {
    bch = (char)intch;
  }
  motor(bch, pow);
}

void motor_stop(char ch) {
  motor((char) ch, 0);
  __motorLastpow = 0;
}

void motor_stop(int intch) {
  char bch;
  if (intch == 1234) {
    bch = 100;
  } else {
    bch = (char)intch;
  }
  motor_stop(bch);
}


void motor(char ch, int pow, int step) {
  int i;
  if (pow > __motorLastpow) {
    for (i = __motorLastpow; i <= pow; i++) {
      motor(ch, i);
      delay(step);
    }
    __motorLastpow = pow;
  } else {
    for (i = __motorLastpow; i >= pow; i--) {
      motor(ch, i);
      delay(step);
    }
    __motorLastpow = pow;
  }
}

void fd(int8_t speed) {
  motor(12, speed);
}
void fd2(int8_t speed1, int8_t speed2) {
  motor(1, speed1);
  motor(2, speed2);
}
void bk(int8_t speed) {
  motor(12, -speed);
}
void bk2(int8_t speed1, int8_t speed2) {
  motor(1, -speed1);
  motor(2, -speed2);
}
void tl(int8_t speed) {
  motor(1, 0);
  motor(2, speed);
}
void tr(int8_t speed) {
  motor(1, speed);
  motor(2, 0);
}
void sl(int8_t speed) {
  //     motor(1,-speed);
  //     motor(2,speed);
  motor(__MTR_MR21, speed);
}
void sr(int8_t speed) {
  //     motor(1,speed);
  //     motor(2,-speed);
  motor(__MTR_MR12, speed);
}
// For 4 WD
void FD(int8_t speed) {
  motor(100, speed);
}
void FD2(int8_t speed1, int8_t speed2) {
  motor(12, speed1);
  motor(34, speed2);
}
void BK(int8_t speed) {
  motor(100, -speed);
}
void BK2(int8_t speed1, int8_t speed2) {
  motor(12, -speed1);
  motor(34, -speed2);
}
void TL(int8_t speed) {
  motor(12, 0);
  motor(34, speed);
}
void TR(int8_t speed) {
  motor(12, speed);
  motor(34, 0);
}
void SL(int8_t speed) {
  motor(12, -speed);
  motor(34, speed);
}
void SR(int8_t speed) {
  motor(12, speed);
  motor(34, -speed);
}

//-------------------------------------------------------------
// End Motor
//-------------------------------------------------------------


//-------------------------------------------------------------
// Start Servo
//-------------------------------------------------------------
void servo(char n, signed int angle) {
  if (n < 1 || n > 3) {
    return;
  }
  n -= 1; // 1 - 3 to 0 - 2
  
  if (angle == -1) {
    myServo[n].detach();
  } else {
    if (!myServo[n].attached()) {
      myServo[n].attach(servo_pins[n]);
    }
    myServo[n].write(angle);
  }
}
//-------------------------------------------------------------
// End Servo
//-------------------------------------------------------------

//-------------------------------------------------------------
// Board initialize
//-------------------------------------------------------------
void board_init() {

}
//-------------------------------------------------------------
// End Board initialize
//-------------------------------------------------------------



#endif
