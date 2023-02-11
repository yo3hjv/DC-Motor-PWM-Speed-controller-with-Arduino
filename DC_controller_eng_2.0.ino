#include <LiquidCrystal.h>
#include <LcdBarGraph.h>

/* DC motor speed controller
   based on Arduino 
		Autor: Adrian YO3HJV
		februarie 2023
		Versiunea 2.0
		
		Bargraph library: https://github.com/prampec/LcdBarGraph
*/



/*
Speed potentiometer wiper to A2
PWM output for LCD backlight D3
PWM output to power transistor D10
*/


byte lcdNumCols = 16;   // LCD display number of columns used to draw the bargraph
byte pot_pin = A2;      // potentiometer wiper
byte volt_pin = A0;     // input voltage through resistive divider

int pot_value = 0;
int pwm_factor = 0;
int percentage = 0;
int v_adc = 0;                   

float divider_ratio = 13.1386;  // voltage ratio from the voltage divider
                                // this value is to be fine adjusted to have a precise reading of the voltage
#define lcd_led 3       // PWM output for LCD backlight ("A" pin on LCD)
#define motor_pin 10    // PWM output to MOS-FET

int lcd_led_intensity = 255;  // LCD backlight brightness

//  4 bit LCD connections
const int rs = 9, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


LcdBarGraph lbg(&lcd, lcdNumCols);             

void setup(){
  Serial.begin(9600);
  analogWrite(lcd_led, lcd_led_intensity);    // Set backlight
                                              // -- initializing the LCD
  lcd.begin(2, lcdNumCols);
  lcd.clear();
    delay(100);

      // these are fixed on LCD. We set them here because the LCD will not be cleared
  lcd.setCursor (3, 1);
  lcd.print("%");
    lcd.setCursor (15, 1);
    lcd.print("V"); 
}




void loop()
{  

  int i;
  int numReadings = 10;                    // the number of samples for the input voltage

  for (i = 0; i < numReadings; i++){
    
                v_adc = v_adc + analogRead(volt_pin);

                // 10ms to stabilise the ADC readings
                delay(10);
                  }

									// ADC sampled values averaging
  v_adc = v_adc / numReadings;
									// we determine the voltage from the averaged adc readings after the voltage divider 
                                    // For greater precision, we can replace '5V' with the value measured at RAW pin on Arduino board
  float voltage = v_adc * (5.0 / 1023.0); 
                                    // Apply voltage divider ratio to determine the input voltage BEFORE the voltage divider
  float volt = voltage * divider_ratio;
         Serial.println(volt);             // debug and calibration. Comment this line after calibration
  
if (volt > 0 & volt < 10){
                              
                              lcd.setCursor (11, 1);
                              lcd.print(" ");
                              lcd.print (volt,1);}                              

if (volt>= 10 & volt <100){
                              lcd.setCursor (11, 1);
                              lcd.print("");
                              lcd.print (volt,1);}
  
							// read ADC from Speed potentiometer and transform the 1024 steps into 255 steps
  pot_value = analogRead(pot_pin);
  pwm_factor = map(pot_value, 0, 1023, 0, 255);
  percentage = map(pot_value, 0, 1023, 0, 100);   // maximum printed value is 100%
  

							// Draw bargraph from 255 steps
      lbg.drawValue(pwm_factor, 255); 
      delay(100);
  

							// set PWM for DC motor speed
  analogWrite(motor_pin, pwm_factor);



  
							//  print percentage on LCD

if (percentage > 0 & percentage< 10){                              
                              lcd.setCursor (0, 1);
                              lcd.print("  ");
                              lcd.print (percentage);}                              

if (percentage>= 10 & percentage <100){
                              lcd.setCursor (0, 1);
                              lcd.print(" ");
                              lcd.print (percentage);}

if (percentage >=100){
                              lcd.setCursor (0, 1);
                              lcd.print("");
                              lcd.print (percentage);}
            delay(100);
}
