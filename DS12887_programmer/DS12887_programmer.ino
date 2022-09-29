// DS1288 ADDRESS REGISTERS
const uint8_t REGISTER_SECONDS = 0x00; // Seconds
const uint8_t REGISTER_SECONDS_ALARM = 0x01; // Seconds alarm
const uint8_t REGISTER_MINUTES = 0x02; // Minutes
const uint8_t REGISTER_MINUTES_ALARM = 0x03; // Minutes alarm
const uint8_t REGISTER_HOURS = 0x04; // Hours
const uint8_t REGISTER_HOURS_ALARM = 0x05; // Hours alarm
const uint8_t REGISTER_DOW = 0x06; // Day of week
const uint8_t REGISTER_DOM = 0x07; // Day of month
const uint8_t REGISTER_MONTH = 0x08; // Month
const uint8_t REGISTER_YEAR = 0x09; // Year
const uint8_t REGISTER_A = 0x0A; // REGISTER A
const uint8_t REGISTER_B = 0x0B; // REGISTER B
const uint8_t REGISTER_C = 0x0C; // REGISTER C
const uint8_t REGISTER_D = 0x0D; // REGISTER D (the last bit- battary state)

// dataPins = AD0 - AD7
// Array with pin numbers connected to data pins of ds1287 (need puldown resistors)
int dataPins[8] = {7, 8, 9, 10, 11, 12, 6, 2 };
//int dataPins[8] = {9, 8, 7, 6, 5, 4, 3, 2}; //INVERTED
int CS = A4; // SELECT CHIP
int AS = A3; // STROBE ADDRESS
int WR = A2; // HIGH READ LOW WRITE
int DS = A1; // READ

// my array to write to RAM
//uint8_t ramDump[128]= {16, 153, 32, 153, 8, 153, 5, 35, 9, 34, 38, 0, 80, 128, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 16, 153, 32, 153, 8, 153, 5, 35, 9, 34, 38, 0, 64, 0, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255} ;

/////// ELEMENTARY FUNCTIONS IMPLEMENTED FOR INTEL BUS TIMING (DS1287 first pin should be connected to GND or disconected))

boolean getBit(uint8_t myVarIn, uint8_t whatBit) 
{
 boolean bitState;
 bitState = myVarIn & (1 << whatBit); 
 return bitState; 
} 

uint8_t setBit(uint8_t myVarIn, uint8_t whatBit, boolean bitState) 
{ 
 if (bitState) { myVarIn |= (1 << whatBit); } 
 else { myVarIn &= ~(1 << whatBit); } 
 return myVarIn; 
} 

void switchRead() 
{ 
 for (int i=0; i<8; i++) 
 { 
  pinMode(dataPins[i], INPUT); 
 } 
} 
void switchWrite() 
{ 
  for (int i=0; i<8; i++) 
  { 
    pinMode(dataPins[i], OUTPUT);
  } 
} 
   
uint8_t readBusData() 
{ 
 uint8_t dataBuffer = 0b00000000; 
 for (int i=0; i<8; i++) 
 { 
  boolean state = digitalRead(dataPins[i]); dataBuffer = setBit(dataBuffer, i, state); 
 } 
return dataBuffer; 
} 

void writeBusData(uint8_t data) 
{ 
  for (int i=0; i<8; i++) 
  { 
   digitalWrite(dataPins[i], getBit(data, i)); 
  } 
} 

void setIdle() 
{ 
  digitalWrite(CS, HIGH);
  digitalWrite(AS, HIGH); 
  digitalWrite(WR, HIGH); 
  digitalWrite(DS, HIGH); 
} 

