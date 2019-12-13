//--------------------------------------------------------------
#include <ArduinoJson.h>
static void Task_SetupA9G(void* pvParameters);
static void prvAutoReloadCallback3(TimerHandle_t xTimer)
{
  BaseType_t qStatus;
  #if DEBUG_TIMER
  if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
        Serial.println(F("Timer 3 is running"));
        xSemaphoreGive( xSerialSemaphore );
  } 
  #endif
  memset(RxDataA9G, '\0', 70);
  while (Serial1.available()) {
    char c = (char)Serial1.read();
    RxDataA9G[index_receive_a9g++] = c;
    if (Serial1.available() == 0) {
      RxDataA9G[index_receive_a9g] = '\0';
      qStatus = xQueueSend(A9GQueue, &RxDataA9G[0], ( TickType_t ) 5);
      if (qStatus !=  pdPASS){
        #if DEBUG
        if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
           Serial.println(F("Could not send to the A9G Queue"));
          xSemaphoreGive( xSerialSemaphore );
        };
        #endif  
      };
      index_receive_a9g = 0;
    };
  }
}
//--------------------------------------------------------------------//
static void Task_SendTCP(void* pvParameters)
{
  #if DEBUG 
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
      Serial.println("Task SendTCP running and Suspend Task Handle Local"); // Đoạn code thêm vào để kiếm tra
      xSemaphoreGive( xSerialSemaphore ); 
    };
  #endif 
  vTaskSuspend(TaskHandle_Local); // Đang kiểm tra
  state = Idle;
  while (1)
  {
    switch (state)
    {
      case Idle :
        Serial1.flush();
        Serial1.println(F("AT+CIPSTART=\"TCP\",\"159.65.5.133\",1334"));
        state = SendedTCPStart;
        break;
      case SendedTCPStart :
        Serial1.flush();
        Serial1.println(F("AT+CIPSEND"));
        state = SendedTCPsend; 
        break;
      case SendedTCPsend :
        Serial1.flush();
        Serial1.println(Jsonstring);
        state = SendedData;
        break;
      case SendedData :
        Serial1.flush();
        Serial1.write(0x1A);
        state = DoNothing;
        break;
    }
    Serial1.flush();  // Đợi quá trình gửi thông tin lên Serial1 kết thúc
    #if DEBUG
      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 10 ) == pdTRUE ) {
            Serial.print(F("Send TCP is running!:"));
            Serial.println(state);
            xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
      }
    #endif
    if(state == DoNothing){
      #if DEBUG 
        if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
        Serial.println("Resume Task Handle Local and Delete Task SendTCP"); // Đoạn code thêm vào để kiểm tra
        xSemaphoreGive( xSerialSemaphore ); 
        };
      #endif 
      vTaskResume(TaskHandle_Local);  // Đang kiểm tra
      vTaskDelete(TaskHandle_SendWeb); 
    }
  }
}
//--------------------------------------------------------------------//
static void TaskWrapJS(void* pvParameters)
{
  #if DEBUG 
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
      Serial.println("Task WrapJS running and Suspend Task Handle Local");  // Đoạn code thêm vào để kiểm tra
      xSemaphoreGive( xSerialSemaphore ); 
    };
  #endif 
  vTaskSuspend(TaskHandle_Local); 
  BaseType_t qStatus;
  char * DTC              = (char*) pvPortMalloc(6);
  
  memset(Jsonstring, '\0', 300);
  memset(DTC,'\0',6);
  /*memset(Data_Buffer_DTCsJson,'\0',60);*/
  
  StaticJsonBuffer<400> jsonBuffer;               // tạo một vùng nhớ chứa chuỗi json
  JsonObject& root = jsonBuffer.createObject();   // tạo một một đối tượng json là root (hoặc bất cứ tên nào khác)
  root["id"] = "51A-13343";                       // gán value là "51A-25882" cho key "id"
  JsonArray& data = root.createNestedArray("GPS"); 
  /* 
   *  tạo 1 key chứa dùng chứa mảng có tên là "GPS" trong root, 
   *  đồng thời gán nó cho một mảng json là data
  */
    
  data.add(DataGPSLat);  // thêm 2 tọa độ vào Data
  data.add(DataGPSLon);

  root["dtc_numb"] = sum_DTCs;
  JsonArray& dtc_array = root.createNestedArray("DTCs"); 
  if(Buffer_Danger.TempHot)         dtc_array.add("P0217"); // Lỗi quá nhiệt.  
  /*
  if(Buffer_Danger.OilPressureLow)  dtc_array.add("P0522"); // Lỗi áp lực nhớt thấp.
  if(Buffer_Danger.HighCurrent)     dtc_array.add("C0101");  // Lỗi quá công suất máy phát.
  if(HC01.ErorSys.NotReadCurrent)   dtc_array.add("C0118"); // Lỗi mạch cảm biến dòng điện. 
  */
  if(Buffer_Danger.VoltEror)        dtc_array.add("P0562"); // Lỗi điện áp bình không tốt. 

  qStatus = xQueueReceive(OBDQueue_DTC, &Data_Buffer_DTCsJson[0], ( TickType_t ) 5 );
  if(sum_DTCs > 0){
    for(int index_dtc = 0; index_dtc < (int)strlen(Data_Buffer_DTCsJson);index_dtc++){
      memset(DTC,'\0',6);
      if (Data_Buffer_DTCsJson[index_dtc] == ' ') continue;
      strncpy(DTC, (char*)(Data_Buffer_DTCsJson + index_dtc), 5);
      dtc_array.add(DTC);
      index_dtc = index_dtc + 5;
    }
  }
  root["a"] = State_Car.rpm;  
  root["b"] = 0;  
  root["c"] = State_Car.spd; 
  root["d"] = 0;
  State_Car.EngineCoolantTemp   == -40 ? root["d"] = "0" : root["d"] = State_Car.EngineCoolantTemp;
  root["e"] = State_Car.BatteryVoltage;
  root["f"] = State_Car.Current;
  root["g"] = 0;   
  root["h"] = 0; 
  State_Car.OilPres   == false ? root["i"] = "ON" : root["i"] = "OFF";
  State_Car.Handbrake == false ? root["j"] = "ON" : root["j"] = "OFF";
  root["k"] = State_Car.TimeRunning; 
  root["l"] = HC01.TimeRun; // Thời gian động cơ hoạt động từ trước đến giờ (đơn vị phút);
  root["m"] = 0;
  root["n"] = 0;
  root["o"] = 0;    
  root["p"] = 0; 
  root["r"] = 0; 
  root["s"] = 0; 
  root["t"] = HC01.odo_Meter; // Số km trên odo.
  root["u"] = 0; 
  root["v"] = 0;   
  root.printTo(Jsonstring); // lưu chuối Json vừa tạo vào chuỗi Jsonstring
  
  vPortFree(DTC);
  //vPortFree(Data_Buffer_DTCs);
  
  #if 1
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
      Serial.flush();
      Serial.print(F("Jsonstring: "));
      for(int i = 0; i <= strlen(Jsonstring); i++){
        Serial.print(Jsonstring[i]);
      }
      Serial.println();
      Serial.println("Resume lai Task Handle Local and Delete Task WrapJS"); //  Đoạn code thêm vào để kiểm tra
      xSemaphoreGive( xSerialSemaphore );
    }
  #endif
  vTaskResume(TaskHandle_Local);
  vTaskDelete(TaskHandle_JS);    
}
//---------------------------------------------------------------------//
static void Task_Handle_Local(void* pvParameters)
{
  BaseType_t qStatus;
  int Time_repeat = 0;
  while (1)
  {
   char * Data =(char*) pvPortMalloc(70);
   memset(Data, '\0', 70);
   Time_repeat = 0;
   RESEND:
   Time_repeat++;
   #if DEBUG 
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
      Serial.println("Task Handle Local running");  //  Đoạn code thêm vào để kiểm tra
      Serial.print(F("Task Handle Position:"));
      Serial.println(Time_repeat);
      xSemaphoreGive( xSerialSemaphore ); 
    };
   #endif   
   Serial1.flush();
   Serial1.println(F("AT+LOCATION=2"));
   qStatus =  xQueueReceive(A9GQueue, &Data[0], ( TickType_t ) 625 );
   if (qStatus  == pdPASS){
      if (strstr(Data, "AT+LOCATION=2") != NULL && strstr(Data,"OK") != NULL && Data[12] == '2'){
        for (int i = 0; i < strlen(Data); i++){
              /*  Serial.print(Data[i]);  */
          if(i >= 17 && i <= 25) DataGPSLat[i-17] = Data[i];
          if(i >= 27 && i <= 36) DataGPSLon[i-27] = Data[i];
        };
            //strcpy(_DataGPSLat,DataGPSLat);
            //strcpy(_DataGPSLon,DataGPSLon);
        Time_repeat = 0;
      }   
      else{
        if (Time_repeat <= 10) goto RESEND;
        Time_repeat = 0;
              //strcpy(DataGPSLat,_DataGPSLat);
              //strcpy(DataGPSLon,_DataGPSLon);
      }
   }
   else{
      //strcpy(DataGPSLat,_DataGPSLat);
      //strcpy(DataGPSLon,_DataGPSLon);
      if (Time_repeat <= 10) goto RESEND;
      Time_repeat = 0;        
    }
    vPortFree(Data);
    #if DEBUG
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 50 ) == pdTRUE ) {
      Serial.println("Da nhan duoc Data tu A9GQueue!");  //  Đoạn code thêm vào để kiểm tra
      Serial.print("Data from A9GQueue: "); Serial.println(Data); // Đoạn code thêm vào để kiểm tra
      Serial.print(F("Lat: "));
      for (int i = 0; i < strlen(DataGPSLat); i++){
        Serial.print(DataGPSLat[i]);
      };
      Serial.println();
      Serial.print(F("Lon: "));
      for (int i = 0; i < strlen(DataGPSLon); i++){
        Serial.print(DataGPSLon[i]);
      };
      Serial.println();
      Serial.print(F("HeapSize:")); 
      Serial.println(xPortGetFreeHeapSize());   
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    #endif
    while((unsigned int)xPortGetFreeHeapSize() < 800){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    xTaskCreate(TaskWrapJS, "Task WrapJS", 600, NULL, 2,  &TaskHandle_JS);  // tạm tắt Task_WrapJS
    while((unsigned int)xPortGetFreeHeapSize() < 600){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    xTaskCreate(Task_SendTCP, "Tasksendweb", 400, NULL, 2,  &TaskHandle_SendWeb); // tạm tắt Task_SendTCP
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  };
}
//---------------------------------------------------------------------//
static void Task_SetupA9G(void* pvParameters)
{
  BaseType_t qStatus;
  
  byte String_Complete = 0;
  byte Count_Resend_One_Data  = 0;
  byte Count_Wait = 0;
  
  char * Rx = (char*) pvPortMalloc(70);
//  vTaskSuspend(TaskHandle_Sleep);
  vTaskSuspend(TaskHandle_Local);
//  vTaskSuspend(TaskHandle_Warn);
//  vTaskSuspend(TaskHandle_HC);
//  vTaskSuspend(TaskHandle_Ble);
//  vTaskSuspend(TaskHandle_OBD);
  #if DEBUG_HC_OS
  vTaskSuspend(TaskHandle_3);
  #endif
//  vTaskSuspend(TaskHandle_CTime);
//  vTaskSuspend(TaskHandle_Door);
  Serial.println(F("A9G Initial..."));
  while (1)
  {
Resend_Data:
    Serial1.flush();
    switch (String_Complete)
    {
      case 0: Serial1.println(F("AT"));
        Count_Resend_One_Data++;
        break;
      case 1: Serial1.println(F("AT+GPS=0"));
        Count_Resend_One_Data++;
        break;
      case 2: Serial1.println(F("AT+CREG=1"));
        Count_Resend_One_Data++;
        break;
      case 3: Serial1.println(F("AT+CGATT=1"));
        Count_Resend_One_Data++;
        break;
      case 4: Serial1.println(F("AT+CGDCONT=1,\"IP\",\"V-INTERNET\""));
      //case 4: Serial1.println(F("AT+CGDCONT=1,\"IP\",\" m3-world\""));
        Count_Resend_One_Data++;
        break;
      case 5: Serial1.println(F("AT+CGACT=1,1"));
        Count_Resend_One_Data++;
        break;
      case 6: Serial1.println(F("AT+GPS?"));
        Count_Resend_One_Data++;
        break;
      case 7: Serial1.println(F("AT+GPS=1"));
        Count_Resend_One_Data++;
        break;
    }
Continue_ReData:
    Count_Wait++;// đếm số lần chờ.
    if ((String_Complete  >= 8) && (Count_Resend_One_Data <= 20)) {
      #if DEBUG
        if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
          Serial.flush();
          Serial.println(F("Successed !"));
          Serial.println("Resume cac Tasks and Delete Task SetupA9G!"); // Đoạn code thêm vào để kiểm tra
          xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
        }
      #endif
//      vTaskResume(TaskHandle_Sleep);
      vTaskResume(TaskHandle_Local);
//      vTaskResume(TaskHandle_HC);
//      vTaskResume(TaskHandle_Ble);
      #if DEBUG_HC_OS
      vTaskResume(TaskHandle_3);
      #endif
//      vTaskResume(TaskHandle_CTime);
//      vTaskResume(TaskHandle_Door);
//      vTaskResume(TaskHandle_OBD);
      vPortFree(Rx);
      vTaskDelete(TaskHandle_A9G);
    }
    else if ((String_Complete  < 8) && (Count_Resend_One_Data > 50)) {
      // Reset and Waiting a respone of A9G.
      digitalWrite(10, LOW);
      digitalWrite(6, LOW);
      vTaskDelay(pdMS_TO_TICKS(200));
      digitalWrite(10, HIGH);
      digitalWrite(6, HIGH);
      vTaskDelay(pdMS_TO_TICKS(1000));
      //Reset variable count;
      String_Complete = 0;
      Count_Resend_One_Data  = 0;
      #if DEBUG
        if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
          Serial.println("Reset lai chuong trinh SetupA9G!"); // Đoạn code thêm vào để kiểm tra
          xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
        }
      #endif
    }
    else {
      qStatus =  xQueueReceive(A9GQueue, &Rx[0], ( TickType_t ) 625 );
      if (qStatus  == pdPASS) {
        switch (String_Complete)
        {
            case 0:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
              break;
            case 1:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT+GPS=0") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
              break;
            case 2:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT+CREG=1") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
              break;
            case 3:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT+CGATT=1") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
              break;
            case 4:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT+CGDCONT=1,\"IP\",\"V-INTERNET\"") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
              break;
            case 5:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT+CGACT=1,1") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
              break;
            case 6:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT+GPS?") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
              break;
            case 7:if ((strstr(Rx, "OK") != NULL)&&(strstr(Rx,"AT+GPS=1") != NULL)) {
                        String_Complete++;
                        Count_Resend_One_Data = 0;
                        Count_Wait    = 0;
                    }
                break;
        } 
        #if DEBUG
          if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
            Serial.print("Truong hop yeu cau: "); Serial.println(String_Complete);  // Đoạn code thêm vào kiểm tra biến đếm String_Complete
            for (int i = 0; i < strlen(Rx); i++){
              Serial.print(Rx[i]);
            }
            Serial.println(F("Read Succesful!"));
            /* Test cac bien dem yeu cau va phan hoi */
            Serial.print("So lan check data gui yeu cau:  "); Serial.println(Count_Resend_One_Data);
            Serial.print("So lan check data phan hoi: "); Serial.println(Count_Wait);
            xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
          };
        #endif
        vPortFree(Rx);
		  }
      else {
        /* Test cac bien dem yeu cau va phan hoi */
        #if DEBUG
          if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
            Serial.println(F("No Read Data From Queue!"));
            Serial.print("So lan check data gui yeu cau:  "); Serial.println(Count_Resend_One_Data);
            Serial.print("So lan check data phan hoi: "); Serial.println(Count_Wait);  
            xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
          };
        #endif
        if (Count_Wait > 5) {
          goto Resend_Data;
        }
        else {
          goto Continue_ReData;
        }
		  }
    }
  }
}
