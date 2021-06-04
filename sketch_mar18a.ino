
/*This is the complete code for the Arduino digital watch with HM10 BLE module.
 * You can costomize it as you like; the code is commented for your comprehension. 
 * I recommend you go to file>preferences> amd check "Display line number" "Enable 
 * code folding" because the code is long.
 * Refer the datasheets of Atmega328p, DS3231, 24C32 4kb EEPROM, and BLE module here-
 * Atmega321p: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
 * DS3231: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
 * EEPROM: https://ww1.microchip.com/downloads/en/DeviceDoc/doc0336.pdf
 * HM10 BLE: https://components101.com/asset/sites/default/files/component_datasheet/HM10%20Bluetooth%20Module%20Datasheet.pdf
 * If you are using the EEPROM seperately, the the address would be 0x50 with A0, A1, & A2 connnected low
 * On the DS3231 module, the A0, A1, and A2 pins of EEPROM are puled high so the address is 0x57
 * You myst change the SparkFun_External_EEPROM.h file manually-- toward the end of the code, 
 * change 'deviceAddress' from 0b1010000 to 0b1010111 and memorySize_bytes from 512000/8
 * to 32000/8.
 
 * For the prototype on breadboard--
 * On Arduino UNO board, SDA and SCL pins are connected to SDA and SCL of the DS3231 module 
 * with EEPROM and the oled. Pins 2, 4, and 5 are used as interrupts for puchbutton 
 * and pin 3 is used as interupt for alarm pin 7 is connected to buzzer. All the 
 * modules are power with 3.3v on board regulator of Arduino uno. 
 * 
 
 * While uploading the code make sure to disconnect the BLE module from Tx and Rx 
 * pin. If you forget to do that, the IDE throws a buch of errors, and HM10 stops 
 * working, make a simple code to communicate with the module using AT commands 
 * through serial moniter.First, type AT+DEFAULT to resore factory settings. Type 
 * AT+HELP for getting all commands. Type AT+PWRM0 to set the HM10 in auto sleep 
 * mode. Type AT+NAME[name] to set the name for pairing.  
 * 
 * Affter uploading make sure that the Tx pin of Arduino is connected to Rx  pin 
 * of BLE through a voltage devider that gives 3.3v to BLE's RX pin. Then connect 
 * Rx of arduino to Tx of BLE. Meanwhile,the OLED screen will blink the alarm bell
 * click the middle button to switch it off and wait for 5 seconds before switching 
 * the watch on. If you switch the watch on before 5 seconds, The features may not 
 * work correcly; just wait for 5 seconds more without pressing any button. If you 
 * select any option on the menupage, go back and wait for screen to shut off. 
 * 
 * The watch is ready to use. 
 * 
 * 
 * To send messages with the watch you can use any simple Bluetoth terminal app with cit meant 
 * for BLE. Apps meant for classic bluetooth devices wont work and the Phone wont pair with the watch.
*/

const unsigned char  Alarm [] PROGMEM = //Alarm bitmap
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
  0xfc, 0x00, 0x00, 0x00, 0x01, 0x00, 0x3f, 0xff, 0x00, 0x20, 0x00, 0x07, 0x00, 0x7f, 0xff, 0x80, 
  0x38, 0x00, 0x07, 0x00, 0x7f, 0xff, 0x80, 0x38, 0x00, 0x1e, 0x61, 0xff, 0xff, 0xe0, 0xde, 0x00, 
  0x1c, 0x61, 0xff, 0xff, 0xe0, 0xce, 0x00, 0x18, 0xe1, 0xff, 0xff, 0xe0, 0xe6, 0x00, 0x19, 0xc1, 
  0xff, 0xff, 0xe0, 0xe6, 0x00, 0x19, 0xc1, 0xff, 0xff, 0xe0, 0xe7, 0x00, 0x39, 0x01, 0xff, 0xff, 
  0xe0, 0x27, 0x00, 0x39, 0x01, 0xff, 0xff, 0xf0, 0x27, 0x00, 0x39, 0x03, 0xff, 0xff, 0xf8, 0x27, 
  0x00, 0x39, 0xc7, 0xff, 0xff, 0xf8, 0x27, 0x00, 0x39, 0xc7, 0xff, 0xff, 0xf8, 0xe7, 0x00, 0x18, 
  0x07, 0xff, 0xff, 0xf8, 0x06, 0x00, 0x18, 0x07, 0xff, 0xff, 0xf8, 0x06, 0x00, 0x18, 0x07, 0xff, 
  0xff, 0xf8, 0x06, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xfc, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xfc, 0x00, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0xff, 
  0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0xff, 0xff, 0xff, 
  0xff, 0xe0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Notification [] PROGMEM =  //notification butmap
{
  // 'Notification, 13x10px
  0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x1f, 0xc0, 0x1f, 0xc0, 0x1f, 0xc0, 0x0f, 0x80, 0x00, 0x80, 
  0x00, 0x00, 0x00, 0x00
};
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include<DS3231.h>
#include<Fonts/Org_01.h>
#include<gfxfont.h>
#include<Fonts/DSEG7_Classic_Regular_11.h>
#include "SparkFun_External_EEPROM.h"
#include <avr/power.h>
#include <avr/sleep.h>


