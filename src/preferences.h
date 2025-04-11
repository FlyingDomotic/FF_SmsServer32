#include <arduino.h>                                                // Arduino

//  LiLyGo T-SIM70XX pinout
//
//  02: SD CARD MISO
//  04: SIM 70XX PWRKEY
//  12: LED
//  13: SD CARD CS
//  14: SD CARD CLK
//  15: SD CARD MOSI
//  25: SIM 70XX DTR
//  26: SIM 70XX TX
//  27: SIM 70XX RX
//  35: BAT ADC
//  36: SOLAR ADC

#ifdef ESP32
    uint8_t gsmTxPin = 27;                                          // ESP pin where GSM TX is connected to
    uint8_t gsmRxPin = 26;                                          // ESP pin where GSM RX is connected to
    uint8_t gsmPowerPin = 4;                                        // ESP pin where GSM power is connected to
    uint8_t batAdcPin = 35;                                         // ESP pin where LI battery is connected to 
    uint8_t relayPin = 18;                                          // ESP pin where relay is connected to
    bool reversedRelay = false;                                     // Relay is on at low state when set to true
    #define SIM7000_MODEM_SPEED 115200                              // Modem speed

    bool traceDebug = false;                                        // Trace debug messages?
    bool traceCode = false;                                         // Trace this code?
    bool traceJava = false;                                         // Trace Java code?

    // Battery
    #define BATTERY_EXISTS                                          // There's a battery connected
    #define BATTERY_LOOP_TIME 5000                                  // Scan battery every 5000 ms
    #define BATTERY_MAX_VOLTAGE 4.2                                 // Voltage when battery is fully loaded
    #define BATTERY_MIN_VOLTAGE 3.0                                 // Voltage when battery is nearly empty
    #define BATTERY_SIGNAL_PERCENT 30                               // Start signalling battery percentage at or under 30%
    #define BATTERY_SIGNAL_STEP 10                                  // Then decrease of 10%

    // LED
    #define LED_PIN 12                                              // Pin where signal LED is connected to
    #define LED_INVERTED                                            // Define it if LED is inverted (light at low level)
#endif

#ifdef ESP8266
    uint8_t gsmTxPin = D6;                                          // ESP pin where GSM TX is connected to
    uint8_t gsmRxPin = D5;                                          // ESP pin where GSM RX is connected to
    uint8_t gsmPowerPin = D1;                                       // ESP pin where GSM power is connected to
    uint8_t batAdcPin = -1;                                         // ESP pin where battery is connected to 
    uint8_t relayPin = D2;                                          // ESP pin where relay is connected to
    bool reversedRelay = false;                                     // Relay is on at low state when set to true
    #define SIM7000_MODEM_SPEED 115200                              // Modem speed

    bool traceDebug = false;                                        // Trace debug messages?
    bool traceCode = false;                                         // Trace this code?
    bool traceJava = false;                                         // Trace Java code?

    // Battery
    #define BATTERY_LOOP_TIME 5000                                  // Scan battery every 5000 ms
    #define BATTERY_MAX_VOLTAGE 4.2                                 // Voltage when battery is fully loaded
    #define BATTERY_MIN_VOLTAGE 3.0                                 // Voltage when battery is nearly empty
    #define BATTERY_SIGNAL_PERCENT 30                               // Start signalling battery percentage at or under 30%
    #define BATTERY_SIGNAL_STEP 10                                  // Then decrease of 10%

    // LED
    #define LED_PIN D4                                              // Pin where signal LED is connected to
    #define LED_INVERTED                                            // Define it if LED is inverted (light at low level)
#endif