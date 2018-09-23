/////////////////////
/*
GY39----MINI
VCC----VCC
GND----GND
1:RX---TX,send A5 02 A7 to GY-39
2:TX---RX
3:MINI_TX---FT232_RX
*/
//////////////////
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX ESP8266
String ydpd;
unsigned char Re_buf[15],counter=0;
unsigned char sign=0;
float Press,HUM,ALT;
typedef struct
{
    uint32_t P;
    uint16_t Temp;
    uint16_t Hum;
    uint16_t Alt;
} bme;
void setup() {
  //pinMode(4,INPUT); //yd 电位检测雨滴
 mySerial.begin(115200);
 Serial.begin(9600);  
  delay(900);    
  Serial.write(0XA5); 
  Serial.write(0X02);    //初始化,连续输出模式
  Serial.write(0XA7);    //初始化,连续输出模式
  delay(900); 
  mySerial.print("+++");
  delay(900); 
  mySerial.println("AT+CIPSTART=\"TCP\",\"www.lewei50.com\",80");
  delay(900); 
  mySerial.println("AT+CIPSEND");
  delay(900); 
 // mySerial.print("POST /api/V1/gateway/Updatesensors/01 HTTP/1.1\r\nuserkey:cc0a9e4e079d40d7893dd65581bc8281\r\nHost:open.lewei50.com \r\nContent-Length: 27 \r\nConnection: close\r\n\r\n[{\"Name\":\"tem\",\"Value\":31}]");
 


}

void loop() {
  unsigned char i=0,sum=0;
   uint16_t data_16[2]={0};
    bme Bme;
  if(sign)
  {   
  
     for(i=0;i<14;i++)
      sum+=Re_buf[i]; 
     if(sum==Re_buf[i] )        //检查帧头，帧尾
     {  	 
       Bme.Temp=(Re_buf[4]<<8)|Re_buf[5];
        data_16[0]=(Re_buf[6]<<8)|Re_buf[7];
        data_16[1]=(Re_buf[8]<<8)|Re_buf[9];
       Bme.P=(((uint32_t)data_16[0])<<16)|data_16[1];
       Bme.Hum=(Re_buf[10]<<8)|Re_buf[11];
       Bme.Alt=(Re_buf[12]<<8)|Re_buf[13];      
       //Serial.print("Temp: ");
       //Serial.print( (float)Bme.Temp/100);
       //Serial.print(" DegC  PRESS : ");
       //Serial.print( ((float)Bme.P)/10000);
       //Serial.print(" Pa  HUM : ");
       //Serial.print( (float)Bme.Hum/100);
       //Serial.print(" % ALT:");
       //Serial.print( Bme.Alt);
       //Serial.println("m");
       char tem[]="";
       dtostrf((float)Bme.Temp/100,2,2,tem);
       Press=((float)Bme.P)/10000;
       HUM=(float)Bme.Hum/100;
       ALT=Bme.Alt;
       delay(3000);
       ydpd=1023-analogRead(A0);
      /* int n =digitalRead(4);                   //创建一个变量n，将4号数字口的状态采集出来赋值给他。
       ydpd="1";
  if (n==HIGH)                             //判断n是否为高电平，如果是执行下面的语句，不是则跳过。
  {ydpd="0";}*/
  mySerial.print("+++");
  delay(900); 
  mySerial.println("AT+CIPSTART=\"TCP\",\"www.lewei50.com\",80");
  delay(9000); 
  mySerial.println("AT+CIPSEND");
       delay(12000);
      String data="[{\"Name\":\"tem\",\"Value\":\"" + (String)tem
       + "\"},{\"Name\":\"Press\",\"Value\":\""
       + Press
       +"\"},{\"Name\":\"HUM\",\"Value\":\""
       + HUM
       +"\"},{\"Name\":\"ALT\",\"Value\":\""
       +Bme.Alt
       +"\"},{\"Name\":\"yd\",\"Value\":\""
       +ydpd
       +"\"}]";
       String postRequest =(String)("POST ") + "/api/V1/gateway/Updatesensors/01 HTTP/1.1\r\nuserkey:cc0a9e4e079d40d7893dd65581bc8281\r\nHost:open.lewei50.com \r\nContent-Length: " + data.length() + "\r\n" +
          "Connection: close\r\n\r\n" +
          data+"\r\n";
       mySerial.println(postRequest);
          
       /*if (Bme.Alt>=10)
       {
       mySerial.print("POST /api/V1/gateway/Updatesensors/01 HTTP/1.1\r\nuserkey:cc0a9e4e079d40d7893dd65581bc8281\r\nHost:open.lewei50.com \r\nContent-Length: 126 \r\nConnection: close\r\n\r\n[{\"Name\":\"tem\",\"Value\":\"");
       mySerial.print((float)Bme.Temp/100);
       mySerial.print("\"},{\"Name\":\"Press\",\"Value\":\"");
       mySerial.print(((float)Bme.P)/10000);
       mySerial.print("\"},{\"Name\":\"HUM\",\"Value\":\"");
       mySerial.print((float)Bme.Hum/100);
       mySerial.print("\"},{\"Name\":\"ALT\",\"Value\":\"");
       mySerial.print(Bme.Alt);
       mySerial.print("\"}]");
       }
       else{
       mySerial.print("POST /api/V1/gateway/Updatesensors/01 HTTP/1.1\r\nuserkey:cc0a9e4e079d40d7893dd65581bc8281\r\nHost:open.lewei50.com \r\nContent-Length: 125 \r\nConnection: close\r\n\r\n[{\"Name\":\"tem\",\"Value\":\"");
       mySerial.print((float)Bme.Temp/100);
       mySerial.print("\"},{\"Name\":\"Press\",\"Value\":\"");
       mySerial.print(((float)Bme.P)/10000);
       mySerial.print("\"},{\"Name\":\"HUM\",\"Value\":\"");
       mySerial.print((float)Bme.Hum/100);
       mySerial.print("\"},{\"Name\":\"ALT\",\"Value\":\"");
       mySerial.print(Bme.Alt);
       mySerial.print("\"}]");}
       //mySerial.print("POST /api/V1/gateway/Updatesensors/01 HTTP/1.1\r\nuserkey:cc0a9e4e079d40d7893dd65581bc8281\r\nHost:open.lewei50.com \r\nContent-Length: 142 \r\nConnection: close\r\n\r\n[{\"Name\":\"tem\",\"Value:%d\",tem},{\"Name\":\"Press\",\"Value:%d\",Press},{\"Name\":\"HUM\",\"Value:%d\",HUM},{\"Name\":\"ALT\",\"Value:%d\",ALT}]");
        //delay(10000);*/
 

    
    sign=0;        
   }
  } 

}
void serialEvent() {

 
      while (Serial.available()) {   
      Re_buf[counter]=(unsigned char)Serial.read();
      if(counter==0&&Re_buf[0]!=0x5A) return;      // 检查帧头         
      counter++;       
      if(counter==15)                //接收到数据
      {    
         counter=0;                 //重新赋值，准备下一帧数据的接收 
         sign=1;
         //delay(5000);
      }      
      }

}
