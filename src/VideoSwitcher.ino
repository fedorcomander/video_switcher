/** By Alex Fedorov aka FedorCommander, 2018 **/

#include "avr/interrupt.h"

volatile unsigned long PWMStart;
volatile unsigned long PWMLast;
volatile int PWMValue;
volatile int PWMLastState = 0;

volatile int PWMFiltered;

#define PWM 0

#define VID_1 1
#define VID_2 2
#define VID_3 3
#define VID_4 4

volatile int CUR_VID = VID_1;

void setup() {
  PWMLast = millis();
  pinMode(VID_1, OUTPUT);
  pinMode(VID_2, OUTPUT);
  pinMode(VID_3, OUTPUT);
  pinMode(VID_4, OUTPUT);

  digitalWrite(VID_1, HIGH);
  digitalWrite(VID_2, LOW);
  digitalWrite(VID_3, LOW);
  digitalWrite(VID_4, LOW);
  
  pinMode(PWM, INPUT_PULLUP);
  GIMSK |= (1 << PCIE);
  PCMSK |= (1 << PWM); 
  sei();        
}

int getMappedVid(int rcValue) {
  if (rcValue<1250) return VID_1;
  else if (rcValue>=1250 && rcValue<1500) return VID_2;
  else if (rcValue>=1500 && rcValue<1750) return VID_3;
  else return VID_4;
}

ISR(PCINT0_vect)
{
  PWMLast = millis();
  if (digitalRead(PWM) == HIGH) {
    if (PWMLastState == 0) {
      PWMStart = micros();
      PWMLastState = 1;
    }
  } else {
    if(PWMLastState == 1) {
      PWMValue = micros() - PWMStart;
      PWMLastState = 0;
    }
  }
}

void loop() {
  PWMFiltered = 0.9 * PWMFiltered + 0.1 * constrain(PWMValue, 1000, 2000);
  int newVid = getMappedVid(constrain(PWMFiltered, 1000, 2000));

  if ((millis() - PWMLast) > 100) { // no pwm
    newVid = VID_1;
  }
  
  digitalWrite(VID_1, newVid == VID_1 ? HIGH : LOW);
  digitalWrite(VID_2, newVid == VID_2 ? HIGH : LOW);
  digitalWrite(VID_3, newVid == VID_3 ? HIGH : LOW);
  digitalWrite(VID_4, newVid == VID_4 ? HIGH : LOW);
}





