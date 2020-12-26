//Defines so the device can do a self reset
#define SYSRESETREQ (1 << 2)
#define VECTKEY (0x05fa0000UL)
#define VECTKEY_MASK (0x0000ffffUL)
#define AIRCR (*(uint32_t *)0xe000ed0cUL) // fixed arch-defined address
#define resetboard (AIRCR = (AIRCR & VECTKEY_MASK) | VECTKEY | SYSRESETREQ)

#define buzpin 4
void beep(){
  analogWrite(buzpin, 120); 
  delay(100);
  digitalWrite(buzpin, LOW);
}