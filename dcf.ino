#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



//<-------------------------


#define inputPin 11

void setup() {
  
  pinMode(inputPin,INPUT_PULLUP);
  delay(100);

  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

byte set=0;
int last=0;
int sek=0;
int lastSek=0;
int current=0;
short int bits[59];                    //DCF datensatz
int travel=0,bitsSet=0;
int minute=61;
int stunde=25;
void loop() {
  
  short int io13=digitalRead(inputPin);
  delay(17);
  
  if(io13!=set){                  //Flankenwechsel    

      set=io13;
      
      if(io13==LOW){              //fallende Flanke

        travel=millis()-last;        
        last=millis();
        
        if(travel>1500){          //minute voll
          lastSek=sek;          
          if(sek==58){
            // Guter Datensatz  
            stunde=0;
            minute=0;
            stunde=extraktion(bits,0);         
            minute=extraktion(bits,1);  
            Serial.println("Minute");
            Serial.println(stunde);
            Serial.println(minute);
          }else{            
            // kein guter Datensatz            
          }
          sek=0;
        }else{                    // normale Sekunde
          if((travel>900) && (travel<1100)){                        
            sek++;    
            Serial.print("|");        
          }else{                        
            
          }
          if(travel<900){
              Serial.print("FEHLER");
            }
            
        }
      }                    // <-------------------------fallende Flanke

      if(io13==HIGH){                                               //steigende Flanke        
        current=millis()-last;             
        if((sek-1)<59){                                             //Grundbedingung für brauchbaren Wert          
          if((current<150) && (current>50)){            
            bits[sek-1]=0;                  
          }
          
          if((current<250) && (current>=150)){            
            bits[sek-1]=1;                  
          }          
        }//<-------------------------------------------------------Grundbedingung für brauchbaren Wert
      }//<---------------------------------------------------------steigende Flanke
      
      //anzeige(travel,sek,lastSek,stunde,bits[sek-1],minute);
      //<-- hier wertausgabe auf pins im DCF77 sekunden takt
/*
      display.clearDisplay();
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println(stunde);
      display.setCursor(32,0);             // Start at top-left corner
      display.println(minute);
      display.setCursor(64,0);             // Start at top-left corner
      display.println(sek);
      display.display();
      */
    }//<-----------------------------------------------------------Flankenwechsel
    

      

  
}




// type 0:Stunde; 1:Minute; 2:Tag; 3:Monat; 4:Jahr
int extraktion(short int *input,short int type){

  byte wertArray[8]={1,2,4,8,10,20,40,80};
  short int puffer=0;
  
    if(type==0){

          for (int i=28;i<34;i++){                //Stunde
                if(bits[i]==1){
                puffer+=wertArray[i-28];
                bitsSet++;
               }
              }
              Serial.println(bits[34]);
           if(bits[27]==(bitsSet % 2)){           
                puffer=26;
           }
            
    }

    if(type==1){
      
          for (int i=20;i<27;i++){              //Minute
                  if(bits[i]==1){
                  puffer+=wertArray[i-20];
                  bitsSet++;
                 }
                }
                Serial.println(bits[27]);
             if(bits[34]==(bitsSet % 2)){           
                  puffer=62;
             }
      
    }
    return puffer;
}
