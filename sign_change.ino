// test gsm module sim900
 

String numberCall_1 = "70000000000"; // Номер абонента №1 для звонка
String numberSMS_1 = "+70000000000"; // Номер абонента №1 для СМС (отличается только знаком +)

String numberCall_2 = "70000000000"; // Номер абонента №2 для звонка
String numberSMS_2 = "+70000000000"; // Номер абонента №2 для СМС (отличается только знаком +)

String textZone_1 = "Alarm! Zone1";    // Свое название зоны ,  на латинице.
String textZone_2 = "Alarm! Zone2";    // Свое название зоны ,  на латинице.
String textZone_3 = "Alarm! Zone3";    // Свое название зоны ,  на латинице.
String textZone_4 = "Alarm! Zone4";    // Свое название зоны ,  на латинице.





#define pinSensor_0 4 
#define pinSensor_1 5 
#define pinSensor_2 6 
#define pinSensor_3 7 
#define pinSensor_4 8 

// =========================================================================статус 
// изначально неготов
bool Status = false;  // общий статус
bool Phone_1_exist = false;   // 1-й телефонный номер получен
bool Phone_2_exist = false;   // 2-й телефонный номер получен
//
//доступность pinSensor(для отладки)
bool Enable_pinSensor_0 = false;  
bool Enable_pinSensor_1 = false;
bool Enable_pinSensor_2 = false;
bool Enable_pinSensor_3 = false;
bool Enable_pinSensor_4 = false;


String From_GSM;

void Enable_pinSensor(bool Set)
{
  Enable_pinSensor_0 = Set;  
  // для рабочей версии раскоментить
  /*
  Enable_pinSensor_1 = Set;
  Enable_pinSensor_2 = Set;
  Enable_pinSensor_3 = Set;
  Enable_pinSensor_4 = Set;
  */
}





//============================================================================



#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); 



void initGSM(void) {
  delay(2000);                            
  mySerial.begin(9600);                   // Выставляем скорость общения с GSM-модулем 9600 Бод/сек.  
  mySerial.println("AT+CLIP=1");          
  delay(300);                             
  mySerial.println("AT+CMGF=1");          
  delay(300);                             
  mySerial.println("AT+CSCS=\"GSM\"");    
  delay(300);                             
  mySerial.println("AT+CNMI=2,2,0,0,0");  
  delay(300);                             
}

/* Отправка SMS */
void sendSMS(String text, String phone) {
  mySerial.println("AT+CMGS=\"" + phone + "\""); 
  delay(500);
  mySerial.print(text);                         
  delay(500);
  mySerial.print((char)26);       
  delay(2500);  
}


unsigned long timerTemp = 0;  
uint8_t hours = 0;            

uint8_t flagSensor_0 = 0;
uint8_t flagSensor_1 = 0;
uint8_t flagSensor_2 = 0;
uint8_t flagSensor_3 = 0;
uint8_t flagSensor_4 = 0;

  

void setup() {
  mySerial.begin(9600);
  initGSM(); 
  
  pinMode(pinSensor_0, INPUT);
  pinMode(pinSensor_1, INPUT);
  pinMode(pinSensor_2, INPUT);
  pinMode(pinSensor_3, INPUT);
  pinMode(pinSensor_4, INPUT);

  timerTemp = millis();
  //========================================================
  Serial.begin(115200);
  Serial.println("setup complete");           
  //========================================================
}

void clearSerialBuffer() {
  while (Serial.available() > 0) {
    char dummy = Serial.read();
  }
}