DS3231 clock;
RTClib rtc;
ExternalEEPROM Message;


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




int i=1;                            //variable for all for loops in the program.
int T, gap, Hlim, Mlim, Slim, lim=0; // refer stopwatch or timer function.
int l, al, max_characters=500;       //refer notification funtion.
int Scroll_height=1;                 //A common varaible to store value while scrolling; controlled by top and bottom button.
int Scroll_depth=-1;                 //A value to move forward in settings, or select options contolled by middle button
int Scroll_Hrange=5;                 //range for Scroll_heigth.
int settingdepth=0;                  // Similar to Scrolldepth but used in setting mode
int address[3];                      //Array to store which options were selected, and accordingly display a page.
int settinglist[6];                  // A powerful and versitile array to act as buffer for storing values in a function that 
                                     //is in setting mode. Its uses vary according to the function.

unsigned long t=500;                //Stores the millis value to keep screen on for specific period and for debouncing.

const char *alarmtype[3]={"Hour", "Daily", "Date"};  //list of options for setting type of alarm
const char *DoW[7]={"MON","TUE","WED","THU","FRI","SAT","SUN"};   
const char *month[12]={"JAN","FEB","MAR","JUN","JUL","AUG","SEPT","OCT","NOV","DEC"};
const char *menupage[7]={"Message","Set Alarm","Set time","Stopwatch","Timer", "Sound", "Back"};  //options in menupage, 
//if new option is added adjust the for loop and Scroll range for Menuscreen function accordingly


bool h12,pm, Century,AP_flag; //First 3 variables are just to use as input in functions for DS3231, 
                              //AP_flag allows to change AM to PM while displaying ince it can't be in the clock chip. 
                              //AP flag XOR with PM flag from chip is displayed on screen
                              
bool Sound=false;            //enable or disable buzzer
bool scroll_num[8];          //Array in which the corresponding element of settindepth/Scrollheight is set to 1. 
                             //E.g if Scroll_height is 4, scroll_num[3] is 1 and rest are 0.
                             // it is used in deciding accentuating the selected parameter, option.
bool insettingmode=false;    // Setting mode decides the behavior of the pushbuttons.
bool scilent=false;           //rings alarm notification or makes it scilent.
bool Screen_On=false;         // value to decide if screen should shut in 5 seconds or stay on permanantly. Screen is on in all options from Menupage.

byte  alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits, alarmperhour,c;    // 'c' is used as for reading bytes from EEPROM, 
                                                                                   


void setup()
{
// Refer Atmega328p datasheet page 53 onwards for interrupts-- https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

cli(); // Clear all interrupts                                
EIMSK |=B00000011;  // enable interrupt INT0 and INT1 on pin 2 and 3 of arduino respectively. 
                    //Pin 2 is reserved for middle button and pin 3 for alarm interrupt by the INT/SQW pin of DS3231.
                    
EICRA |= B00001010; // Set interrupts INT0 and INT1 to be triggered on falling edge
PCICR |=B00000100;  // Enable pin change interrupt on pins PCINT 23-16 of atmega328 chip
PCMSK2 |=B01110000; // Set interrupts to be triggered only on pins PCINT21(pin 5 of arduino) 
                    //and PCINT20 (pin 4 of arduino) for top and bottom buttons
                    
sei();     // set all interrupts

Wire.begin();
Serial.begin(9600);
display.begin(SSD1306_SWITCHCAPVCC,0x3C);

 // Setting input  pullup resistor for pin 2, 3, 4, 5 and setting pin 7 as output for buzzer
pinMode(2, INPUT_PULLUP); 
pinMode(3, INPUT_PULLUP);
pinMode(4, INPUT_PULLUP);
pinMode(5, INPUT_PULLUP);
pinMode(7, OUTPUT);
//retreive the address of the last character the was sent via bluetooth. This is stored in the EEPROM on address-- "max_characters+50+al". 
l=Message.read(max_characters+50+al); 
}


