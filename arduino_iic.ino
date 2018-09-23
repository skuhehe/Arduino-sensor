#include <Wire.h>
//#include <i2cmaster.h>
/////////////////////////////////
/*
GY-39-----mini
VCC----VCC
SCL----A5
SDA----A4
GND--GND
*/


//////////////////////////////////
#define uint16_t unsigned int
#define iic_add  0x5b
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX ESP8266
String ydpd;
float Press,HUM,ALT;
typedef struct
{
    uint32_t P;
    uint16_t Temp;
    uint16_t Hum;
    uint16_t Alt;
} bme;

bme Bme;
  uint32_t Lux;

void i2c_speed(uint16_t scl_speed)
{
  /* initialize TWI clock: 40 kHz clock, TWPS = 0 => prescaler = 1 */
  
  TWSR = 0;                         /* no prescaler */
  TWBR = ((F_CPU/scl_speed)-16)/2;  /* must be > 10 for stable operation */

}
void setup() {
 mySerial.begin(115200);
       Serial.begin(115200);
       Wire.begin();
       i2c_speed(40000);
       delay(1); 
       delay(900); 
 /* mySerial.print("+++");
  delay(900); 
  mySerial.println("AT+CIPSTART=\"TCP\",\"www.lewei50.com\",80");
  delay(900); 
  mySerial.println("AT+CIPSEND");*/
}
void loop() {

    get_bme();
    /*Serial.print("Temp: ");
    Serial.print( (float)Bme.Temp/100);
    Serial.print(" DegC  PRESS : ");
    Serial.print( ((float)Bme.P)/100);
    Serial.print(" Pa  HUM : ");
    Serial.print( (float)Bme.Hum/100);
    Serial.print(" % ALT:");
    Serial.print( Bme.Alt);
    Serial.println("m");*/
    get_lux();
   /* Serial.print( "Lux: ");
    Serial.print( ((float)Lux)/100);
    Serial.println(" lux");*/
    delay(200); 
    char te[]="";
       dtostrf((float)Bme.Temp/100,2,2,te);
       String tem=te;
       //tem=(float)Bme.Temp/100;
       Press=((float)Bme.P)/10000;
       HUM=(float)Bme.Hum/100;
       ALT=Bme.Alt;
       float lux=((float)Lux)/100;
       delay(3000);
       ydpd=1023-analogRead(A0);
       //if(Press<5000&&ALT<3000&&HUM<105&&lux<100000)
       if(Press<5000){
        if(lux<100000){
        post(tem,Press,HUM,ALT,ydpd,lux);}
        }
      /* int n =digitalRead(4);                   //创建一个变量n，将4号数字口的状态采集出来赋值给他。
       ydpd="1";
  if (n==HIGH)                             //判断n是否为高电平，如果是执行下面的语句，不是则跳过。
  {ydpd="0";}*/
 
}
void get_bme(void)
{
    uint16_t data_16[2]={0};
    uint8_t data[10]={0};
   iic_read(0x04,data,10);
    //iic_read2(0x04,data,10);
   Bme.Temp=(data[0]<<8)|data[1];
   data_16[0]=(data[2]<<8)|data[3];
   data_16[1]=(data[4]<<8)|data[5];
   Bme.P=(((uint32_t)data_16[0])<<16)|data_16[1];
   Bme.Hum=(data[6]<<8)|data[7];
   Bme.Alt=(data[8]<<8)|data[9];
}
void get_lux(void)
{     
    uint16_t data_16[2]={0};
    uint8_t data[10]={0};
    //iic_read2(0x00,data,4);
    iic_read(0x00,data,4);
    data_16[0]=(data[0]<<8)|data[1];
    data_16[1]=(data[2]<<8)|data[3];
    Lux=(((uint32_t)data_16[0])<<16)|data_16[1];

}
void iic_read(unsigned char reg,unsigned char *data,uint8_t len )//#include <Wire.h>
{
   Wire.beginTransmission(iic_add);  
   Wire.write(reg); 
   Wire.endTransmission(false);
   delayMicroseconds(10);
   if(len>4)
     Wire.requestFrom(iic_add,10);     
   else
     Wire.requestFrom(iic_add,4);
   for (uint8_t i = 0; i < len; i++)
    {  
       data[i] = Wire.read(); 
   }
  
}
void post(String tem,float Press,float HUM,float Alt,String ydpd,float lux)
{
   mySerial.print("+++");
   delay(600);
   mySerial.println("AT+CWJAP=TP-LINK_B551,doudou02");
  delay(300); 
  mySerial.println("AT+CIPSTART=\"TCP\",\"www.lewei50.com\",80");
  delay(300); 
  mySerial.println("AT+CIPSEND");
       delay(2000);
      String data="[{\"Name\":\"tem\",\"Value\":\"" +(String)tem
       + "\"},{\"Name\":\"Press\",\"Value\":\""
       + Press
       +"\"},{\"Name\":\"HUM\",\"Value\":\""
       + HUM
       +"\"},{\"Name\":\"ALT\",\"Value\":\""
       +Bme.Alt
       +"\"},{\"Name\":\"yd\",\"Value\":\""
       +ydpd
       +"\"},{\"Name\":\"lux\",\"Value\":\""
       +lux
       +"\"}]";
       //Serial.println(data);
       String apostRequest =(String)("POST ") + "/api/V1/gateway/Updatesensors/01 HTTP/1.1\r\nuserkey:cc0a9e4e079d40d7893dd65581bc8281\r\nHost:open.lewei50.com \r\nContent-Length: " + data.length() + "\r\n" +
          "Connection: close\r\n\r\n" ;
         // String bdata=data+"\r\n";
       mySerial.print(apostRequest);
       delay(1);
       mySerial.println(data);
       mySerial.println("\r\n");
       //Serial.println(apostRequest);
       delay(1);
       //Serial.println(data);
       delay(18000);
  }
//void iic_read2(unsigned char add,unsigned char *data,unsigned char len)//#include <i2cmaster.h>
//{
//  i2c_start_wait(0xb6);
//   i2c_write(add);
//   i2c_start_wait(0xb6+1);
//   while(len-1)
//   {
//    *data++=i2c_readAck();
//    len--;
//    }
//    *data=i2c_readNak();
//    i2c_stop();
//}
