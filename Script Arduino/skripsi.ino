
#include <OneWire.h>
#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_Buttons.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

int pressed_button, butmenu, butmenu2, butmenu3,i,j,x=0,y,index_min1,index_min2,it,maksimum1,maksimum2;
boolean berhenti;
float particles_pos1[12][1],particles_pos2[12][1],local_value[12][1],local_best[12][1],local_value2[12][1],local_best2[12][1],value1[12][1],value2[12][1],pos1[12][1],pos2[12][1];
float particles_vel[12][1]={{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
float particles_vel2[12][1]={{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
float e = 0.0005;
float average1,average2,sum1,sum2,minimum1,minimum2,global_best1,global_best2,best_value1,best_value2,r1,r2,v_new1,v_new2,p_new1,p_new2;
float new_particles_pos1[12],new_particles_pos2[12],ave1,ave2;

const int AirValue = 620;   //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;int soilMoistureValue2 = 0;
int soilmoisturepercent=0;int soilmoisturepercent2=0;

// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

UTFT myGLCD(ITDB32WC,38,39,40,41);  ; 
URTouch      myTouch(6,5,4,3,2);
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

const int chipSelect = 10; //specific to Arduino UNO
File dataFile;

// PSO SENSOR
float GlobalBestA;
float GlobalBestB;
void start()
{
// Reading IF SERVER return PSO value
// 
/////////////////////////////////////////////////
if(Serial.available()){
      String data = Serial.readStringUntil(':');
      if(data == "A"){
        String data2 = Serial.readStringUntil('\n');
        GlobalBestA = data2.toFloat(); 
      }
      if(data == "B"){
        String data2 = Serial.readStringUntil('\n');
        GlobalBestB = data2.toFloat();
      }
  }
//////////////////////////////////////////////////
//cek
                  soilMoistureValue = analogRead(0);  //put Sensor insert into soil
                  soilMoistureValue2 = analogRead(1);
//                  Serial.println(soilMoistureValue);
//                  Serial.println(soilMoistureValue2);
                  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
                  soilmoisturepercent2 = map(soilMoistureValue2, AirValue, WaterValue, 0, 100);
                  Serial.print("Soil Moisture Value 1 : ");
                  Serial.println(soilmoisturepercent);
                  Serial.print("Soil Moisture Value 2 : ");
                  Serial.println(soilmoisturepercent2);
//                    y=y+1;
//                    Serial.println(y);
//                    pos1[y][1]=soilmoisturepercent;
//                    pos2[y][1]=soilmoisturepercent2;
//                        sum1 = sum1+soilmoisturepercent;
//                        sum2 = sum2+soilmoisturepercent2;
//                    if(y==12){
//                      ave1=sum1/12;
//                      Serial.println(ave1);
//                      ave2=sum2/12;
//                      Serial.println(ave2);
//                      y=0;
                  if(soilmoisturepercent>25&&soilmoisturepercent<50){
                    digitalWrite(9, HIGH);
                    digitalWrite(8, LOW);
                    delay(120000);
                    digitalWrite(9, LOW);
                  }
                  else if(soilmoisturepercent<25){
                    digitalWrite(9, HIGH);
                    digitalWrite(8, LOW);
                    delay(60000);
                    digitalWrite(9, LOW);
                  }
                  if(soilmoisturepercent2>25&&soilmoisturepercent2<50){
                    digitalWrite(8, HIGH);
                    digitalWrite(9, LOW);
                    delay(120000);
                    digitalWrite(8, LOW);
                  }
                  else if(soilmoisturepercent2<25){
                    digitalWrite(8, HIGH);
                    digitalWrite(9, LOW);
                    delay(60000);
                    digitalWrite(8, LOW);
                  }
//              }
//     delay(1000);
//  DateTime now = rtc.now();
//    
//  // make a string for assembling the data to log:
//  String dataString = "";
//
//   dataString += String(now.year(), DEC);
//   dataString += String('/');
//   dataString += String(now.month(), DEC);
//   dataString += String('/');
//   dataString += String(now.day(), DEC);
//   dataString += String(" (");
//   dataString += String(daysOfTheWeek[now.dayOfTheWeek()]);
//   dataString += String(") ");
//   dataString += String(now.hour(), DEC);
//   dataString += String(':');
//   dataString += String(now.minute(), DEC);
//   dataString += String(':');
//   dataString += String(now.second(), DEC);
//   dataString += String(" Response: ");
   
//
//   if(sensor > 400) {
//     digitalWrite(ledPin, HIGH);
//     dataString += String(" : FLOW ON");
//   } 
//   else {
//     digitalWrite(ledPin, LOW);
//     dataString += String(" : FLOW OFF");
//   }  
//  
//  dataFile.println(dataString);
  
  // The following line will 'save' the file to the SD card after every
  // line of data - this will use more power and slow down how much data
  // you can read but it's safer! 
  
  // If you want to speed up the system, remove the call to flush() and it
  // will save the file only every 512 bytes - every time a sector on the 
  // SD card is filled with data.
  
//  dataFile.flush();

  // print our dataString to the serial port too:
//  Serial.println(dataString);
  myButtons.deleteAllButtons();
     //set colour for the title, display title and position on x axis(center) and position on y axis(0, 16)
   
    myGLCD.setColor(255, 215, 0);                         
    myGLCD.print("BINTANG PRATAMA", CENTER, 0);
    myGLCD.print("PROGRAM BY", CENTER, 16);
    myGLCD.print("MUHAMMAD MUTHO", CENTER, 32); 
//set colour to red, name and sensor names with positions
    myGLCD.setColor(255, 0, 50);                       
    myGLCD.print("Capasitive Soil", CENTER, 64);
    myGLCD.print("Moisture", CENTER, 80); 
//set colour to green, more names and positions 
//    myGLCD.setColor(0, 255, 0);                         
//    myGLCD.print("VFI PROJECT", CENTER, 130);
    butmenu = myButtons.addButton( 10, 180, 300, 30, "Menu");
    myButtons.drawButtons();
 
//          while (myTouch.dataAvailable() == true)
//   
//      {
//          pressed_button = myButtons.checkButtons();
//              if (pressed_button==butmenu)
//                  {
//                      menu();
//                      myButtons.deleteButton(butmenu);
//                      
//                  }
//      }
//    
  
}
void setup()
  {
    y=0;
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    myGLCD.InitLCD();
    myGLCD.clrScr();
    myGLCD.setFont(BigFont);
    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);
    myButtons.setTextFont(BigFont);
    
    Serial.begin(115200); // Dubugging on hardware Serial 0
    Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
//    while (1) ;    
  }
  Serial.println("card initialized.");
  
  // Open up the file we're going to log to!
  dataFile = SD.open("datalog.txt", FILE_WRITE); 
  // will create the file if it doesn't exist  
  if (! dataFile) {
    Serial.println("error opening datalog.txt");
    // Wait forever since we can't write data
//    while (1) ;
  }
  #ifdef AVR

  Wire.begin();

  #else

  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due

#endif
  rtc.begin();
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}
void loop()
  {
      boolean default_colors = true;
      start();
  }       
         
void ldr()     
      {  
       
      }
   

void menubutton()
     {
 
        if (pressed_button==butmenu)
        myGLCD.clrScr();
        myButtons.deleteButton(butmenu);
//        menu();
     }

void menu()
    {
        myButtons.deleteAllButtons();
        myGLCD.clrScr();
//        myGLCD.print("Menu", CENTER, 0);
        myGLCD.setColor(0, 255, 0);               
        butmenu = myButtons.addButton( 10, 100, 300, 30, "POMPA MANUAL A");
        butmenu2 = myButtons.addButton( 10, 150, 300, 30, "POMPA MANUAL B");
        myButtons.drawButtons();
 
     while (true)
        {
                  if (myTouch.dataAvailable() == true)
                  {
                     pressed_button = myButtons.checkButtons();
                      if (pressed_button==butmenu3)
                      {
                        dhtmenu();
                        myButtons.deleteButton(butmenu3);
                      }
                      else if (pressed_button==butmenu2)
                      {
                        dhtmenu2();
                        myButtons.deleteButton(butmenu2);
                      }
                  }
                   
                  soilMoistureValue = analogRead(0);  //put Sensor insert into soil
                  soilMoistureValue2 = analogRead(1);
//                  Serial.println(soilMoistureValue);
//                  Serial.println(soilMoistureValue2);
                  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
                  soilmoisturepercent2 = map(soilMoistureValue2, AirValue, WaterValue, 0, 100);
                  if(soilmoisturepercent>25&&soilmoisturepercent<50){
                    digitalWrite(8, HIGH);
                    digitalWrite(9, LOW);
                    delay(60000);
                    digitalWrite(8, LOW);
                  }
                  else if(soilmoisturepercent<25){
                    digitalWrite(8, HIGH);
                    digitalWrite(9, LOW);
                    delay(120000);
                    digitalWrite(8, LOW);
                  }
                  if(soilmoisturepercent2>25&&soilmoisturepercent2<50){
                    digitalWrite(9, HIGH);
                    digitalWrite(8, LOW);
                    delay(60000);
                    digitalWrite(9, LOW);
                  }
                  else if(soilmoisturepercent2<25){
                    digitalWrite(9, HIGH);
                    digitalWrite(8, LOW);
                    delay(120000);
                    digitalWrite(9, LOW);
                  }
                  //Lupakan ini bang. Ngga masuk di codingan
                  x=x+1;
                  if(x=10){
                    x=0;
                    y=y+1;
                    pos1[y][1]=soilmoisturepercent;
                    pos2[y][1]=soilmoisturepercent2;
                    if(y=12){
                      for(i=0;i<12;i++){
//                        int sum1 += pos1[i][1];
//                        int sum2 += pos2[i][1];
                     }
                      y=0;
                  }
                  
                  myGLCD.print(" Soil Moisture A ", CENTER, 0);
                  myGLCD.printNumI(soilmoisturepercent, CENTER, 20);
                  myGLCD.drawRoundRect (120, 18, 200, 40);
                  myGLCD.print(" Soil Moisture B ", CENTER, 50);
                  myGLCD.printNumI(soilmoisturepercent2, CENTER, 70);
                  myGLCD.drawRoundRect (120, 68, 200, 90);
               
                  delay(2000);
        }       
    }
    }


void dhtmenu()
  {
                        myGLCD.print("POMPA POT A ON", CENTER, 200);
                        delay(5000);
                        menu();
  }
 void dhtmenu2()
  {
                        myGLCD.print("POMPA POT B ON", CENTER, 200);
                        delay(5000);
                        menu();
  }
//  float f(float x, float average){
//    float y = pow((100 - (average + x)), 2);
//    return y;
//  }

//  float pso(float data1[12][1], float data2[12][1]){
//    float e=0.0005; boolean lr_reduce=true; boolean minimum=true; int n_particles=12; int iteration=5; int ndim=1; float c1=0.3;float c2=0.4; float w=0.9; float l_teta=0.4; float u_teta=0.9;
//    for(i=0;i<1;i++){
//        for(j=0;j<12;j++){
//            particles_pos1[j][i]=data1[j][i];
//            particles_pos2[j][i]=data2[j][i];
////          particles_vel[j][i]=
//            sum1 = sum1 + particles_pos1[j][i];
//            sum2 = sum2 + particles_pos2[j][i];
//        }
//    }
//    local_value[j][i]={};
//    local_value2[j][i]={};
//    local_best[j][i]={};
//    local_best2[j][i]={};
//    average1=sum1/12;
//    average2=sum2/12;
//    for(i=0;i<1;i++){
//        for(j=0;j<12;j++){
//            value1[j][i]=f(particles_pos1[j][i], average1);
//            value2[j][i]=f(particles_pos2[j][i], average2);
//            local_value[j][i]=value1[j][i];
//            local_value2[j][i]=value2[j][i];
//            local_best[j][i]=particles_pos1[j][i];
//            local_best2[j][i]=particles_pos2[j][i];
//        }
//    }
//    minimum1 = local_value[0][0];
//    minimum2 = local_value2[0][0];
//    for(i=0;i<12;i++){
//      if(local_value[i][0]<minimum1){
//        minimum1=local_value[i][0];
//        index_min1=i; 
//      }
//      if(local_value2[i][0]<minimum2){
//        minimum2=local_value2[i][0];
//        index_min2=i; 
//      }
//    }
//    best_value1=minimum1;
//    best_value2=minimum2;
//    global_best1=local_best[index_min1][0];
//    global_best2=local_best2[index_min2][0];
//    it = 0;
//    berhenti = false;
//    for(i=0;i<5;i++){
//        if(lr_reduce=true){
//          w = u_teta - ((u_teta - l_teta)/iteration)*(it+1);
//        }
//        for (i=0;i<12;i++){
//          for(j=0;j<1;j++){
//            r1=random(0,1);
//            r2=random(0,1);
//            v_new1=w*particles_vel[i][j] + c1*r1*(local_best[i][j]-particles_pos1[i][j]) + c2*r2*(global_best1-particles_pos1[i][j]);
//            v_new2=w*particles_vel2[i][j] + c1*r1*(local_best2[i][j]-particles_pos2[i][j]) + c2*r2*(global_best2-particles_pos2[i][j]);
//            p_new1=v_new1+particles_pos1[i][j];
//            p_new2=v_new2+particles_pos2[i][j];
//            new_particles_pos1[j]=p_new1;
//            new_particles_pos2[j]=p_new2;
//            particles_vel[i][j]=v_new1;
//            particles_vel2[i][j]=v_new2;
//          }
//          if(minimum=true){
//            if(f(new_particles_pos1[i], average1)<f(particles_pos1[i][j],average1)){
//              local_best[i][j]=new_particles_pos1[i];
//              local_value[i][j]=f(new_particles_pos1[i], average1); 
//            }
//            if(f(new_particles_pos2[i], average2)<f(particles_pos2[i][j],average2)){
//              local_best2[i][j]=new_particles_pos2[i];
//              local_value2[i][j]=f(new_particles_pos2[i], average2); 
//            }
//          }
//          else{
//            if(f(new_particles_pos1[i], average1)>f(particles_pos1[i][j],average1)){
//              local_best[i][j]=new_particles_pos1[i];
//              local_value[i][j]=f(new_particles_pos1[i], average1); 
//            }
//            if(f(new_particles_pos2[i], average2)>f(particles_pos2[i][j],average2)){
//              local_best2[i][j]=new_particles_pos2[i];
//              local_value2[i][j]=f(new_particles_pos2[i], average2); 
//            }
//          }
//        }
//        if(minimum=true){
//          for(i=0;i<12;i++){
//            if(local_value[i][0]<minimum1){
//              minimum1=local_value[i][0];
//              index_min1=i; 
//            }
//            if(local_value2[i][0]<minimum2){
//              minimum2=local_value2[i][0];
//              index_min2=i; 
//            }
//          }
//          best_value1=minimum1;
//          best_value2=minimum2;
//          global_best1=local_best[index_min1][0];
//          global_best2=local_best2[index_min2][0];
//          maksimum1 = local_value[0][0];
//          maksimum2 = local_value2[0][0];
//        }
//        else{
//          for(i=0;i<12;i++){
//            if(local_value[i][0]>maksimum1){
//              maksimum1=local_value[i][0];
//              index_min1=i; 
//            }
//            if(local_value2[i][0]>maksimum2){
//              maksimum2=local_value2[i][0];
//              index_min2=i; 
//            }
//          }
//          best_value1=maksimum1;
//          best_value2=maksimum2;
//          global_best1=local_best[index_min1][0];
//          global_best2=local_best2[index_min2][0];
//        }
//      
//    }
//    Serial.println(best_value1);
//    Serial.println(best_value2);
//    Serial.println(global_best1);
//    Serial.println(global_best2);
//    return global_best1, global_best2, best_value1, best_value2;
//  }
