static void Task_Check_Sys(void* pvParameters)
{
  vTaskResume(TaskHandle_Warn);
  while (1)
  {
    #if DEBUG_HC_OS
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
    Serial.println(F("Task Check is Running"));
    xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    };
    #endif
    xQueueReceive(OBDQueue_Data, &State_Car, ( TickType_t ) 1000 );
    
    State_Car.Current            = HC01.ReadCurrent();
    State_Car.Handbrake          = HC01.ReadHandBrake();
    HC01.StateCar.spd            = State_Car.spd;
    State_Car.StateVehicle       = HC01.GetStateVehicle(State_Car.BatteryVoltage,State_Car.rpm,State_Car.spd);
    State_Car.OilPres            = HC01.ReadOilPressure();
    State_Car.TimeRunning        = HC01.GetTimeRunning(); 
    
   if (State_Car.StateVehicle >= 2){
      xEventGroupSetBits(xEvent_Receive_Group,ENGINE_OFF_BIT);
   };
   HC01.Check_DangerState(&Buffer_Danger,&State_Car);
   vTaskDelay(50/ portTICK_PERIOD_MS); 
  }
 }
//-------------------------------------------------------------------
static void Task_Kill_Machine(void* pvParameters){
  EventBits_t xEventGroupValue;
  EventBits_t xBit_NotWarning ;
  while (1)
  {
    #if DEBUG
      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
      Serial.println(F("Task Kill Engine is Running"));
      xSemaphoreGive( xSerialSemaphore );
      }
    #endif
    xEventGroupValue = xEventGroupWaitBits(xEvent_Receive_Group,
                                    KILL_MACHINE_BIT,
                                    pdTRUE,
                                    pdFALSE,
                                    portMAX_DELAY
                                   );
    if((xEventGroupValue & KILL_MACHINE_BIT) != 0){
      xEventGroupClearBits(xEvent_Receive_Group,NOT_WARNING_BIT);
      xBit_NotWarning = xEventGroupWaitBits(xEvent_Receive_Group,
                                    NOT_WARNING_BIT,
                                    pdTRUE,
                                    pdTRUE,
                                    (TickType_t)1250
                                   );
      if((xBit_NotWarning & NOT_WARNING_BIT) == 0){
        xEventGroupClearBits(xEvent_Receive_Group,ENGINE_OFF_BIT);
        digitalWrite(IFUEL,LOW);
        xEventGroupValue = xEventGroupWaitBits(xEvent_Receive_Group,
                                    ENGINE_OFF_BIT | NOT_WARNING_BIT,
                                    pdTRUE,
                                    pdFALSE,
                                    (TickType_t)625 
                                   );
        digitalWrite(IFUEL,HIGH);
      }
    }
  }
}
//-------------------------------------------------------------------//
#if DEBUG_HC_OS
static void Task_Serial_Data(void* pvParameters)
{
  while (1)
  {
   if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 500 ) == pdTRUE ){
      Serial.println(F("Task Serial is Running"));
      Serial.print(F("RPM:"));
      Serial.print('\t');
      Serial.print(State_Car.rpm);
      Serial.print('\t');
      Serial.print(F("SPD:"));
      Serial.print('\t');
      Serial.print(State_Car.spd);
      Serial.print('\t');
      Serial.print(F("Bat Voltage:"));
      Serial.print('\t');
      Serial.println(State_Car.BatteryVoltage);
      Serial.print(F("State Vehicle:"));
      Serial.print('\t');
      Serial.println(State_Car.StateVehicle);
      Serial.print(F("Time Engine Running:"));
      Serial.print('\t');
      Serial.println(State_Car.TimeRunning);
      Serial.print(F("Odo_Meter:"));
      Serial.print('\t');
      Serial.println(HC01.odo_Meter);
      Serial.print(F("Micros:"));
      Serial.print('\t');
      Serial.println(micros());
      Serial.print(F("Temp warning:"));
      Serial.print('\t');
      Serial.println(HC01.Temp_WARN);
      Serial.print(F("Max Current:"));
      Serial.print('\t');
      Serial.println(HC01.MaxCurrent);
      Serial.print(F("EROR:"));
      Serial.print('\t');
      Serial.print(Buffer_Danger.TempHot);
      Serial.print('\t');
      Serial.print(Buffer_Danger.OilPressureLow);      
      Serial.print('\t');
      Serial.print(Buffer_Danger.HighCurrent);
      Serial.print('\t');
      Serial.print(Buffer_Danger.HandBrakeON);
      Serial.print('\t');
      Serial.println(Buffer_Danger.VoltEror);      
      Serial.print(F("Heap Memory Free:"));
      Serial.print('\t');
      Serial.println(xPortGetFreeHeapSize());
      Serial.print(F("F_CPU:"));
      Serial.print('\t');
      Serial.println((uint32_t)F_CPU);
      Serial.print(F("PortTICK_PERIOD_MS:"));
      Serial.print('\t');
      Serial.println(portTICK_PERIOD_MS);
      Serial.print(F("configTICK_RATE_HZ:"));
      Serial.print('\t');
      Serial.println(configTICK_RATE_HZ);
      Serial.print(F("Tick Count of System:"));
      Serial.print('\t');
      Serial.println((uint32_t)xTaskGetTickCount());
      Serial.print(F("High water mark of OBDTask:"));
      Serial.print('\t');
      Serial.println(uxTaskGetStackHighWaterMark(TaskHandle_OBD));
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
   vTaskDelay(3000 / portTICK_PERIOD_MS); 
  };
}
#endif
//-------------------------------------------------------------------
static void Task_Warning(void* pvParameters){
  while (1)
  {
    #if DEBUG_HC
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
    Serial.println(F("Task Warning is Running"));
    xSemaphoreGive( xSerialSemaphore );
    }
    #endif
   if(Buffer_Danger.TempHot || Buffer_Danger.OilPressureLow){// || Buffer_Danger.HighCurrent){
    digitalWrite(BUZZER,!digitalRead(BUZZER));
    xEventGroupSetBits(xEvent_Receive_Group,KILL_MACHINE_BIT);
    vTaskDelay(100 / portTICK_PERIOD_MS);
   }
   else if (Buffer_Danger.HandBrakeON){
    digitalWrite(BUZZER,!digitalRead(BUZZER));
    xEventGroupSetBits(xEvent_Receive_Group,KILL_MACHINE_BIT);
    vTaskDelay(200 / portTICK_PERIOD_MS);
   }
   else if (Buffer_Danger.VoltEror){
    digitalWrite(BUZZER,!digitalRead(BUZZER));
    vTaskDelay(300 / portTICK_PERIOD_MS);
   }      
   else{
    digitalWrite(BUZZER,LOW);
    xEventGroupSetBits(xEvent_Receive_Group,NOT_WARNING_BIT);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
  }
}
//------------------------------------------------------------------------
static void Task_Caculator_Time(void* pvParameters)
{ 
  TickType_t xLastWakeTime;
  while (1)
  {
    #if DEBUG_HC_OS
      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 100 ) == pdTRUE ){
        Serial.println(F("Task Caculator Time Engine Running"));
        xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
      }
    #endif
    HC01.BeginC_TimeRun(&State_Car);
    HC01.EndC_TimeRun(&State_Car);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//------------------------------------------------------------------------------

