#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <VirtualWire.h>
#include <dht.h>
#include <LiquidCrystal.h>


Adafruit_BMP085 bmp;
dht DHT;

#define DHT11_PIN   A2                        //A2 will read the humidity

#define co2_ip     A1                        //analog feed from MQ135
                 //digital feed from MQ135
#define co2_0      55                        //calibrated CO2 0 level. This needs to be changed according to the value of the place you are using the CO2 sensor
   
int tx_no=0;                                 // Transmission # for testing



// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

  
void setup() {
    Serial.begin(9600);
    Serial.println("setup");
    digitalWrite(10,LOW);
    
    pinMode(DHT11_PIN, INPUT);

    pinMode(7, OUTPUT);
    pinMode(co2_ip,INPUT);                     //MQ135 analog feed set for input
    
   

    bmp.begin();

    
    // Initialise the IO and ISR
    vw_set_tx_pin(7);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);  // Bits per sec
    
    lcd.begin(16, 2);

    
    
  

}
  
void loop() {

  /*Initial setup for gas sensor.. this is long...*/ 
     
     int co2_now[10];                               //int array for co2 readings
     int co2_raw = 0;                               //int for raw value of co2
     int co2_comp = 0;                              //int for compensated co2 
     int co2_ppm = 0;                               //int for calculated ppm
     int co2_avg = 0;                               //int for averaging

    for (int x = 0;x<10;x++)                        //samplpe co2 10x over 0.5 seconds
        {                   
         co2_now[x]=analogRead(A1);
         delay(50);
        }

     for (int x = 0;x<10;x++)
        {                     //add samples together
          co2_avg=co2_avg + co2_now[x];
    
        }

     co2_raw = co2_avg/10;                           //divide samples by 10
     co2_comp = co2_raw - co2_0;                     //get compensated value
     co2_ppm = map(co2_comp,0,1023,400,5000);        // 0 is mapped to the average CO2 levels of Earth i.e., 400 ppm AND 1023 is mapped to 5000 ppm. The 5000 ppm is chosen arbitrarily just for lulz.

    /* reading CO2 Level ends now, and reading values of other sensors begins*/

    float temperature = bmp.readTemperature();       // reading temperature
    
    long pressure = bmp.readPressure();              // reading pressure
    
    DHT.read11(DHT11_PIN);                           // reading humidity

    /* Code for all transmitted values begins now*/

    /*Transmitting the numver of values read*/

    String value0 = "-------- This is Reading# " + String(tx_no) + " --------";

    char *msg = value0.c_str();
  
    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send(msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);
    
    /*Transmitting Temperature*/

    String value1 = " : Temperature = " + String(temperature) + " *C";  //displaying temperature as Temperature = [Temperature Value] *C

    msg = value1.c_str();
  
    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send(msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);

    /*Transmitting Pressure*/

    String value2 = "Pressure = " + String(pressure) + " Pa"; //displaying Pressure as Pressure = [Pressure Value] Pa
    
    msg = value2.c_str();
    
    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send(msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);


    /*Transmitting relative Humidity*/

    String value3 = "Humidity = " + String(DHT.humidity) + " %rH"; //displaying Humidity as Humidity = [Humidity Value] %rH

    msg = value3.c_str();
  
    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send(msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);


    /*Transmitting CO2 Level*/

    String value4 = "CO2 Level = " + String(co2_ppm) + " ppm"; //displaying CO2 Level as CO2 Level = [CO2 Level Value] ppm

    msg = value4.c_str();
  
    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send(msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);

    /*Updating Transmission values*/

    tx_no++;                                // Update transmission number after every loop
    

    /*Codes for Transmission ends and codes for LCD display begins*/
    
    
    /* Code for Temperature display begins from here. Do not copy this part for other sensors*/

    lcd.setCursor(0, 0); //set cursor to initial position
   
    lcd.print("Temp = ");
    lcd.print(temperature);
    lcd.print(" *C");
    
    
    /* Code for Pressure display begins from here. You copy this part for other sensors but be careful about the LCD cursor position, pins and inputs*/

    lcd.setCursor(0, 1);

    lcd.print("Pre = ");
    lcd.print(pressure);
    lcd.print(" Pa");

     
    /*Observer can read the values within the delay below, and after that LCD clears to show more sensor values*/
    
    delay (1500);
    lcd.clear();

    /* Code for Humidity sensor begins from here. You copy this part for other sensors but be careful about the LCD cursor position, pins and inputs*/
    
    lcd.setCursor(0, 0);

    lcd.print("Hum = ");
    lcd.print(DHT.humidity);
    lcd.print(" %rH");


    /* Code for CO2 levels begins from here. You copy this part for other sensors but be careful about the LCD cursor position, pins and inputs*/
    
    lcd.setCursor(0,1);

    lcd.print("CO2 = ");
    lcd.print(co2_ppm);
    lcd.print(" ppm");


    
    /*Observer can read the values within the delay below, and after that LCD clears*/

    delay(1500);
    
  
}
