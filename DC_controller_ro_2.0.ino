#include <LiquidCrystal.h>
#include <LcdBarGraph.h>

/* Regulator de viteza pentru motoare DC
cu afisarea valorilor pe LCD.
		Montajul afiseaza valoarea PWM in procent si cu un bargraf precum 
		si tensiunea de alimentare a montajului
		Autor: Adrian YO3HJV
		februarie 2023
		Versiunea 2.0
Libraria pentru bargraf: https://github.com/prampec/LcdBarGraph
*/



/*
Cursorul de la potentiometru la A2
Iesirea PWM pentru retroiluminare la D3
Iesirea PWM pentru controlul turatiei la D10
*/


byte lcdNumCols = 16;   // numarul de coloane ale afisajului LCD
byte pot_pin = A2;      //  cursorul potentiometrului
byte volt_pin = A0;     //  divizorul rezistiv pentru tensiunea de intrare

int pot_value = 0;
int pwm_factor = 0;
int percentage = 0;
int v_adc = 0;                   // variabila de stocare a citirii ADC pentru tensiunea de intrare

float divider_ratio = 13.1386;  // raportul de divizare rezistiva (se stabileste in functie de valorile divizorului rezistiv)
// se va ajusta pentru etalonarea indicatiei pe LCD raportat la tensiunea masurata la intrarea dispozitivului
#define lcd_led 3       // iesire PWM pentru retroiluminare - LEDul afisajului > Anod
#define motor_pin 10    // iesire PWM pentru controlul MOS-FET-ului

int lcd_led_intensity = 255;  // variabila pentru intensitatea retroiluminarii afisajului LCD

// Afisajul LCD pe 4 bit
const int rs = 9, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


LcdBarGraph lbg(&lcd, lcdNumCols);             // crearea obiectului bargraf

void setup(){
  Serial.begin(9600);
  analogWrite(lcd_led, lcd_led_intensity);    // intensitatea retroiluminarii afisajului
  // -- initializing the LCD
  lcd.begin(2, lcdNumCols);
  lcd.clear();
    delay(100);

      // Afisez valorile statice pe LCD
  lcd.setCursor (3, 1);
  lcd.print("%");
    lcd.setCursor (15, 1);
    lcd.print("V"); 
}




void loop()
{  

  int i;
  int numReadings = 10;                    // cate masuratori executam

  for (i = 0; i < numReadings; i++){
    
                v_adc = v_adc + analogRead(volt_pin);

                // 10ms pauza pentru stabilizarea ADC-ului cf fisei de catalog a uC
                delay(10);
                  }

									// Facem media valorii adc masurate
  v_adc = v_adc / numReadings;
									// calculam tensiunea la intrare pe baza mediei masuratorii executate de ADC  
  
                                    // 5V este referinta ideala. Pentru precizie crescuta, putem masura
                                    // valoarea tensiunii la pinul RAW al placii Arduino si introduce aici ca referinta
  float voltage = v_adc * (5.0 / 1023.0);    
     
  float volt = voltage * divider_ratio;
  Serial.println(volt);   // pentru debug si calibrare
  
if (volt > 0 & volt < 10){
                              
                              lcd.setCursor (11, 1);
                              lcd.print(" ");
                              lcd.print (volt,1);}                              

if (volt>= 10 & volt <100){
                              lcd.setCursor (11, 1);
                              lcd.print("");
                              lcd.print (volt,1);}
  
							// Citim valoarea ADC generata de potentiometru si o transformam din 1024 de pasi in 255
  pot_value = analogRead(pot_pin);
  pwm_factor = map(pot_value, 0, 1023, 0, 255);
  percentage = map(pot_value, 0, 1023, 0, 100);   // valoarea maxima este 100%
  

							// desenam bargraful pe baza valorii pwm
      lbg.drawValue(pwm_factor, 255);  // sunt doar 255 pasi
      delay(100);
  

							// seteaza viteza prin PWM
  analogWrite(motor_pin, pwm_factor);



  
							//  afiseaza "procentul"

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