// Interrupt service routine(ISR) for top and bottom buttons. 
//With the millis finction only rising edge of the pin change interrupt will trigger the interrupt. 
ISR(PCINT2_vect)  
{
  //enable all modules after waking up from sleep mode. It is commin in all interrup service routines of this code.
  power_all_enable();

  // Condition for debouncing
  if (millis()-t>300)
  { 
    // if button pressed for D4
    if (!digitalRead(4))
    { 
      // if in the timer page or stopwatch page, set function of this button to go back. 
     if (address[1]==3 || (address[1]==4 && settingdepth==4) )
     {
        address[1]=0;          //clear the address on the second element of "address" array before exiting the page.
        Scroll_depth-=2;       // set Scroll_depth to 0 which corresponds to time screen.
        Scroll_height=0;       
        settinglist[0]=settinglist[1]= settinglist[2]=0;       //clear the settinglist  
        insettingmode=false;   //exit the setting mode                            
        Screen_On=false;       // enble screen to shutdown and go to sleep after 5 seconds of no acitivity
        
       }

     // increment Scroll_height by one when button is pressed until it is one lass than the range.
     //Then restart from 0. If range is 2, Scroll height can take 2 values starting from 0-- {0,1}
     else if (Scroll_height == Scroll_Hrange-1)
     {
     Scroll_height=0;
     }
     
     else{Scroll_height++;}
     }


   if (!digitalRead(5))
     {
       // decrement Scroll_height by 1 until it is 0. Then start from one less than range.
       if (Scroll_height == 0)
       {
       Scroll_height=Scroll_Hrange-1;
      
       }
       else{ Scroll_height-=1;}  
    

       }
   // if not setting mode is enabled then do this additional procedure-- clear the scroll_
   //num array and set the Scroll_height'th element if this array to 1.
   if (!insettingmode)
         {
          for (int i=0; i<Scroll_Hrange; i++)
         {
          scroll_num[i]=0;
         }
         scroll_num[Scroll_height]=1;
     }

  
  t=millis(); // set t equal to millis after service routine so if it interrupt is triggered
              //again in next 300ms the routine will not do any activity.Used for debouncing
              //pushbuttons, common to all bushbutton interrupts of this project.
}
}

// ISR for alarm interrupt
ISR(INT1_vect)      
{

  power_all_enable();
  // Switch on the alarm notification and wake up all modules of chip.
  scilent=false;  

}
  

// ISR for middle button
ISR(INT0_vect)   
{ 
 
  power_all_enable();

  if(millis()-t>300)
  {
    
    if (!scilent)
    {scilent=true;} // Shut the alarm notification
   

    else if(!insettingmode)
     {
        // if not in setting mode, Clear the scroll_num array, set scroll height to 0,
        //store the scroll depth value in the address array anf increment scroll depth by 1.
       for (int i=0; i<8; i++)
       {
         scroll_num[i]=0;
       }
       scroll_num[0]=1;
       address[Scroll_depth]=Scroll_height;
       Scroll_height=0; 
       Scroll_depth++;    
      
      }
       
     // if in setting mode, Clear Scroll num array and store the settingdepth'th value of this array to 1.
     //increment settingdepth by 1.
     else
     {
       
       for (int i=0; i<8; i++)
       {
        scroll_num[i]=0;
       }
       scroll_num[settingdepth]=1;
       settingdepth++;
       Scroll_height=0; 
     }
    t=millis();  
   
  }
}


// Page to show time
void Timescreen()
{
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setFont(&DSEG7_Classic_Regular_11);
  
  display.drawFastHLine(0,14,128,WHITE);
  display.setCursor(7,41);
  display.setTextSize(2);
  display.print(clock.getHour(h12,pm));
  display.print(":");
  display.print(clock.getMinute()/10);
  display.print(clock.getMinute()%10);
  display.setTextSize(1);
  display.print(clock.getSecond()/10);
  display.print(clock.getSecond()%10);
  display.setCursor(display.getCursorX()-10,29);
  if(pm xor AP_flag){display.print("P");}
  else {display.print("A");}
 
  display.setFont(&Org_01);
  display.setCursor(3,10);
  display.print(clock.getTemperature());
  display.print("`C");
  if (Serial.available()>0){display.drawBitmap(50, 4,Notification, 13,10,WHITE);}
  if (clock.checkAlarmEnabled(1) || clock.checkAlarmEnabled(2)){ display.print("  A");}
  
  //if using the PPCB with lipo battery, uncomment this function to display battery percentage. 
  //The battery voltage varying from 3.3v to 3.7 v is mapped between 0 to 100
  /*display.setCursor(100, 10);
   display.print(map(analogRead(A0), 681, 773, 0,100));
  display.print("%");*/
  display.setTextSize(2);
  display.setCursor(4,60);
  display.print(DoW[clock.getDoW()-1]);
  display.setCursor(80,60);
  display.print(clock.getDate());
  display.print( "-" ); 
  display.print(clock.getMonth(Century));
 
}

