////////////////////////
// Sleep Controls

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


void enter_deep_sleep() {
  Serial.println("Entering sleep for " + String(TIME_TO_SLEEP) + " Seconds");
  Serial.flush(); 
  esp_deep_sleep_start();
}
