#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEBUG_ENABLE 1

#if DEBUG_ENABLE
  #define DEBUG_PRINT(x)       Serial.print(x)
  #define DEBUG_PRINTLN(x)     Serial.println(x)
  #define DEBUG_PRINTF(...)    Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

#define LCD_ADDR          0x27
#define LCD_COLS          20
#define LCD_ROWS          4

#define DEBOUNCE_DELAY_MS 50
#define TCP_TIMEOUT_MS    3000
#define BOOT_STEP_MS      600

#endif // CONSTANTS_H