// page to show options to use features
void Menuscreen()
{
  
  display.setCursor(3,17);
  display.setTextSize(2);
  Scroll_Hrange= 7;
   // display the options by iterating through the array.
   // Set the pointer on the option you are currently on. 
   //This is checked using curresponding element of Scroll num array; if it is 1, set the pointer to that option,
   //else just print the option.
   
  //display first four option of the menupage array if scrolling through first four options
  if (Scroll_height>=4){lim=4;} else {lim=0;}
  
  for ( i=lim; i < 7; i++)
  { 
  if (scroll_num[i]==1){display.print(menupage[i]);  display.println( "<-");}
  else{display.println(menupage[i]);}
  }
}


void Backbutton()
{
  //exit setting mode, clear the current element in adress array, got the previous page, and clear the scroll num array.
  insettingmode=false;
  address[Scroll_depth-1]=0;
  Scroll_depth-=2;
  Scroll_height=0;
  for (int i=0; i<Scroll_Hrange; i++)
   {
     scroll_num[i]=0;
   }
   scroll_num[0]=1;
}


 void settime()
{
 display.clearDisplay();
 display.setFont(); // **Clear ffont because setting background color to white doesnt work with costom fonts.


switch (settingdepth)
 {
  //First, scroll down to default.
  case 1:     //adjust DoW and store it as 1st element of settinglist.
    Scroll_Hrange=7;
    if (clock.getDoW()-Scroll_height>0){ settinglist[0]= clock.getDoW()-Scroll_height; }
    else {settinglist[0]=clock.getDoW()-Scroll_height + Scroll_Hrange;} 
    break;  
  
  case 2:  //Adjust Date and store it as 2nd element of settinglist
    Scroll_Hrange=31;
    if (clock.getDate()-Scroll_height>0){  settinglist[1]= clock.getDate()-Scroll_height; }
    else { settinglist[1]=clock.getDate()-Scroll_height + Scroll_Hrange;}
    break;  

  case 3:          //Adjust Month and store it as 3rd element of settinglist
    Scroll_Hrange=12;
    if (clock.getMonth(Century)-Scroll_height>0){ settinglist[2]= clock.getMonth(Century)-Scroll_height; }
    else {settinglist[2]=clock.getMonth(Century)-Scroll_height + Scroll_Hrange;} 
    break;  
     
  case 4:     //Adjust year and store it as 4th element of settinglist
   Scroll_Hrange=100;
   if (clock.getYear()-Scroll_height>=0){ settinglist[3]= clock.getYear()-Scroll_height; }
   else {settinglist[3]=clock.getYear()-Scroll_height + Scroll_Hrange;} 
   break;  
  case 5:          // Adjust hour and set it as 5th element of settinglist
   
    Scroll_Hrange=12;
    if (clock.getHour(h12,pm)-Scroll_height>0){ settinglist[4]= clock.getHour(h12,pm)-Scroll_height; }
    else {settinglist[4]=clock.getHour(h12,pm)-Scroll_height + Scroll_Hrange;}
    break;
  
  case 6: //Adjust minute and set it as 6th element of settinglist
    
    Scroll_Hrange=60;
    if (clock.getMinute()-Scroll_height>=0){settinglist[5]= clock.getMinute()-Scroll_height; }
    else {settinglist[5]=clock.getMinute()-Scroll_height + Scroll_Hrange;}
    break;
  
  case  7: //Adjust AP flag and set it as 7th element of settinglist
    
    Scroll_Hrange=2;
    if (Scroll_height==1){AP_flag=true;} else {AP_flag=false;}
    break;
  
  case 8:       // set all the parameters and exit
    clock.setDoW(settinglist[0]);
    clock.setDate(settinglist[1]);
    clock.setMonth(settinglist[2]);
    clock.setYear(settinglist[3]);
    clock.setHour(settinglist[4]);
    clock.setMinute(settinglist[5]);
    insettingmode=false;
    address[Scroll_depth-1]=0;
    Scroll_Hrange=4;
    settingdepth=0;
    Scroll_depth=0;
    break;   
   
   default: //set the elements of settinglist as current time
    settinglist[0]=clock.getDoW();  //takes values 1-7
    settinglist[1]=clock.getDate();
    settinglist[2]=clock.getMonth(Century);
    settinglist[3]=clock.getYear();
    settinglist[4]=clock.getHour(h12,pm);
    settinglist[5]=clock.getMinute();
    settingdepth++;
 }
display.setCursor(0,10);
display.setTextSize(2);

//display elements of the setting list on oled display; 
//if element of scroll num corresponding to the parameter being displayed is 1, 
//the background color is set to white
for (int i=0; i <6; i++)
  { 
    display.setTextColor(!scroll_num[i], scroll_num[i]);
    
    if (i==0){display.println(DoW[settinglist[i]-1]); continue;}
    else if (i==3){display.println(settinglist[i]);}
    else {display.print(settinglist[i]);}
    display.print(" ");
  } 
  
display.setTextColor(!scroll_num[6], scroll_num[6]);
if (pm xor AP_flag) {display.print("P");}
else{display.print("A");}
}


