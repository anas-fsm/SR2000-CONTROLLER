#ifndef PINS_H
#define PINS_H

// --- ETH LAN8720 Pins (WT32-ETH01 Default Pins) ---
#define ETH_PHY_ADDR     1
#define ETH_PHY_POWER    16
#define ETH_PHY_MDC      23
#define ETH_PHY_MDIO     18
#define ETH_CLK_MODE     ETH_CLOCK_GPIO0_IN

// --- LCD I2C Pins ---
#define PIN_I2C_SDA      14
#define PIN_I2C_SCL      15

// --- Push Buttons Pins (INPUT_PULLUP) ---
#define PIN_BTN_UP       32
#define PIN_BTN_DOWN     33
#define PIN_BTN_OK       35
#define PIN_BTN_BACK     39

#endif // PINS_H