void loop() {
  //========================================================
   while (!Status) 
   {
    String From_GSM;
    From_GSM = mySerial.readString(); 
    if(From_GSM.length()>0)
    {
      //Serial.println("==================" + From_GSM);           
       int clipIndex = From_GSM.indexOf("+CLIP");
        if (clipIndex != -1) 
        {
          char dummy;
           //Serial.println(From_GSM);    
           delay(1000); 
           int quoteIndex1 = From_GSM.indexOf('\"', clipIndex);
           //int quoteIndex2 = From_GSM.indexOf('\"', quoteIndex1 + 1);
           //String phoneNumber = From_GSM.substring(quoteIndex1 + 1, quoteIndex2);
           int quoteIndex2 = From_GSM.indexOf('\"', quoteIndex1 + 2);
           String phoneNumber = From_GSM.substring(quoteIndex1 + 2, quoteIndex2);
           if (!Phone_1_exist) 
           {
              Serial.println("Phone number 1: " + phoneNumber);           
              Phone_1_exist = true;
              numberCall_1 = phoneNumber;
              numberSMS_1 = '+' + phoneNumber;
              delay(3000);  // задержка что бы один звонок не воспринимался как два
              clearSerialBuffer();
              mySerial.print("ATA");    
              delay(10000);  // задержка что бы один звонок не воспринимался как два
              Serial.println("delay(10000); ");           
              mySerial.print("ATH0");   
              delay(1000);
              Serial.println("delay(1000); ");           

              clearSerialBuffer();

  //              dummy = mySerial.read();  //очистка буфера ком порта
              goto loc_1;
           }
           if (!Phone_2_exist) 
           {
              clearSerialBuffer();
              Serial.println("Phone number 2: " + phoneNumber);           
              numberCall_2 = phoneNumber;
              numberSMS_2 = '+' + phoneNumber;
              Phone_2_exist = true;
              delay(1000); // задержка что бы один звонок не воспринимался как два
              mySerial.print("ATH0");
              delay(1000); // задержка 
              
           }
           if ((Phone_1_exist)&(Phone_2_exist))  
           {
            Enable_pinSensor(true);
            Status = true;
            Serial.println("Phone number 1 CALL: " + numberCall_1);           
            Serial.println("Phone number 1 SMS : " + numberSMS_1);           
            Serial.println("Phone number 2 CALL: " + numberCall_2);
            Serial.println("Phone number 2 SMS : " + numberSMS_2);                      
            sendSMS(numberCall_1 + " " + numberCall_2, numberSMS_1); 
           }
loc_1:           
           delay(1);  // рустой оператор

           //Serial.println("Phone number: " + phoneNumber);          
        }
    } 
    
    

   }



  //========================================================
    if(millis() - timerTemp >= 3600000) {timerTemp = millis(); hours++;}

  if(hours >= 144) {// Меняем время контроля системы на свое,144 часа.кол-во часов .
    sendSMS(String("The system works normally.OK"), numberSMS_1); 
    delay(10000);                                  
    sendSMS(String("The system works normally.OK"), numberSMS_2); 
    delay(10000);                                  
    hours = 0;                                     
    timerTemp = millis();                         
  }
 //========================================================
  if (Enable_pinSensor_0)
  {
    if(flagSensor_0 == 0 && digitalRead(pinSensor_0) == 0) flagSensor_0 = 1;   
  }
  if (Enable_pinSensor_1)
  {
    if(flagSensor_1 == 0 && digitalRead(pinSensor_1) == 0) flagSensor_1 = 1;   
  }
  if (Enable_pinSensor_2)
  {
    if(flagSensor_2 == 0 && digitalRead(pinSensor_2) == 0) flagSensor_2 = 1;    
  }
  if (Enable_pinSensor_3)
  {
    if(flagSensor_3 == 0 && digitalRead(pinSensor_3) == 0) flagSensor_3 = 1;  
  }
  if (Enable_pinSensor_3)
  {
    if(flagSensor_4 == 0 && digitalRead(pinSensor_4) == 0) flagSensor_4 = 1;
  }
  
//========================================================
  if(flagSensor_0 == 1) {
    String command;

    command = "ATD+" + numberCall_1 + ";";  
    mySerial.println(command);              
    delay(20000);                           
    mySerial.println("ATH");                
    delay(1000);                            

   
    command = "ATD+" + numberCall_2 + ";";  
    mySerial.println(command);              
    delay(20000);                           
    mySerial.println("ATH");                
    delay(1000);                            

    flagSensor_0 = 2;                       
  }


  if(flagSensor_1 == 1) {
    sendSMS(textZone_1, numberSMS_1); 
    delay(10000);                     
    sendSMS(textZone_1, numberSMS_2); 
    delay(10000);                     
    flagSensor_1 = 2;                 
  }


  if(flagSensor_2 == 1) {
    sendSMS(textZone_2, numberSMS_1); 
    delay(10000);                     
    sendSMS(textZone_2, numberSMS_2); 
    delay(10000);                     
    flagSensor_2 = 2;                 
  }


  if(flagSensor_3 == 1) {
    sendSMS(textZone_3, numberSMS_1); 
    delay(10000);                     
    sendSMS(textZone_3, numberSMS_2); 
    delay(10000);                     
    flagSensor_3 = 2;                 
  }


  if(flagSensor_4 == 1) {
    sendSMS(textZone_4, numberSMS_1); 
    delay(10000);                    
    sendSMS(textZone_4, numberSMS_2); 
    delay(10000);                     
    flagSensor_4 = 2;                
  }

  if(flagSensor_0 == 2 && digitalRead(pinSensor_0) != 0) flagSensor_0 = 0;
  if(flagSensor_1 == 2 && digitalRead(pinSensor_1) != 0) flagSensor_1 = 0;
  if(flagSensor_2 == 2 && digitalRead(pinSensor_2) != 0) flagSensor_2 = 0;
  if(flagSensor_3 == 2 && digitalRead(pinSensor_3) != 0) flagSensor_3 = 0;
  if(flagSensor_4 == 2 && digitalRead(pinSensor_4) != 0) flagSensor_4 = 0;
}