void setalarm()
{
// similar to set time finction
byte alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits, alarmperhour;
bool alarmDy, alarmH12Flag, alarmPmFlag;
display.setFont();
switch (settingdepth)
 {
  case 1:
  //adjust the type of alarm(1 or 2), retreive the parameters and store it in settinglist, and set the alarm it for alarm to ring when minutes match(alarmperhour).
   Scroll_Hrange=2; 
   if ( Scroll_height==0)
   {settinglist[0]=1;
    clock.getA1Time(alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits, alarmDy, alarmH12Flag, alarmPmFlag); 
    alarmperhour= B00001100;} 
    
    else 
    {settinglist[0]=2; 
    clock.getA2Time(alarmDay, alarmHour, alarmMinute,  alarmBits, alarmDy, alarmH12Flag, alarmPmFlag ); 
    alarmperhour= B01100000;}

    //if all alarm bits are 0, them alarm is set to ring when hour, Minute, and seconds match current time. (refer datasheet)
    //So check if it rings according to DoW or date
    if (alarmBits == B00000000) 
    { if (alarmDy) {settinglist[1]=1;} //1 corresponds to alarm  DoW and time match.
    else{settinglist[1]=2;}} // 2 corresponds to alarm when date and time match.
 
    //else it rings when minutes and seconds match. 0 corresponds to hourly alarm.
    else {settinglist[1]=0;}
   
    settinglist[2]=alarmDay;
    settinglist[3]=alarmHour;
    settinglist[4]=alarmMinute;
    break;
    
  case 2: // chooses type of alarm(hourly--0, daily--1, according to date--2) 
    Scroll_Hrange=3;
    settinglist[1]=Scroll_height;
    break;
  
  case 3:     // sets the alarm bits and alarmDy parameter according to type of alarm choosen.
   
     
    if (settinglist[1] ==0)
    // For hourly alarm sets alarm bits to- 00000000. 
    //Then clear the scroll num array and jump to adjusting minute when alarm should ring 
    { 
     alarmBits= alarmperhour; settinglist[2]=1; settingdepth= 5;    
    for (int i=0; i<8; i++)
     {
      scroll_num[i]=0;
     }
     scroll_num[4]=1;}
    
    // For alarm acc to DoW, sets alarmDy to true, adjusts the DoW, and stores it in setting list.
    else if (settinglist[1] ==1)
    {   Scroll_Hrange=7;
        alarmDy=true; 
        alarmBits= B00000000;
        if (alarmDay-Scroll_height>=0) {settinglist[2]=alarmDay-Scroll_height;  }
        else {settinglist[2]=alarmDay-Scroll_height+Scroll_Hrange; }   }
    
    // For alarm acc to Date, sets alarmDy to false, adjusst the date, and stores it in setting list.
    else 
    { Scroll_Hrange=31; 
      alarmDy=false; 
      alarmBits= B00000000;
      if ( alarmDay-Scroll_height>0){ settinglist[2]= alarmDay-Scroll_height; }
      else {settinglist[2]=alarmDay-Scroll_height + Scroll_Hrange; }   }

    break;  
  
  case 4:  // adjusts hour and stores it in settinglist.
    Scroll_Hrange=12;
    if (alarmHour-Scroll_height>0){ settinglist[3]= alarmHour-Scroll_height; }
    else {settinglist[3]=alarmHour-Scroll_height + Scroll_Hrange;}
    break;  

  case 5:          //adjusts Minute and stores it in settinlist.
    Scroll_Hrange=60;
    if (alarmMinute-Scroll_height>=0){settinglist[4]= alarmMinute-Scroll_height; }
    else {settinglist[4]=alarmMinute-Scroll_height + Scroll_Hrange;}
    break;  
     
  case 6:    //adjusts AP_flag  and stores it in settinglist.
    Scroll_Hrange=2;
    if (Scroll_height==1){alarmPmFlag=true ^ AP_flag;} else {alarmPmFlag=false^AP_flag;}
    break;
  
  case 7:    // enables or disables the alarm.
    Scroll_Hrange==2;
    display.setCursor(85,35);
    display.setTextColor(!scroll_num[6], scroll_num[6]);
    
    if (Scroll_height==1)
    {clock.turnOffAlarm(settinglist[0]); display.print("Clr");} 
    else {clock.turnOnAlarm(settinglist[0]); display.print("Set"); }
    
    settinglist[5]=Scroll_height;
    break;
  
  case 8:       // Set the values from the settinglist as alarm and exit

    insettingmode=false;
    address[Scroll_depth-1]=0;
    Scroll_Hrange=4;
    settingdepth=0;
    Scroll_depth=0;
    
    if (settinglist[5]==1)
    {
    if (settinglist[0]==1) 
    { clock.setA1Time(settinglist[2], settinglist[3], settinglist[4],00, alarmBits, alarmDy, true, alarmPmFlag); }
    else 
    {clock.setA2Time(settinglist[2], settinglist[3], settinglist[4], alarmBits, alarmDy, true, alarmPmFlag); }
    }
    
    break;   
   
   //start off the page
   default:
     settinglist[0]=1;
    settingdepth++;
 }
 display.setCursor(5,5);
display.setTextSize(2);
// displays the values in setting list on the screen. Same as previous functions
for (int i=0; i <5; i++)
  { 
    display.setTextColor(!scroll_num[i], scroll_num[i]);
    if (i==1){display.println(alarmtype[settinglist[i]]);  continue;}
    else if (i==2 && settinglist[1]!=0 )
    {
      if (alarmDy) {display.println(DoW[settinglist[i]]);} 
      else { display.println(settinglist[i]);} continue;
    }
    
    else{
    display.print(settinglist[i]);
    display.print(" ");}
  } 
display.setTextColor(!scroll_num[5], scroll_num[5]);
if (alarmPmFlag) {display.print("P");}
else{display.print("A");} 
}
 

