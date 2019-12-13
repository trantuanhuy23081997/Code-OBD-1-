int OBD2_Convert_Hex(char a, char b)
{
  int Num_High, Num_Low;
  a = toupper(a);
  b = toupper(b);
  //Convert firt tring of hex value
  if ((int(a)>= ASCII_A) && (int(a)<= ASCII_F)){
    Num_High = int(a) - 55;
  }
  else if ((int(a)>= ASCII_0) && (int(a)<= ASCII_9)){
    Num_High = int(a) - 48;
  }
  //Convert second string of hex calue
  if((int(b)>= ASCII_A) && (int(b)<= ASCII_F))
  {
    Num_Low = int(b) - 55;
  }
  else if ((int(b)>= ASCII_0) && (int(b)<= ASCII_9)){
    Num_Low = int(b) - 48;
  }
  //Conver to integer
  return (Num_High*16 + Num_Low);
}
int OBD2_Read_DTC(char* Data_DTC_Raw , int Length_of_DTC ,char* data_buffer_DTC)
{
  int OBD2_RECEIVE_BUFF_LEN = Length_of_DTC;
  char SubrxIndex = 0;
  unsigned char Byte_A = 0, Byte_B = 0, Byte_C = 0, Byte_D = 0, Byte_temp = 0;
  bool twobyte = 0;
  
  for (int dtc_idx = 0; dtc_idx < OBD2_RECEIVE_BUFF_LEN; dtc_idx++)
  {
    if ((Data_DTC_Raw[dtc_idx]=='4') && (Data_DTC_Raw[dtc_idx+1]=='3') 
    &&(strtol(&Data_DTC_Raw[dtc_idx+3],0,16) != 0))
    {
      // Convert to Byte A, B, C, D
      Byte_A = OBD2_Convert_Hex(Data_DTC_Raw[dtc_idx+6], Data_DTC_Raw[dtc_idx+7]);
      Byte_B = OBD2_Convert_Hex(Data_DTC_Raw[dtc_idx+9], Data_DTC_Raw[dtc_idx+10]);
      // If ODB return 2 DTCs
      if (OBD2_Convert_Hex(Data_DTC_Raw[dtc_idx+3], Data_DTC_Raw[dtc_idx+4]) >= 2)
      {
        twobyte = 1;
        Byte_C = OBD2_Convert_Hex(Data_DTC_Raw[dtc_idx+12], Data_DTC_Raw[dtc_idx+13]);
        Byte_D = OBD2_Convert_Hex(Data_DTC_Raw[dtc_idx+15], Data_DTC_Raw[dtc_idx+16]);
      }
      break;
    }
    if (dtc_idx >= (OBD2_RECEIVE_BUFF_LEN - 2))return 0;
  }
  Byte_temp = (Byte_A >> 6) && 0x03;
  if(Byte_temp == 0)data_buffer_DTC[0]='P';
  if(Byte_temp == 1)data_buffer_DTC[0]='C';
  if(Byte_temp == 2)data_buffer_DTC[0]='B';
  if(Byte_temp == 3)data_buffer_DTC[0]='U';
  
  Byte_temp = (Byte_A >> 4) & 0x03;
  data_buffer_DTC[1] = 0x30 + Byte_temp;
  Byte_temp = Byte_A & 0x0F;
  data_buffer_DTC[2] = 0x30 + Byte_temp;
  Byte_temp = (Byte_B >> 4) & 0x0F;
  data_buffer_DTC[3] = 0x30 + Byte_temp;
  Byte_temp = Byte_B & 0x0F;
  data_buffer_DTC[4] = 0x30 + Byte_temp;
    
  if(twobyte)
  {
    data_buffer_DTC[5] = ' '; // Space between 2 DTC
    Byte_temp = (Byte_C >> 6) && 0x03;
    if(Byte_temp == 0)data_buffer_DTC[6]='P';
    if(Byte_temp == 1)data_buffer_DTC[6]='C';
    if(Byte_temp == 2)data_buffer_DTC[6]='B';
    if(Byte_temp == 3)data_buffer_DTC[6]='U';
    Byte_temp = (Byte_C >> 4) & 0x03;
    data_buffer_DTC[7] = 0x30 + Byte_temp;
    Byte_temp = Byte_C & 0x0F;
    data_buffer_DTC[8] = 0x30 + Byte_temp;
    Byte_temp = (Byte_D >> 4) & 0x0F;
    data_buffer_DTC[9] = 0x30 + Byte_temp;
    Byte_temp = Byte_D & 0x0F;
    data_buffer_DTC[10] = 0x30 + Byte_temp;
  }
  return 1;
}
//-------------------------------------------------------------
static void prvAutoReloadCallback_OBD(TimerHandle_t xTimer)
{
  BaseType_t qStatus;
  #if DEBUG_TIMER
  if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
        Serial.println(F("Timer 4 is running"));
        xSemaphoreGive( xSerialSemaphore );
  } 
  #endif
  while (Serial3.available()) {
    char c = (char)Serial3.read();
    if (c == '\r' || c == '\n') continue;
    if (c != '>'){
    rxData[index2++] = c;
    }
    else{
      rxData[index2] = '\0';
      qStatus = xQueueSend(OBDQueue, &rxData[0], ( TickType_t ) 100);
      if (qStatus !=  pdPASS){
        #if DEBUG
          if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
          Serial.println(F("Could not send to the OBD Queue"));
          xSemaphoreGive( xSerialSemaphore );
          };
        #endif
      };
      index2 = 0;
      memset(rxData, '\0', 70);
      xEventGroupSetBits(xEvent_Receive_Group, RECEIVE_COMPLETE_BIT_OBD);
    };
  }
}
//----------------------------------------------------------
static void Task_SetupOBD(void* pvParameters)
{
  BaseType_t qStatus;
  BaseType_t qStatus_DTC;

  StateSys DataOBD2;
  
  const EventBits_t xBitsToWaitFor  = RECEIVE_COMPLETE_BIT_OBD;
  const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS;
  
  EventBits_t xEventGroupValue;
  
  byte count_respone  = 50;
  
  char Data_Buffer_DTCs[60];
  char Rx[70]; 
  
  memset(Data_Buffer_DTCs,'\0',60);
  memset(Rx,'\0',70);
  Serial.println(F("OBDII Init.."));
  while (1)
  { 
    memset(Rx,'\0',70);
    Serial3.flush();
    switch (Cmplete_Sent)
    {
      case 0: Serial3.write("ATZ\r");
        break;
      case 1: Serial3.write("ATSP0\r");
        break;
      case 2: Serial3.write("0100\r");
        break;
      case 3: Serial3.write("ATRV\r");
        break;
      case 4: Serial3.write("0105\r");    // nhiệt độ
        break;
      case 5: Serial3.write("010C\r");    // tốc độ động cơ
        break;
      case 6: Serial3.write("010D\r");    // tốc độ xe
        break;
      case 7: Serial3.write("0101\r");    // đọc số lỗi của xe
        break;
      case 8: Serial3.write("03\r");      // đọc mã lỗi xe.
        break;
    }
    xEventGroupValue  = xEventGroupWaitBits(xEvent_Receive_Group,
                                            xBitsToWaitFor,
                                            pdTRUE,
                                            pdFALSE,
                                            xTicksToWait
                                           );
      if ((xEventGroupValue & RECEIVE_COMPLETE_BIT_OBD) != 0){
        qStatus =  xQueueReceive(OBDQueue, &Rx[0], ( TickType_t ) 100 );
        if (qStatus  == pdPASS){
           if (strstr(Rx, "ATRV") != NULL   &&    Rx[3] == 'V') DataOBD2.BatteryVoltage         = (float)atof(&Rx[4]);
           if (strstr(Rx, "0105") != NULL   &&    Rx[3] == '5') DataOBD2.EngineCoolantTemp      =   strtol(&Rx[10],0,16)- 40;
           if (strstr(Rx, "010C") != NULL   &&    Rx[3] == 'C') DataOBD2.rpm                    =   (unsigned int)((strtol(&Rx[10],0,16)*256)+strtol(&Rx[13],0,16))/4;
           if (strstr(Rx, "010D") != NULL   &&    Rx[3] == 'D') DataOBD2.spd                    =  (unsigned int) strtol(&Rx[10],0,16);
           if (strstr(Rx, "010141") == &Rx[0]){
            sum_DTCs = (byte)(strtol(&Rx[strlen(Rx)-12],0,16));
            if (sum_DTCs > 5) sum_DTCs = 5;
           };
           if (strstr(Rx, "0343 00")   == &Rx[0] || (strstr(Rx, "03")   == &Rx[0] &&strstr(Rx, "43")   == &Rx[9])){ 
              memset(Data_Buffer_DTCs,'\0',60);
              OBD2_Read_DTC(&Rx[0],(int)strlen(Rx),&Data_Buffer_DTCs[0]);  
              qStatus_DTC = xQueueSend(OBDQueue_DTC, &Data_Buffer_DTCs[0], ( TickType_t ) 5);  
              if (qStatus_DTC !=  pdPASS){
                #if 0
                if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
                   Serial.println(F("Could not send to the OBD_DTC Queue"));
                  xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
                };
                #endif   
              }        
           };
          if (Cmplete_Sent == 6) xQueueSend(OBDQueue_Data, &DataOBD2, ( TickType_t ) 100); 
          Cmplete_Sent++;
          /*Đoạn code này có mục đích đảm bảo 50 lần update data thì 1 lần đọc lỗi*/
          if(Cmplete_Sent >= 8){
            count_respone++; // Biến đếm số lần làm update: rpm,spd,volt,temp.
            Cmplete_Sent = 3;
            if (count_respone >= 50){
                Cmplete_Sent = 8;// gán biến đếm này để task tiến hành đọc lỗi.
                count_respone = 0;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
          }
          if (strstr(Rx,"STOPPED") != NULL) Cmplete_Sent = 0;
          #if 0 //DEBUG
          if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 100 ) == pdTRUE ) {
            for (int i = 0; i < strlen(Rx); i++) {
              Serial.print(Rx[i]);
            }
            Serial.println();
            Serial.print(F("Bat Voltage: "));
            Serial.println(DataOBD2.BatteryVoltage);
            Serial.print(F("RPM: "));
            Serial.println(DataOBD2.rpm);
            Serial.print(F("SPD: "));
            Serial.println(DataOBD2.spd);
            Serial.print(F("TEMP: "));
            Serial.println(DataOBD2.EngineCoolantTemp);
            Serial.print(F("Sum_DTCs: "));
            Serial.println(sum_DTCs);            
            for(int i = 0; i < strlen(Data_Buffer_DTCs);i++){
              Serial.write(Data_Buffer_DTCs[i]);
            }
            Serial.print(F("Cmplete_Sent: "));
            Serial.println(Cmplete_Sent);
            Serial.print(F("count_respone: "));
            Serial.println(count_respone);
            Serial.println();
            xSemaphoreGive( xSerialSemaphore ); 
          };
          #endif
        }
      }
  }
}