void WriteRegisterData (uint8_t registerAddr,uint8_t data)
{
  switchWrite();  
  digitalWrite(CS, LOW); 
  delayMicroseconds(20); 
  writeBusData(registerAddr);
  delayMicroseconds(20); 
  digitalWrite(AS, LOW); 
  delayMicroseconds(20);
  writeBusData(data);
  delayMicroseconds(20);
  digitalWrite(WR, LOW); 
  delayMicroseconds(20);
  digitalWrite(WR, HIGH);  
  delayMicroseconds(20);
  digitalWrite(AS, HIGH); 
  delayMicroseconds(20); 
  digitalWrite(CS, HIGH);  
}
 
  uint8_t getRegisterData(uint8_t registerAddr) 
  { 
  uint8_t dataBuffer = 0b00000000; 
  switchWrite(); 
  digitalWrite(CS, LOW); 
  delayMicroseconds(20); 
  writeBusData(registerAddr); 
  digitalWrite(AS, LOW); 
  delayMicroseconds(20); 
  switchRead(); 
  delayMicroseconds(20); 
  digitalWrite(DS, LOW); 
  delayMicroseconds(20); 
  dataBuffer = readBusData(); 
  digitalWrite(DS, HIGH); 
  delayMicroseconds(20); 
  digitalWrite(AS, HIGH); 
  delayMicroseconds(20); 
  digitalWrite(CS, HIGH); 
  return dataBuffer; 
  } 

void Control_reg_to_serial()
  {
    Serial.print("  REG_A "); 
    Serial.println(getRegisterData(REGISTER_A));
    Serial.print("  REG_B "); 
    Serial.println(getRegisterData(REGISTER_B));
    Serial.print("  REG_C "); 
    Serial.println(getRegisterData(REGISTER_C));
    Serial.print("  REG_D "); 
    Serial.println(getRegisterData(REGISTER_D));
  }  

/////functions for work wih RTC data in BCD mode (register B bit 2 =1)

void SetTime(uint8_t year,uint8_t month,uint8_t dom,uint8_t dow,uint8_t hour,uint8_t min,uint8_t sec)
    {
    WriteRegisterData(REGISTER_SECONDS,sec); 
    WriteRegisterData(REGISTER_MINUTES,min); 
    WriteRegisterData(REGISTER_HOURS,hour); 
    WriteRegisterData(REGISTER_DOW,dow); 
    WriteRegisterData(REGISTER_DOM,dom); 
    WriteRegisterData(REGISTER_MONTH,month); 
    WriteRegisterData(REGISTER_YEAR,year); 
    }

void RTC_date_to_serial()
{
  Serial.print("("); 
  Serial.print(getRegisterData(REGISTER_DOW)); 
  Serial.print(") "); 
  Serial.print(getRegisterData(REGISTER_MONTH));     
  Serial.print("/"); 
  Serial.print(getRegisterData(REGISTER_DOM));
  Serial.print("/"); 
  Serial.print(getRegisterData(REGISTER_YEAR));
  Serial.print(" ");
  Serial.print(getRegisterData(REGISTER_HOURS)); 
  Serial.print(":"); 
  Serial.print(getRegisterData(REGISTER_MINUTES)); 
  Serial.print(".");
  Serial.println(getRegisterData(REGISTER_SECONDS));
}

void SetAlarmTime(uint8_t hour,uint8_t min,uint8_t sec)
{
 WriteRegisterData(REGISTER_SECONDS_ALARM,sec); 
 WriteRegisterData(REGISTER_MINUTES_ALARM,min); 
 WriteRegisterData(REGISTER_HOURS_ALARM,hour); 
}

   void Alarm_time_to_serial()
   {
      Serial.print("Alarm time: ");
      Serial.print(getRegisterData(REGISTER_HOURS_ALARM)); 
      Serial.print(":"); 
      Serial.print(getRegisterData(REGISTER_MINUTES_ALARM)); 
      Serial.print(".");
      Serial.println(getRegisterData(REGISTER_SECONDS_ALARM));
   }


/////functions for work wih RTC data in BCD mode (register B bit 2 =0)

uint8_t BCDreadVal(uint8_t addr ) 
  { 
    uint8_t val, val10;
    val= getRegisterData(addr);
    val10 = val>>4;
    val = val&0x0F;
    val = 10*val10 + val; 
    return val;
  } 