void stopwatch()
{
// In this function top button is used to start or stop the watch, middle button to reset, and bottom button to go back.
// The time since starting of stop watch is stored in settinglist and then displaed on screen.
//first element stores hour , second element stores minutes and thid element strores seconds.

DateTime now = rtc.now();
Scroll_Hrange=2;
//if middle button is pressed i.e. scrolldepth is 1, set hour minute and second to 0, and set scrolldepth to 0.

if(settingdepth==1)
{settinglist[0]=settinglist[1]=settinglist[2]=0; 
Scroll_height=0; 
settingdepth=0; }

// When top button is pressed, Scroll height becomes 1 and timer starts.

if (Scroll_height==1)
 { // if time is starting from 0, get unixtime.
   if (settinglist[0]==0 && settinglist[1]==0 && settinglist[2]==0){ T=now.unixtime();}
   gap=now.unixtime() - T+1;  // gap is the number of seconds since button was pressed. 1 is added so that the previous if condition failes and T is not set to unixtime
   settinglist[2]= gap%60;    //calculating second as quotient when gap is devided by 60
   settinglist[1]= (gap/60)%60; //calculating minutes
   settinglist[0]= gap/3600;    // calculating number of hours
   
 }

// stoping the timer, and updating T to fit the formula for gap 
else { T=now.unixtime()-gap+1; }

// showing the time stored in settinglist.
display.setTextSize(2);
display.setCursor(30,40);
display.print(settinglist[0]);
display.print(":");
display.print(settinglist[1]);
display.print(":");
display.print(settinglist[2]);
display.setTextSize(1);
display.setCursor(100, 10);
display.print("st/sp");
display.setCursor(100, 33);
display.print("rset");
display.setCursor(100, 55);
display.print("Back");
}

 
void timer()
{ 
 // In this function first the time is set and then countdown starts. Hlim for hour, Mlim for minutes and Slim for seconds.
 DateTime now = rtc.now();
 display.setFont();

 // setting lime and 
 switch (settingdepth)
 {
  case 1:                   //setting hour 
  Hlim=Scroll_height;
  settinglist[0]=Scroll_height;
  break;
  
  case 2:                   //Setting minutes
  Mlim=Scroll_height;
  settinglist[1]=Scroll_height;
  break;
  
  case 3:                  // Setting seconds
  Slim=Scroll_height;
  settinglist[2]=Scroll_height+1;
  break;
  
  case 4:
  Scroll_Hrange=2;
  if (Scroll_height==1)         //Start the timer
   {
    // if seconds is -1 ring the the buzzer and show bell. i.e contdown gets over after zeroth second.
    if (settinglist[0]==0 && settinglist[1]==0 && settinglist[2]==-1)
    {
      display.clearDisplay(); display.drawBitmap(35, 15,Alarm, 50,50,WHITE); display.display();
      digitalWrite(7, Sound);
      delay(1000);
      digitalWrite(7, LOW);
      settingdepth=5;
    }
    
    // if countdown is not over, then check if it is maximum value. 
    //If yes, set T as current unixtime plus number of seconds after which countdown stops.
    else if (settinglist[0]==Hlim && settinglist[1]==Mlim && settinglist[2]==Slim+1)
    {T=now.unixtime() + 3600*Hlim + 60*Mlim + Slim;}
    
    // Calculating gap between T and current unixtime
    gap= T - now.unixtime();
    settinglist[0]= gap/3600; //calculating hours left and storing it in setting list
    settinglist[1]=(gap/60)%60; //calculating minutes and storing it in setting list
    settinglist[2]= gap%60;    // calculating seconds and storing it in setting list
   }

   // if top button i spressed again, pausing the timer but updating T to fit the formula for gap when timer resumes.
   else {T= gap + now.unixtime();} 
   
   break;

   //Resetting the timer
   case 5:
   settinglist[0]=0;
   settinglist[1]=0;
   settinglist[2]=0;
   settingdepth=0;
   break;
  
  //Setting up the page by clearing the Scroll num list and allowing to adjust Hlim, Mlim and Slim
  default:
  for (int i=0; i<8; i++)
     {
      scroll_num[i]=0;
     }
  scroll_num[settingdepth]=1;
  settingdepth++;
  settinglist[2]=0;
  Scroll_height=0;
  Scroll_Hrange=60;
 }
//Displaying all the values for timer on the page; front end.
display.setTextColor(1,0);
display.setTextSize(1);
display.setCursor(90, 10);
display.print("st/sp");
display.setCursor(100, 33);
display.print("rset");
display.setCursor(100, 55);
display.print("Back");
display.setTextSize(2);
display.setCursor(25,30);
for (int i=0; i <3; i++)
  { 
    display.setTextColor(!scroll_num[i], scroll_num[i]);
   display.print(settinglist[i]);
   if (i!=2){display.print(":");}
  } 

}