/*
static void Task_CLD(void* pvParameters)
{
  pinMode(LDO,OUTPUT);
  digitalWrite(LDO,HIGH);
  while (1)
  {
      #if DEBUG_HC_OS
      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 100 ) == pdTRUE ){
      Serial.println(F("Task Check Lock Door"));
      xSemaphoreGive( xSerialSemaphore );
      }
      #endif
     if (State_Car.spd > 10 && FLAGDOR == 0){
      digitalWrite(LDO,LOW);
      FLAGDOR = 1;
     }
     if (State_Car.spd < 10){
      FLAGDOR = 0;
      digitalWrite(LDO,HIGH);
     }
  vTaskDelay(500 / portTICK_PERIOD_MS); 
  };
}
*/
static void Task_Odometers (void * pvParameters)
{ 
  while(1)
  {
    HC01.BeginC_MDistance(&State_Car);
    HC01.EndC_MDistance(&State_Car);
    vTaskDelay(1400/portTICK_PERIOD_MS);
  }
}
/*
static void Task_Sleep (void * pvParameters)
{
  while(1)
  {
    #if DEBUG
      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
      Serial.println(F("Task sleep is running"));
      Serial.print(F("FLAG_POWER_DOWN: "));
      Serial.println(FLAG_POWER_DOWN);
      xSemaphoreGive( xSerialSemaphore );
      }
    #endif
  if (HC01.ReadBatVolt() <= 5.0 && FLAG_POWER_DOWN  != 1){
    
    vTaskDelay(( TickType_t)625);
    
    vTaskSuspend(TaskHandle_OBD); 
    vTaskSuspend(TaskHandle_Ble);
    vTaskSuspend(TaskHandle_Warn);
    
    #if DEBUG_HC_OS
      vTaskSuspend(TaskHandle_3);
    #endif
    
    vTaskSuspend(TaskHandle_Kill_Machine);
    vTaskSuspend(TaskHandle_Door);
    vTaskSuspend(TaskHandle_HC);
    vTaskSuspend(TaskHandle_Local);
    xTimerStop(xTimer2,(TickType_t)100);

    digitalWrite(34,LOW);
    digitalWrite(35,LOW);
    
    FLAG_POWER_DOWN  = 1;
  }
  if (HC01.ReadBatVolt() > 5.0 && FLAG_POWER_DOWN  != 0) {
    
    digitalWrite(34,HIGH);
    digitalWrite(35,HIGH);
    Cmplete_Sent = 0;// reset OBD2 task.
    vTaskDelay(( TickType_t)50 ); // Wait for power loaded

    xTaskCreate(Task_SetupA9G, "Task Setup A9G", 200, NULL, 2,  &TaskHandle_A9G);
    vTaskResume(TaskHandle_OBD);
    vTaskResume(TaskHandle_Ble);
    vTaskResume(TaskHandle_Warn);
    
    #if DEBUG_HC_OS
    vTaskResume(TaskHandle_3);
    #endif
    
    vTaskResume(TaskHandle_Kill_Machine);
    vTaskResume(TaskHandle_Door); 
    vTaskResume(TaskHandle_HC);
    vTaskResume(TaskHandle_Local); 
    xTimer2Started =  xTimerStart(xTimer2,(TickType_t)100);

    FLAG_POWER_DOWN = 0;
  };
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
*/
