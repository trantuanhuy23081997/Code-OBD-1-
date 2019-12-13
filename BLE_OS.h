void Return_DataBLE(unsigned int A,float B)
{
      switch (A){
      case 101:Serial2.println((float)State_Car.BatteryVoltage);
              break;
      case 102:Serial2.println((float)State_Car.EngineCoolantTemp);
              break;
      case 103:Serial2.println(State_Car.OilPres);
              break;
      case 104:Serial2.println(State_Car.Handbrake);
              break;
      case 105:Serial2.println(State_Car.StateFan);
              break; 
      case 106:Serial2.println((float)HC01.Temp_ON);
              break;   
      case 107:Serial2.println((float)HC01.Temp_OFF);
              break;   
      case 108:Serial2.println((float)HC01.Temp_WARN);
              break;     
      case 109:Serial2.println((float)HC01.VBat_HIGH);
              break;  
      case 110:Serial2.println((float)HC01.VBat_LOW);
              break;   
      case 111:Serial2.println((float)HC01.R1ECU);
              break;   
      case 112:Serial2.println((float)HC01.Ro);
              break;    
      case 113:Serial2.println((float)HC01.Beta);
              break;    
      case 114:
              break;
      
      case 115:Serial2.println((float)HC01.VCharge);
              break; 
      case 116:Serial2.println((float)HC01.NumberOfReset);  // số lần đã reset bảo dưỡng.
              break; 
      case 117:Serial2.println((float)HC01.CycleMaintenance); //đơn vị giờ.
              break;  
      case 118:Serial2.println((float)HC01.TimeRunTooLong); // Thời gian cảnh báo quá lâu chạy đơn vị phút.
              break;  
      case 119:Serial2.println((float)Rev,4); //  Thời gian yêu cầu nghỉ ngơi trong TimeT đơn vị phút
              break;  
      case 120:Serial2.println((float)State_Car.spd); //  Thời gian yêu cầu nghỉ ngơi trong TimeT đơn vị phút
              break;
      case 121:Serial2.println((float)State_Car.rpm); //  Thời gian yêu cầu nghỉ ngơi trong TimeT đơn vị phút
              break;
      case 122:Serial2.println((float)State_Car.TimeRunning); //  Thời gian yêu cầu nghỉ ngơi trong TimeT đơn vị phút
              break;              
      case 123:Serial2.println((float)_Rev,4);    //  Thời gian yêu cầu nghỉ ngơi trong TimeT đơn vị phút
              break;
      case 124:Serial2.println((float)Radius,4);  //  Bán kính bánh xe
              break;
      case 125:Serial2.println((float)HC01.MaxCurrent); //  Dòng điện tối đa
              break;
      case 126:Serial2.println((float)HC01.odo_Meter); //  Số km xe đi được từ trước đến giờ
              break;
      case 127:Serial2.println((float)HC01.TimeRun); //  Thời gian xe chạy từ trước đến giờ
              break;
      case 128:Serial2.println((float)State_Car.Current); //  Thời gian yêu cầu nghỉ ngơi trong TimeT đơn vị phút
              break;
      case 129:
            Serial.print(' ');
            if(Buffer_Danger.TempHot)         Serial2.print("P0217"); // Lỗi quá nhiệt.
            Serial.print(' ');
            if(HC01.ErorSys.Params_ReadTemp || HC01.ErorSys.Circuit_ReadTemp)  Serial2.print("P0118"); // Lỗi  mạch cảm biến nhiệt độ
            Serial.print(' ');
            if(Buffer_Danger.OilPressureLow)  Serial2.print("P0522"); // Lỗi áp lực nhớt thấp.
            Serial.print(' ');
            if(Buffer_Danger.HighCurrent)     Serial2.print("C0101");  // Lỗi quá công suất máy phát.
            Serial.print(' ');
            if(HC01.ErorSys.NotReadCurrent)   Serial2.print("C0118"); // Lỗi mạch cảm biến dòng điện.
            Serial.print(' ');
            if(Buffer_Danger.VoltEror)        Serial2.print("P0562"); // Lỗi điện áp bình không tốt.
            Serial2.println();
              break; 
              
      case 201:HC01.Temp_ON = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(4,HC01.Temp_ON);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.Temp_ON);
              digitalWrite(BUZZER,HIGH);
              break;
      case 202:HC01.Temp_OFF = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(8,HC01.Temp_OFF);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.Temp_OFF);
              digitalWrite(BUZZER,HIGH);
              break;
      case 203:HC01.Temp_WARN = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(0,HC01.Temp_WARN);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.Temp_WARN);
              digitalWrite(BUZZER,HIGH);
              break;
      case 204: HC01.VBat_HIGH = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(16,HC01.VBat_HIGH);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.VBat_HIGH);
              digitalWrite(BUZZER,HIGH);
              break;
      case 205: HC01.VBat_LOW = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(12,HC01.VBat_LOW);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.VBat_LOW);
              digitalWrite(BUZZER,HIGH);
              break;
      case 206: HC01.R1ECU = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_2_byte(21,HC01.R1ECU);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.R1ECU);
              digitalWrite(BUZZER,HIGH);
              break; 
      case 207: HC01.Ro = (unsigned int)B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_2_byte(23,HC01.Ro);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.Ro);
              digitalWrite(BUZZER,HIGH);
              break;
      case 208:HC01.Beta = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_2_byte(25,HC01.Beta);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.Beta);
              digitalWrite(BUZZER,HIGH);
              break;
      case 209: HC01.VCharge = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(27,HC01.VCharge);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.VCharge);
              digitalWrite(BUZZER,HIGH);
              break;
      case 210: HC01.TimeRun = (unsigned long)(B);
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_4_byte(31,HC01.TimeRun);// Viết giá trị ban đầu cho nhiệt độ bật quạt.
              vTaskDelay(1);
              Serial2.println(HC01.TimeRun);
              digitalWrite(BUZZER,HIGH);
              break;  
      case 211: HC01.TripTimeRun = (unsigned long)(B);
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_4_byte(35,HC01.TripTimeRun);
              vTaskDelay(1);
              Serial2.println(HC01.TripTimeRun);
              digitalWrite(BUZZER,HIGH);
              break;  
      case 212: HC01.TimeRunTooLong = (unsigned long)B;
              digitalWrite(BUZZER,LOW);
              digitalWrite(BUZZER,HIGH);
              break; 
      case 213: HC01.CycleMaintenance = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_2_byte(40,HC01.CycleMaintenance); //HC01.CycleMaintenance đơn vị là giờ.
              HC01.PointTimeMain  = HC01.CycleMaintenance*HC01.NumberOfReset;
              HC01.PointTimeMain += HC01.CycleMaintenance;
              HC01.PointTimeMain *= 60;
              Serial2.println(HC01.CycleMaintenance);
              digitalWrite(BUZZER,HIGH); 
              break;  
      case 214: HC01.NumberOfReset++;
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_1_byte(39,HC01.NumberOfReset);
              Serial2.println(HC01.NumberOfReset);
              HC01.PointTimeMain  = HC01.CycleMaintenance*HC01.NumberOfReset;
              HC01.PointTimeMain += HC01.CycleMaintenance;
              HC01.PointTimeMain *= 60;
              digitalWrite(BUZZER,HIGH); 
              break;  
      case 215: Rev = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(46,Rev*10.0);// Viết giá trị ban đầu cho độ phân giải vòn tua máy.
              vTaskDelay(1);
              Serial2.println(Rev,4);
              digitalWrite(BUZZER,HIGH);
              break; 
      case 216: _Rev = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(50,_Rev*10.0);// Viết giá trị ban đầu cho độ phân giải vòn tua máy.
              vTaskDelay(1);
              Serial2.println(_Rev,4);
              digitalWrite(BUZZER,HIGH);
              break;       
     case 217: Radius = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(54,Radius*100.0);// Viết giá trị ban đầu cho độ phân giải vòn tua máy.
              vTaskDelay(1);
              Serial2.println(Radius,4);
              digitalWrite(BUZZER,HIGH);
              break;     
     case 218: HC01.MaxCurrent = (unsigned int)B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_2_byte(42,HC01.MaxCurrent);// Viết giá trị ban đầu cho độ phân giải vòn tua máy.
              vTaskDelay(1);
              Serial2.println(HC01.MaxCurrent,4);
              digitalWrite(BUZZER,HIGH);
              break;      
     case 219: HC01.MaxCurrent = (unsigned int)B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.write_2_byte(42,HC01.MaxCurrent);// Viết giá trị ban đầu cho độ phân giải vòn tua máy.
              vTaskDelay(1);
              Serial2.println(HC01.MaxCurrent,4);
              digitalWrite(BUZZER,HIGH);
              break;
     case 220: HC01.odo_Meter = B;
              digitalWrite(BUZZER,LOW);
              AT24CXX.eeprom_write_fl(58,HC01.odo_Meter);// Viết giá trị ban đầu cho độ phân giải vòn tua máy.
              vTaskDelay(1);
              Serial2.println(HC01.odo_Meter,4);
              digitalWrite(BUZZER,HIGH);
              break;                                                  
     }
}
static void Task_Ble (void * pvParameters)
{
  char Rx[12];
  unsigned int Var =  0;
  float Data = 0;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount ();
  BaseType_t qStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
  while(1)
  {
    #if DEBUG_TIMER
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
    Serial.println(F("Task BLE is Running"));
    xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    };
    #endif
   qStatus =  xQueueReceive(BLEQueue,&Rx[0],xTicksToWait);
   if(qStatus  == pdPASS)
   {
    Var = strtol(&Rx[0],Rx[4],10);
    if(Var > 200){
      Data = (float)atof(&Rx[5]);
    };
    #if DEBUG
    Serial.print("Data Received Successfully: ");
    Serial.print(Var);
    Serial.print('\t');
    Serial.println(Data);
    #endif
    Serial2.flush();
    for(int i = 0;i<4;i++){
      Serial2.print(Rx[i]);
    };
    Serial2.print(" ");
    Return_DataBLE(Var,Data);
   }
    vTaskDelayUntil( &xLastWakeTime, (150 / portTICK_PERIOD_MS) );
  }
}
static void prvAutoReloadCallback(TimerHandle_t xTimer)
{
  BaseType_t qStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    #if DEBUG_TIMER
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
        Serial.println(F("Timer 1 is running"));
        xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    #endif //Để đảm bảo ổn định app phải gửi dữ liệu với khoảng cách thời gian 2 lần liên tiếp tối thiểu là 150ms.
  while(Serial2.available()){
    char c = (char)Serial2.read();
    if(c != '\n'){
      RxData[index++] = c;
    }
    else{
      RxData[index] = '\0';
      qStatus =xQueueSend(BLEQueue,&RxData[0],10);
      if(qStatus !=  pdPASS){
        #if DEBUG
        if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
           Serial.println(F("Could not send  to the queue"));
          xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
        }
        #endif   
      }
      index = 0;
    };
  }
}