void notification()
{ 
  //notification is received from BLE via the Tx and Rx pins of arduino and then stored in the EEPROM
  //The adress of the latest message byte is assinged to 'l', number of messages received is assinged to 'al', 
  //and lim is the adress starting from which all the messages in EEPROM are to be displayed(it is usd fro scrolling function
  // When settingdepth is 0, it show the terminal with messages
  // When setting depth is 1, it allows select options to clear history/ go back to terminal/ exit the notification page.
  // When settingdepth is 3, it executes the choosen option.
  
   switch (settingdepth)
  { 
    // Scrolling through options and storing that option in settinglist
    case 1:
    Scroll_Hrange=3;
    settinglist[0]=Scroll_height;
    display.setCursor(50, 40);
    if (Scroll_height==0){ display.print("Clr");}
    else if (Scroll_height==0){display.print("Read");}
    else{display.print("back");}
    break;
    
    //Executing option
    case 2:
    // To clear the EEPROM, and reset all the addresses
    if (settinglist[0]==0){Message.erase(); l=0; lim=0; settingdepth=0; al=0;}
    
    // Read option return you to terminal(settingdepth=0)
    else if (settinglist[0]==1){settingdepth=0;}
    
    //Back button goes to time screen and disables Screen_On so screen can shut and MCU go to sleep mode in inactivity
    else { Screen_On=false;  settingdepth=0;  Backbutton();} 
    break;

    default:
    // set lim to the adress from which messages are to be diplayed. it is increased or decreased by 1 using pushbuttons.
    lim = Message.read(max_characters+50+Scroll_height); 
    
   // Range to scroll is set to number of messages receiver plus 1(previous messages plus the upcoming one
    Scroll_Hrange=al+1; 
    
       //if data received via bluetooth, store it in EEPROM; adresses atartnig from 0 to max character plus 50 for buffer
       if (Serial.available()>0 )
       { 
        while (Serial.available()>0)
        { 
          //store it in EEPROM and increment adress
          Message.write(l, Serial.read());
          l++; 
        }
        // after storing the message, increment the variable for number of messages received('al')
        //and store 'l' in EEPROM's adress corresponding to max character plus buffer plus number of messages received.
        // this way we know the address of the newline characters. While scrolling, these addresses are set as 'lim'
        al++;
        Message.write(max_characters+50+al , l);
        
        // if messages exceed max charachers then overwrite the messages from 0.
        if (l>max_characters){l=0; al=0;}
        
        // acknowledgement sent to the phone after receiving and storing message
        Serial.println("ok");
       }
        //displaying messages staring from adress'lim' to latest character received
        display.setCursor(5,10);
        display.print(">");
        display.setTextSize(2);
        for (i=lim; i < max_characters+49; i++)
        {
          c = Message.read(i);
          display.print((char)c);
          if (c==10){display.print(">");}
        }
  }
}