void BCDwriteVal(uint8_t addr, uint8_t val) 
  { 
    uint8_t val10;
    val10 = (val/10)<<4;
    val = val%10;  
    val = val10|val; 
    WriteRegisterData(addr, val);
  } 

void BCD_RTC_date_to_serial()
   {
      Serial.print("("); 
      Serial.print(BCDreadVal(REGISTER_DOW)); 
      Serial.print(") ");
      Serial.print(BCDreadVal(REGISTER_MONTH));       
      Serial.print("/");
      Serial.print(BCDreadVal(REGISTER_DOM));
      Serial.print("/");
      Serial.print(BCDreadVal(REGISTER_YEAR)); 
      Serial.print(" ");
      Serial.print(BCDreadVal(REGISTER_HOURS));
      Serial.print(":"); 
      Serial.print(BCDreadVal(REGISTER_MINUTES));
      Serial.print(".");
      Serial.println(BCDreadVal(REGISTER_SECONDS));
   }


  void BCD_SetTime(uint8_t year,uint8_t month,uint8_t dom,uint8_t dow,uint8_t hour,uint8_t min,uint8_t sec)
    {
    BCDwriteVal(REGISTER_SECONDS,sec); 
    BCDwriteVal(REGISTER_MINUTES,min); 
    BCDwriteVal(REGISTER_HOURS,hour); 
    BCDwriteVal(REGISTER_DOW,dow); 
    BCDwriteVal(REGISTER_DOM,dom); 
    BCDwriteVal(REGISTER_MONTH,month); 
    BCDwriteVal(REGISTER_YEAR,year); 
    }

 void BCD_Alarm_time_to_serial()
{
 Serial.print("Alarm time: ");
 Serial.print(BCDreadVal(REGISTER_HOURS_ALARM)); 
 Serial.print(":"); 
 Serial.print(BCDreadVal(REGISTER_MINUTES_ALARM)); 
 Serial.print(".");
 Serial.println(BCDreadVal(REGISTER_SECONDS_ALARM));
}

void BCD_SetAlarmTime(uint8_t hour,uint8_t min,uint8_t sec)
{
 BCDwriteVal(REGISTER_SECONDS_ALARM,sec); 
 BCDwriteVal(REGISTER_MINUTES_ALARM,min); 
 BCDwriteVal(REGISTER_HOURS_ALARM,hour); 
}

///////// functions for work with RAM
void writeRAM (uint8_t start_reg,uint8_t end_reg,uint8_t ramArray[])
{
  for (uint8_t i=start_reg; i<=end_reg; i++) 
    {         
     WriteRegisterData (i,ramArray[i]);
     Serial.print(i);
     Serial.print(": "); 
     Serial.print(ramArray[i]);
     if (i==0x7F)   Serial.println(" /done");
      else Serial.print(", ");
    }
}

void readRAM_to_serial (uint8_t start_reg, uint8_t end_reg)
  {
    for (uint8_t i=start_reg; i<=end_reg; i++) 
     {  
        Serial.print(i);
        Serial.print(": ");    
        Serial.print(getRegisterData(i));
        if (i==0x7F) Serial.println(" /done");
        else Serial.print(", ");
      }
  }

/////////////////////////////////////////////////////////////////////
    
void setup() 
  {
    pinMode(CS, OUTPUT); 
    pinMode(AS, OUTPUT); 
    pinMode(WR, OUTPUT); 
    pinMode(DS, OUTPUT); 
    setIdle(); 
    switchRead(); 
    Serial.begin(9600);

    //writeRAM (0x00,0x7F,ramDump);
    readRAM_to_serial (0x00,0x7F);
    //SetTime(22,9,28,3,14,25,0);
     BCD_SetTime(22,9,28,3,14,29,0);    
  }
    
void loop() 
{ 
   //RTC_date_to_serial();
   BCD_RTC_date_to_serial();
   BCD_Alarm_time_to_serial();
   Control_reg_to_serial();  
   delay(1000);    
}
