/* Arduino code to trigger pulse trains.
 * Created for controlling an OBIS laser.
 *  
 *  In Tools menu set it to Arduino Nano and Arduino 328P (Old bootloader)
 *  Open Serial Monitor (under Tools or Ctrl+Shit+M)
 *  
 * Create a serial connection with baudrate 115200.
 * That is the highest rate I found to be reliable.
 * Use NO line ending!
 * 
 * Note: Set the HP function generator to:
 * - block shape
 * - burst mode
 * - amplitude 3V seems to work
 * - though the menu you can select multiple pulses if you want, but I'd recommend doing that through Arduino
 * - the pulse duration is determined by the frequency and the duty cycle (50% by default)
 * 
 * Command structure: mode n_pulse, us_on, ms_off
 * mode:    's' (or 'single') or 'm' (or 'multiple')
 * n_pulse: number of pulses, integer (uint32)
 * us_on:   duration of pulse in microseconds, integer 0 - 65535
 * ms_off:  dead-time after pulse in milliseconds, integer (uint32)
 * 
 * Note that this command primes it.
 * Any next command will act as a software trigger 
 * and fires the pulse train.
 * 
 * For example:
 * Send a command like: single 10, 65000, 200
 * Followed by any command.
 * 
 * The first command defines a pulse train of 10 pulses
 * of 65000us on-time and 200ms off-time.
 * The second command triggers it.
 * Any following command will be interpreted but won't trigger a pulse train.
 * 
 * Alternatively, the mode 'multiple' can be used to trigger 
 * a pulse train on every every subsequent command.
 * (But also on the command specifying new parameters)
 * 
 * Version 2021-06-24A by Aron Opheij
 */


#define pin 4 // Change this to the pin used (13 is the led-pin, good for testing)

void setup() {
  Serial.begin(115200);
  pinMode(pin, OUTPUT);
//  Serial.println("test");
//  digitalWrite(pin, HIGH);
//  delay(400);
//  digitalWrite(pin, LOW);
//  while (!Serial.available());
//  {}
//  Serial.println("I heard you");
}

uint16_t t_on = 0;
uint32_t t_off = 0;
uint32_t n_pulse = 0;
char mode = 's';

void loop() {
  // Wait for any signal to arrive on serial...
  while (!Serial.available()) {
    }
  // ... and then immediately fire the pulse train:
  for (uint32_t n=0; n<n_pulse; n++) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(t_on);
    digitalWrite(pin, LOW);
    delay(t_off);
  }
  // If mode is 'single' reset everything to 0,
  // that way it fires an 'empty' train at the next trigger
  if (mode == 's') {
    t_on = 0;
    t_off = 0;
    n_pulse = 0;
  }
  // Read in the first char ...
  char ch1 = Serial.read();
  // ... and if it's 'single' or 'mulitple', interpret 3 integers
  if ((ch1 == 's') or (ch1 == 'm')) { 
    mode = ch1;
    n_pulse = Serial.parseInt();
    t_on = Serial.parseInt();
    t_off = Serial.parseInt();
    Serial.print(n_pulse);
    Serial.print(", ");
    Serial.print(t_on);
    Serial.print(", ");
    Serial.println(t_off);
    // For debugging, print them to screen, but this can be turned off:
//    Serial.println(t_on);
//    Serial.println(t_off);
//    Serial.println(n_pulse);
//    Serial.println(mode);
  }
  // Clear the remainder of the input buffer:
  while (Serial.available()) {
    Serial.read();
  }
}