void loop() 
{
  
  display.clearDisplay();
 /*  if (digitalRead(6))
   {
    if (Serial.available()>0)
    {
     
   //String S= Serial.readStringUntil("$");
   Serial.println( Serial.readStringUntil('$'));
    }
   }*/
  //If Scilent is set to false via INT0 interrupt,ring the alarm
  // alarm starts ringing at seconds=00. Due to 1 second delay in ringing, it may exceed 57, and ring for 58 seconds.
  if (!scilent && clock.getSecond()<57)
  {        
       display.display();
       digitalWrite(7, LOW);
       delay(500);
       display.drawBitmap(35, 15,Alarm, 50,50,WHITE);
       display.display();
       digitalWrite(7, Sound); // if sound is switched on then ring the buzzer too.
       delay(500); 
       Screen_On=true;
  }

  //set scilent to true so alarm ring stops. 
  //Check alarm function is called because it sets the alarm flag bit to 0 thereby reseting the INT/SQW pin on DS3231
  else
  { Screen_On=false;
    scilent=true; 
    clock.checkIfAlarm(1); 
    clock.checkIfAlarm(2); 
    digitalWrite(7, LOW);
   }
  //checks if screen had been on for more than 5 seconds, 
  //if yes, clear the adress, and Scroll num arrays, resets the setting mode, and puts MCU in idle mode.
  if (millis()-t>=5000 && !Screen_On)
  { 
    display.display();
    Scroll_depth=-1; Scroll_height=0; 
   for (int i=0; i<2; i++)
     {
      address[i]=0;
     }
  insettingmode=false;
  settingdepth=0;
    for (int i=0; i<8; i++)
     {
       scroll_num[i]=0;
     }
     
    // Sleep mode configured as IDLE and timers, ADC and BOD disabled. Refer Atmega 328 datasheet , page 34, for more info.
    //https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
   set_sleep_mode(SLEEP_MODE_IDLE);
    cli();
    sleep_enable();
    sleep_bod_disable();    
    power_adc_disable();
    power_timer0_disable();            //millis() clock disabbled
    power_timer1_disable();
    power_timer2_disable();
    sei();
    sleep_cpu();
  }
 
  else
  {
    if (Scroll_depth==0)        //screen for showing time(home)
    { Screen_On=false;
      Scroll_height=0;
      Timescreen();
    } 

    else if (Scroll_depth==1)           //menu
    { Screen_On=false;
      Menuscreen();
    }

    else if (Scroll_depth==2)           //After clicking an option in menu screen.
    { 
      insettingmode=true; 
      Screen_On=true;
      
      switch (address[1])
      {
      case 0: notification(); break;       //for notification
     
      case 1: setalarm(); break;           //for setting alarm

      case 2: settime(); break;            //for setting time
      
      case 3: stopwatch();   break;        //for Stopwatch                  
      
      case 4: timer();   break;            //for timer

      case 5:                              //setting sound on or off
      Scroll_Hrange=2;
      display.setCursor(30,25);
      if (Scroll_height==1) { Sound = !Sound; Scroll_height=0;}
      if (Sound){display.print("Sound:On");}
      else {display.print("Sound:Off");}
      if (settingdepth==1) { Backbutton();}
      break;
      
      default:                              //back button     
           Backbutton(); Screen_On=false;
      } 
    }
  }
  display.display();
  delay(1);
}
