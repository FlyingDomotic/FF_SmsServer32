#define VERSION "25.4.12-1"

/*
 *     English: SMS Web server based on ESP32/SIM7xxx with optional relay
 *     Français : Serveur SMS Web à base d'ESP32/SIM7xxx avec relai optionel
 *
 *  What's that?
 *      This module implements an ESP32 based Web server connected to a SIM7xxx GSM module allowing sending and receiving SMS.
 *          When an optional relay in installed, it can be locally (HTTP and API) and remotely (through SMS commands) switched.
 *          With a battery, it can also detect and signal power failure and restoration by SMS.
 *          It can also be connected to MQTT and/or Serial in order to read SMS to be sent and write those received.
 *          Setup is done using an embeded web server, either through a local access point or connected to an existing one.
 *          Log can be requested by API, and/or sent to a Syslog server.
 *          Time can be provided either through GSM network or NTP server.
 *          A Python example is provided, to receive system commands through SMS and send back command's log by SMS or mail.
 *          A Domoticz plug-in is also provided at https://github.com/FlyingDomotic/domoticz-ff_smsserver-plugin
 *
 *  A quoi ça sert?
 *      Ce code réalise un serveur Web basé sur un ESP32 connecté à modem GSM de type SIM7xxx pour émettre et recevoir des SMS.
 *          Lorsqu'un relai optionnel est installé, il peut être commandé localement (HTTP et API) et à distance (par SMS)
 *          Si une batterie est connectée, il peut aussi détecter et signaler par SMS une coupure et un retour du secteur.
 *          Il peut également être connecté à un serveur MQTT et/ou un lien série pour lire les SMS à envoyer et écrire ceux reçus.
 *          Le paramétrage est réalisé grâce à un serveur web embarqué, au travers d'un point d'accès local ou d'un WiFi existant.
 *          Les traces peuvent être récupérées par une API, et/ou envoyées à un serveur Syslog.
 *          Date et heure peuvent être fournis par le réseau GSM ou par un serveur NTP.
 *          Un exemple en Python permet de recevoir des commandes système par SMS et de renvoyer les logs par SMS ou mail?
 *          Un plug-in Domoticz est disponible à https://github.com/FlyingDomotic/domoticz-ff_smsserver-plugin
 *
 *  Typical use cases
 *      Control relay (to activate heating) in your secondary house without any connection means other than SMS
 *      Add SMS functions to you automation and Linux systems
 *      Both: Send/receive SMS from/to your automation and Linux system, allowing to commad relay/opto-coupler
 *          to phyically interact with your devices, like reset or power recycling)
 * 
 *  Utilisations typiques:
 *      Contrôle un relai dans une  maison de campagne (pour activer le chauffage par exemple), sans autre connecxion que SMS
 *      Ajout de fonctions SMS à un système domotique et des machines Linux
 *      Les deux : recevoir/envoyer des messages à une domotique et et des systèmes Linux, tout en commandant des relais ou
 *          opto-coupleurs pour interégir avec vos équipements, comme les redémarrer ou recycler leur alimentation.
 * 
 *  Hardware
 *      ESP32 with SIM7XXX series GSM board (either LilyGo T SIM7070 or individual components)
 *      Optional relay
 *      Optional battery
 *
 *  Matériel
 *      ESP32 avec carte GSM de la série SIM7XXX (LilyGo T SIM7070 ou composants indépendants)
 *      Relai optionnel
 *      Batterie optionnelle
 *
 *  Connections
 *      By default, the following pins are defined in preference.h file:
 *
 *  | Name / Nom     | ESP32 Pins |
 *  | -------------- | ---------- |
 *  | Modem DTR      | 25         |     (not used)
 *  | Modem Rx       | 26         |
 *  | Modem Tx       | 27         |
 *  | Modem PowerKey | 4          |
 *  | BAT ADC        | 35         |
 *  | -------------- | ---------- |
 *  | SOLAR ADC      | 36         |     (not used)
 *  | SD MISO        | 2          |     (not used)
 *  | SD MOSI        | 15         |     (not used)
 *  | SD CLK         | 14         |     (not used)
 *  | SD CS          | 13         |     (not used)
 *  | LED            | 12         |
 *
 *  | Name / Nom     | SIM70xx Pins           |
 *  | -------------- | -----------------------|
 *  | GPS (on modem) | SIM7070(GPIO4)         |     (not used)
 *  |                | SIM7070(GPIO5)         |     (not used)
 *  |                | Physical pin number 48 |     (not used)
 *
 *  Connexions
 *      Par défaut, les pinoches suivantes sont définies dans le fichier preference.h :
 *      (Voir ci-dessus)
 *
 *  Note:
 *      ESP32 may reboot between USB power loss and before battery gets back. In this case, solder a 100uF
 *          capacitor between GND and 3.3V rail (your configuration may need a larger capacitor).
 *
 *      L'ESP32 peut redémarrer entre la disparition de l'alimentation USB et le retour de la batterie. Dans ce
 *          cas, souder un condensateur de 100 uF entre la masse et le 3.3V (votre configuration peut necéssiter
 *          un condensateur plus gros).
 *
 *  Remark:
 *      Code incorporate references to ESP8266. As of now, only basic functions have been tested. In particular, no
 *          tests have been done with SIM7xxx modem, as I did only have soldered with an ESP32. Be anyway aware that
 *          memory is limited on ESP8266, and OOM messages are likely to occur. However, feedbacks and change
 *          requests are welcome!
 *
 *  Remarque :
 *      Le code contient des références à l'ESP8266. A ce jour, seules les fonctions le plus basiques ont été testées.
 *          En particulier, aucun test n'a été réalisé avec un modem SIM7xxx, le seul que je possède est soudé sur une
 *          carte avec un ESP32. Pas de faux espoirs, la mémoire est limitée sur un ESP8266, et les messages "plus de
 *          mémoire disponible" très probables. Cependant, les retours et les demandes de modifications sont bienvenus !
 *          
 *  Available URL
 *      /           Root page
 *      /status     Returns status in JSON format
 *      /setup      Display setup page
 *      /command    Supports the following commands
 *          /turnOn     Active le relais
 *          /turnOff    désactive le relais
 *      /languages  Return list of supported languages
 *      /settings   Returns settings in JSON format
 *      /debug      Display internal variables to debug
 *      /log        Return saved log
 *      /edit       Manage and edit file system
 *      /changed    Change a variable value (internal use only)
 *      /rest       Execute API commands
 *          /params&number=123456&message=This+is+a+test+message
 *          /send&number=123456&message=This+is+a+test+message
 *                      Sends a "This is a test message" SMS to phone number 123456
 *          /listening
 *                      Send list of listening nodes/applications
 *          /restart
 *                      Restart ESP
 * 
 *  URL disponibles
 *      /           Page d'accueil
 *      /status     Retourne l'état sous forme JSON
 *      /setup      Affiche la page de configuration
 *      /command    Supporte les commandes suivantes :
 *          /turnOn     Active le relais
 *          /turnOff    désactive le relais
 *      /languages  Returne la liste des langues supportées
 *      /settings   Retourne la configuration au format JSON
 *      /debug      Affiche les variables internes pour déverminer
 *      /log        Retourne le log mémorisé
 *      /edit       Gère et édite le système de fichier
 *      /changed    Change la valeur d'une variable (utilisation interne)
 *      /rest       Execute une commande de type API
 *          /params&number=123456&message=Ceci+est+un+message+de+test
 *          /send&number=123456&message=Ceci+est+un+message+de+test
 *                      Envoie un SMS "Ceci est un message de test" au numéro de téléphone 123456
 *          /listening
 *                      Retourne la liste des noeuds/applications à l'écoute
 *          /restart
 *                      Redémarre l'ESP
 *
 *  Flying Domotic - Novembre 2024
 *
 *  GNU GENERAL PUBLIC LICENSE - Version 3, 29 June 2007
 *
 */

#ifdef ESP32
    #include <WiFi.h>                                               // WiFi
    #include <getChipId.h>                                          // ESP.getChipId emulation
    #if CONFIG_IDF_TARGET_ESP32
        #include "esp32/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32S2
        #include "esp32s2/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32C2
        #include "esp32c2/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32C3
        #include "esp32c3/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32S3
        #include "esp32s3/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32C6
        #include "esp32c6/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32H2
        #include "esp32h2/rom/rtc.h"
    #else
        #error Target CONFIG_IDF_TARGET is not supported
    #endif
    #include <Time.h>
    #include <esp_adc_cal.h>                                        // ADC calibration
#else
    #include <ESP8266WiFi.h>                                        // WiFi
    #include <Esp.h>
    extern "C" {
        #include <user_interface.h>
    }
    #include <TZ.h>					                    			// Time zones
    #include <NtpClientLib.h>                                       // https://github.com/gmag11/NtpClient
#endif

#include <ESPAsyncWebServer.h>                                      // Asynchronous web server
#include <ArduinoOTA.h>                                             // OTA (network update)
#include <arduino.h>                                                // Arduino
#include <preferences.h>                                            // Turn table preferences
#include <LittleFS.h>                                               // Flash dile system
#include <littleFsEditor.h>                                         // LittleFS file system editor
#include <ArduinoJson.h>                                            // JSON documents
#include <FF_Trace.h>                                               // Trace module https://github.com/FlyingDomotic/FF_Trace
#include <FF_Sim7000.h>                                             // SIM70XX library https://github.com/FlyingDomotic/FF_Sim7000
#include <AsyncMqttClient.h>                                        // MQTT asynchronous client https://github.com/marvinroger/async-mqtt-client
#ifdef FF_TRACE_USE_SYSLOG
    #include <Syslog.h>                                             // Syslog client https://github.com/arcao/Syslog
    #include <WiFiUdp.h>
#endif

//  ---- Macros ----
#undef __FILENAME__                                                 // Deactivate standard macro, only supporting "/" as separator
#define __FILENAME__ (strrchr(__FILE__, '\\')? strrchr(__FILE__, '\\') + 1 : (strrchr(__FILE__, '/')? strrchr(__FILE__, '/') + 1 : __FILE__))

//          -------------------
//          ---- Variables ----
//          -------------------

//  ---- Log ----
#ifndef LOG_MAX_LINES
    #ifdef ESP32
        #define LOG_MAX_LINES 45
    #endif
    #ifdef ESP8266
        #define LOG_MAX_LINES 5
    #endif
#endif
String savedLogLines[LOG_MAX_LINES];                                // Buffer to save last log lines
uint16_t savedLogNextSlot = 0;                                      // Address of next slot to be written
uint16_t logRequestNextLog = 0;                                     // Address of next slot to be send for a /log request

//  ---- Syslog ----
#ifdef FF_TRACE_USE_SYSLOG
    WiFiUDP udpClient;
    Syslog syslog(udpClient, SYSLOG_PROTO_IETF);
#endif

//  ---- Asynchronous web server ----
AsyncWebServer webServer(80);                                       // Web server on port 80
AsyncEventSource events("/events");                                 // Event root

//  ---- Preferences----
#define SETTINGS_FILE "/settings.json"

String ssid;                                                        // SSID of local network
String pwd;                                                         // Password of local network
String espName = "SmsRelay";                                        // Name of this module
String serverLanguage;                                              // This server language
String deviceName;                                                  // Managed device name
String onCommand;                                                   // Comma separated list of allowed "on" commands
String offCommand;                                                  // Comma separated list of allowed "off" commands
String stateCommand;                                                // Comma separated list of allowed "state" commands
String onButton;                                                    // Label inside "on" button
String offButton;                                                   // Label inside "off" button
String onState;                                                     // Value to display when device is on
String offState;                                                    // Value to display when device is off
String startState;                                                  // State to give to device at startup
String allowedNumbers;                                              // Comma separated list of allowed phone numbers
String syslogServer;                                                // Syslog server name or IP (can be empty)
uint16_t syslogPort;                                                // Syslog port (default to 514)
String ntpServer;                                                   // NTP server nale or IP (can be empty)
String ntpParameters;                                               // NTP parameters
String mqttServer;                                                  // MQTT server name or IP (can be empty)
uint16_t mqttPort;                                                  // MQTT port (default to 1883)
String mqttUser;                                                    // MQTT username
String mqttPasw;                                                    // Mqtt password
String mqttSendTopic = "";                                          // Topic to send received SMS to
String mqttReceivedTopic = "";                                      // Topic to listen for SMS to send
String mqttCommandTopic = "";                                       // Topic to listen for commands to execute
String mqttLwtTopic = "";                                           // Last Will topic

//  ---- Local to this program ----
String resetCause = "";                                             // Used to save reset cause
bool startupDone = false;                                           // Startup is done
bool localDebugFlag = false;                                        // This code debug flag
bool localTraceFlag = false;                                        // This code trace flag
bool localEnterFlag = false;                                        // This code local enter routine flag
bool sendAnUpdateFlag = false;                                      // should we send an update?
String wifiState = "";                                              // Wifi connection state

//  ---- Serial commands ----
#ifdef SERIAL_COMMANDS
    char serialCommand[250];                                        // Buffer to save serial commands
    size_t serialCommandLen = 0;                                    // Buffer used lenght
#endif

// ---- Relay ----
bool relayActive;                                                   // True is relay is active
#define RELAY_STATE_FILE "/relayState.json"                         // Contains last relay state

//  ---- Battery ----
#ifdef BATTERY_EXISTS
    unsigned long lastBatteryLoopTime = 0;                          // Last battery scan time
    unsigned long lastPowerLossTime = 0;                            // Last Power loss time
    uint8_t nextPercentMessage = 0;                                 // Signal again when battery is lower than this
    uint16_t powerDebounceCount = 0;                                // Count of current state
    bool powerDebounceState = false;                                // Last state of debounce value
    bool powerPresent = true;                                       // We're connecting to power
    float batteryVoltage = 0;                                       // Last valid battery voltage
    int vref = 1100;                                                // ADC reference voltage
#endif
String powerMode = "";                                              // Power mode text

// ----- SMS stuff -----
#define MAX_RESTART 10                                              // Reset CPU after this count of modem restart
#define SIM7000_MODEM_SPEED 115200                                  // SIM7000 GSM modem start speed speed
#define SIM7000_MODEM_RX_PIN gsmRxPin                               // Modem RX pin
#define SIM7000_MODEM_TX_PIN gsmTxPin                               // Modem TX pin

int restartCount = 0;                                               // Count of restart

String gsmState = "";
String messageBuffer = "";
String numberBuffer = "";
bool smsReady = false;                                              // Last smsReady state
String smsState = "";                                               // SMS connected (or not)

// SMS hardware stuff
FF_Sim7000 Sim7000Modem;                                            // Sim7000 GSM Modem
unsigned long lastSmsLoopTime = 0;                                  // Last time we ran smsLoop

//  ---- MQTT client ----
AsyncMqttClient mqttClient;                                         // Asynchronous MQTT client
uint8_t mqttDisconnectedCount = 0;                                  // Count of successive disconnected status
unsigned long lastPublishTime = 0;                                  // Last time we published something
unsigned long lastMqttcheck = 0;                                    // Last time we checked MQTT state
String listeningNodes = "";                                         // List of listening nodes from MQTT LWT
#define MAX_MQTT_DISCONNECTED 15                                    // Restart ESP if more than this number of disconnected count has bee seen
#define MQTT_CHECK_STATE_EVERY 30000                                // Check Mqtt state every 30 seconds

//          --------------------------------------
//          ---- Function/routines definition ----
//          --------------------------------------

//  ---- WiFi ----
#ifdef ESP32
    void onWiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void onWiFiStationGotIp(WiFiEvent_t event, WiFiEventInfo_t info);
#endif

#ifdef ESP8266
    WiFiEventHandler onStationModeConnectedHandler;                 // Event handler called when WiFi is connected
    WiFiEventHandler onStationModeGotIPHandler;		                // Event handler called when WiFi got an IP
#endif

//  ---- NTP ----
void timeSetup(void);
String getTime(void);

//  ---- Log routines ----
void saveLogMessage(const String message);
String getLogLine(const uint16_t line, bool reversedOrder = false);
void logSetup(void);
void syslogSetup(void);

//  ---- Serial commands ----
#ifdef SERIAL_COMMANDS
    void serialLoop(void);
#endif

//  ---- Trace rountines ----

trace_callback(traceCallback);
void traceSetup(void);
void enterRoutine(const char* routineName);

//  ---- System routines ----

#ifdef ESP32
    String verbosePrintResetReason(int reason);
#endif

String getResetCause(void);
String cleanMessage(const char* message);

//  ---- Preferences routines ----

void dumpSettings(void);
bool readSettings(void);
void writeSettings(void);

//  ---- Web server routines ----

void percentDecode(char *src);
int parseUrlParams (char *queryString, char *results[][2], const int resultsMaxCt, const boolean decodeUrl);
void setupReceived(AsyncWebServerRequest *request);
void restReceived(AsyncWebServerRequest *request);
void settingsReceived(AsyncWebServerRequest *request);
void debugReceived(AsyncWebServerRequest *request);
void statusReceived(AsyncWebServerRequest *request);
void setChangedReceived(AsyncWebServerRequest *request);
void languagesReceived(AsyncWebServerRequest *request);
void commandReceived(AsyncWebServerRequest *request);
void logReceived(AsyncWebServerRequest *request);
void send404Error(AsyncWebServerRequest *request);
void notFound(AsyncWebServerRequest *request);

//  ---- OTA routines ----

// void onStartOTA(void);
void onEndOTA(void);
void onErrorOTA(const ota_error_t erreur);

//  ---- Asynchronous MQTT client ----

void mqttSetup(void);
void mqttLoop(void);
static void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
static void onMqttConnect(bool sessionPresent);
static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
static void mqttReconnect(void);
static void mqttConnect(void);

// --- User's routines ---
String getServerMessage(const char *key, const char *defaultValue);
bool inString(const String candidate, const String listOfValues, const String separator = ",");
String extractItem(const String candidate, const uint16_t index, const String separator = ",");
void checkFreeBufferSpace(const char *function, const uint16_t line, const char *bufferName, const size_t bufferSize, const size_t bufferLen);
bool isKnownCommand(const String givenCommand);

//  ---- SMS stuff ----
void smsSetup(void);
void setGsmState(String state);
void sendSms(const String message, const String target=allowedNumbers);
void readSmsCallback(const int pendingSmsIndex, const char* receivedNumber, const char* receivedDate, const char* receivedMessage);
void sendBufferedSMS(void);
void smsLoop(void);
uint8_t pendingMessages(void);

//  ---- Relay ----
bool readRelayState(void);
void writeRelayState(void);
void sendCurrentState(void);
void updateWebServerData(void);
void sendWebServerData(void);
void signalRelayChanged(const bool alsoSendSms = true);
void relaySetup(void);
void activateRelay(void);
void deactivateRelay(void);

// --- Battery routines ---
void batterySetup(void);
#ifdef BATTERY_EXISTS
    float readBatteryVoltage(void);
    uint8_t getBatteryPercent(const float batteryVolts);
    void batteryLoop(void);
#endif

//          ----------------------------
//          ---- Functions/routines ----
//          ----------------------------

//  ---- WiFi ----
#ifdef ESP32
    void onWiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
#endif
#ifdef ESP8266
    void onWiFiStationConnected(WiFiEventStationModeConnected data) {
#endif
    trace_debug_P("Connected to %s", ssid.c_str());
    char buffer[100];
    snprintf(buffer, sizeof(buffer), getServerMessage("wifiClientActive", "Connected to WiFi %s").c_str(), ssid.c_str());
    checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
    wifiState = String(buffer); 
    updateWebServerData();
}

#ifdef ESP32
    void onWiFiStationGotIp(WiFiEvent_t event, WiFiEventInfo_t info) {
#endif
#ifdef ESP8266
    void onWiFiStationGotIp(WiFiEventStationModeGotIP data) {
#endif
    trace_debug_P("Got IP address %s", WiFi.localIP().toString().c_str());
}

//  ---- NTP ----
// Setup time server
void timeSetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    if (ntpServer != "") {
        configTime(0, 0, ntpServer.c_str());
    }
    if (ntpParameters != "") {
        #ifdef ESP32
            setenv("TZ",ntpParameters.c_str(),1);
            tzset();
        #endif
        #ifdef ESP8266
            setTZ(ntpParameters.c_str());
        #endif
    }
}

//  Return current local time as String
String getTime(void) {
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    char dateStr[25];
    strftime(dateStr, sizeof(dateStr), "%Y/%m/%d %H:%M:%S", &timeinfo);
    return String(dateStr);
}

//  ---- Log routines ----

// Save a message to log queue
void saveLogMessage(const String message) {
    savedLogLines[savedLogNextSlot++] = message;
    if (savedLogNextSlot >= LOG_MAX_LINES) {
        savedLogNextSlot = 0;
    }
}

// Returns a log line number
String getLogLine(const uint16_t line, bool reversedOrder) {
    int16_t ptr = savedLogNextSlot;
    if (reversedOrder) {
        ptr -= line + 1;
        if (ptr < 0) {
            ptr += (LOG_MAX_LINES - 1);
        }
    } else {
        ptr += line + 1;
        if (ptr >= LOG_MAX_LINES) {
            ptr -= (LOG_MAX_LINES - 1);
        }
    }
    if (ptr >=0 && ptr < LOG_MAX_LINES) {
        return savedLogLines[ptr];
    }
    return String("");
}

// Setup part for log
void logSetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    // Clear all log slots
    for (uint16_t i = 0; i < LOG_MAX_LINES; i++) {
        savedLogLines[i] = "";
    }
}

// Setup part of syslog
void syslogSetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    #ifdef FF_TRACE_USE_SYSLOG
        if (syslogServer != "") {
            syslog.server(syslogServer.c_str(), syslogPort);
        }
        syslog.defaultPriority(LOG_LOCAL0 + LOG_DEBUG);
        syslog.appName(__FILENAME__);
    #endif
}

//  ---- Serial commands ----
#ifdef SERIAL_COMMANDS
    // Manage Serial commands
    void serialLoop(void) {
        while(Serial.available()>0) {
            char c = Serial.read();
            // Check for end of line
            if (c == '\n' || c== '\r') {
                // Do we have some command?
                if (serialCommandLen) {
                    Serial.printf("Command: %s\n", serialCommand);
                    String command = serialCommand;
                    if (isKnownCommand(command)) {
                        // Command is known and already executed, do nothing
                    } else if (command.startsWith("{")) {
                        // Analyze JSON message (will fail if payload is empty)
                        JsonDocument jsonDoc;
                        auto error = deserializeJson(jsonDoc, command);
                        if (error) {
                            trace_error_P("Failed to parse >%s<. Error: %s", command.c_str(), error.c_str());
                            #ifdef SEND_SMS_FROM_SERIAL
                                Serial.printf(PSTR("{\"status\":\failed to parse\"}\n"));
                            #endif
                            return;
                        }
                        // Analyze message
                        String message = jsonDoc["message"].as<const char *>();
                        String number = jsonDoc["number"].as<const char *>();
                        #ifdef SEND_SMS_FROM_SERIAL
                            Serial.printf(PSTR("{\"status\":\"missing keyword\"}\n"));
                        #endif
                        if (message == "" || number == "") {    // Check for message and number
                            trace_error_P("Message and/or number missing from command %s", command.c_str());
                            #ifdef SEND_SMS_FROM_SERIAL
                                Serial.printf(PSTR("{\"status\":\invalid payload\"}\n"));
                            #endif
                            return;
                        }
                        sendSms(message, number);               // Ok, store the SMS in queue
                        #ifdef SEND_SMS_FROM_SERIAL
                            Serial.printf(PSTR("{\"status\":\"ok\"}\n"));
                        #endif
                    } else {
                        Serial.println("Use ATxxx, send xxx, show modem or enable/disable local/modem trace/debug/enter");
                    }
                }
                // Reset command
                serialCommandLen = 0;
                serialCommand[serialCommandLen] = '\0';
            } else {
                // Do we still have room in buffer?
                if (serialCommandLen < sizeof(serialCommand)) {
                    // Yes, add char
                    serialCommand[serialCommandLen++] = c;
                    serialCommand[serialCommandLen] = '\0';
                } else {
                    // Reset command
                    serialCommandLen = 0;
                    serialCommand[serialCommandLen] = '\0';
                }
            }
        }
    }
#endif

//  ---- Trace rountines ----

trace_declare();                                                    // Declare trace class

// Trace callback routine
//    _level contains severity level
//    _file: calling source file name with extension (unless FF_TRACE_NO_SOURCE_INFO is defined)
//    _line: calling source file line (unless FF_TRACE_NO_SOURCE_INFO is defined)
//    _function: calling calling source function name (unless FF_TRACE_NO_SOURCE_INFO is defined)
//    _message contains message to display/send

trace_callback(traceCallback) {
    String messageLevel = FF_TRACE.textLevel(_level);
    Serial.print(messageLevel);
    Serial.print(": ");
    Serial.println(_message);                                       // Print message on Serial
    if (traceCode) {
        if (_level == FF_TRACE_LEVEL_ERROR || _level == FF_TRACE_LEVEL_WARN) {
            events.send(_message, "error");                         // Send message to destination
        } else if (_level != FF_TRACE_LEVEL_NONE) {
            events.send(_message, "info");                          // Send message to destination
        }
    }
    saveLogMessage(_message);                                       // Save message into circular log
    // Send trace to syslog if needed
    #ifdef FF_TRACE_USE_SYSLOG
        if (syslogServer != "" && WiFi.status() == WL_CONNECTED) {
            syslog.deviceHostname(messageLevel.c_str());
            switch(_level) {
                case FF_TRACE_LEVEL_ERROR:
                    syslog.log(LOG_ERR, _message);
                    break;
                case FF_TRACE_LEVEL_WARN:
                    syslog.log(LOG_WARNING, _message);
                    break;
                case FF_TRACE_LEVEL_INFO:
                    syslog.log(LOG_INFO, _message);
                    break;
                default:
                    syslog.log(LOG_DEBUG, _message);
                    break;
            }
        }
    #endif
}

//  Trace setup code
void traceSetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    trace_register(&traceCallback);                                 // Register callback
    FF_TRACE.setLevel(FF_TRACE_LEVEL_VERBOSE);                      // Start with verbose trace
}

//  Trace each routine entering
void enterRoutine(const char* routineName) {
    trace_debug_P("Entering %s", routineName);
}

//  ---- System routines ----

// Return ESP32 reset reason text
#ifdef ESP32
    String verbosePrintResetReason(int reason) {
        switch ( reason) {
            case 1  : return PSTR("Vbat power on reset");break;
            case 3  : return PSTR("Software reset digital core");break;
            case 4  : return PSTR("Legacy watch dog reset digital core");break;
            case 5  : return PSTR("Deep Sleep reset digital core");break;
            case 6  : return PSTR("Reset by SLC module, reset digital core");break;
            case 7  : return PSTR("Timer Group0 Watch dog reset digital core");break;
            case 8  : return PSTR("Timer Group1 Watch dog reset digital core");break;
            case 9  : return PSTR("RTC Watch dog Reset digital core");break;
            case 10 : return PSTR("Instrusion tested to reset CPU");break;
            case 11 : return PSTR("Time Group reset CPU");break;
            case 12 : return PSTR("Software reset CPU");break;
            case 13 : return PSTR("RTC Watch dog Reset CPU");break;
            case 14 : return PSTR("for APP CPU, reseted by PRO CPU");break;
            case 15 : return PSTR("Reset when the vdd voltage is not stable");break;
            case 16 : return PSTR("RTC Watch dog reset digital core and rtc module");break;
            default : return PSTR("Can't decode reason ")+String(reason);
        }
    }
#endif

// Return ESP reset/restart cause
String getResetCause(void) {
    #ifdef ESP32
        if (localEnterFlag) enterRoutine(__func__);
        String reason = "Reset reasons: CPU#0: "+verbosePrintResetReason(rtc_get_reset_reason(0))
            +", CPU#1: "+verbosePrintResetReason(rtc_get_reset_reason(1));
        return reason;
    #else
        struct rst_info *rtc_info = system_get_rst_info();
        // Get reset reason
        String reason = PSTR("Reset reason: ") + String(rtc_info->reason, HEX) + PSTR(" - ") + ESP.getResetReason();
        // In case of software restart, send additional info
        if (rtc_info->reason == REASON_WDT_RST || rtc_info->reason == REASON_EXCEPTION_RST || rtc_info->reason == REASON_SOFT_WDT_RST) {
            // If crashed, print exception
            if (rtc_info->reason == REASON_EXCEPTION_RST) {
                reason += PSTR(", exception (") + String(rtc_info->exccause)+PSTR("):");
            }
            reason += PSTR(" epc1=0x") + String(rtc_info->epc1, HEX)
                    + PSTR(", epc2=0x") + String(rtc_info->epc2, HEX)
                    + PSTR(", epc3=0x") + String(rtc_info->epc3, HEX)
                    + PSTR(", excvaddr=0x") + String(rtc_info->excvaddr, HEX)
                    + PSTR(", depc=0x") + String(rtc_info->depc, HEX);
        }
        return reason;
    #endif
}

// Cleans \n and \r from a message
String cleanMessage(const char* message){
    if (localEnterFlag) enterRoutine(__func__);
    String cleanedMessage = message;
    cleanedMessage.replace("\n", "\\n");
    cleanedMessage.replace("\r", "\\r");
    return cleanedMessage;
}

#include <unaccentuate.h>

//  ---- Preferences routines ----

// Dumps all settings on screen
void dumpSettings(void) {
    trace_info_P("ssid = %s", ssid.c_str());
    trace_info_P("pwd = %s", pwd.c_str());
    trace_info_P("name = %s", espName.c_str());
    trace_info_P("traceDebug = %s", traceDebug? "true" : "false");
    trace_info_P("traceCode = %s", traceCode? "true" : "false");
    trace_info_P("traceJava = %s", traceJava? "true" : "false");
    trace_info_P("serverLanguage = %s", serverLanguage.c_str());
    trace_info_P("deviceName = %s", deviceName.c_str());
    trace_info_P("onCommand = %s", onCommand.c_str());
    trace_info_P("offCommand = %s", offCommand.c_str());
    trace_info_P("stateCommand = %s", stateCommand.c_str());
    trace_info_P("onButton = %s", onButton.c_str());
    trace_info_P("offButton = %s", offButton.c_str());
    trace_info_P("onState = %s", onState.c_str());
    trace_info_P("offState = %s", offState.c_str());
    trace_info_P("startState = %s", startState);
    trace_info_P("allowedNumbers = %s", allowedNumbers.c_str());
    trace_info_P("syslogServer = %s", syslogServer.c_str());
    trace_info_P("syslogPort = %d", syslogPort);
    trace_info_P("ntpServer = %s", ntpServer.c_str());
    trace_info_P("ntpParameters = %s", ntpParameters.c_str());
    trace_info_P("mqttServer = %s", mqttServer.c_str());
    trace_info_P("mqttPort = %d", mqttPort);
    trace_info_P("mqttUser = %s", mqttUser.c_str());
    trace_info_P("mqttPasw = %s", mqttPasw.c_str());
    trace_info_P("mqttSendTopic = %s", mqttSendTopic.c_str());
    trace_info_P("mqttReceivedTopic = %s", mqttReceivedTopic.c_str());
    trace_info_P("mqttCommandTopic = %s", mqttCommandTopic.c_str());
    trace_info_P("mqttLwtTopic = %s", mqttLwtTopic.c_str());
}

// Read settings
bool readSettings(void) {
    if (localEnterFlag) enterRoutine(__func__);
    File settingsFile = LittleFS.open(SETTINGS_FILE, "r");          // Open settings file
    if (!settingsFile) {                                            // Error opening?
        trace_error_P("Failed to open config file", NULL);
        return false;
    }

    JsonDocument settings;
    auto error = deserializeJson(settings, settingsFile);           // Read settings
    settingsFile.close();                                           // Close file
    if (error) {                                                    // Error reading JSON?
        trace_error_P("Failed to parse config file", NULL);
        return false;
    }

    // Load all settings into corresponding variables
    traceDebug = settings["traceDebug"].as<bool>();
    traceCode = settings["traceCode"].as<bool>();
    traceJava = settings["traceJava"].as<bool>();
    ssid = settings["ssid"].as<String>();
    pwd = settings["pwd"].as<String>();
    espName = settings["name"].as<String>();
    serverLanguage = settings["serverLanguage"].as<String>();
    deviceName = settings["deviceName"].as<String>();
    onCommand = settings["onCommand"].as<String>();
    offCommand = settings["offCommand"].as<String>();
    stateCommand = settings["stateCommand"].as<String>();
    onButton = settings["onButton"].as<String>();
    offButton = settings["offButton"].as<String>();
    onState = settings["onState"].as<String>();
    offState = settings["offState"].as<String>();
    startState = settings["startState"].as<String>();
    allowedNumbers = settings["allowedNumbers"].as<String>();
    syslogServer = settings["syslogServer"].as<String>();
    syslogPort = settings["syslogPort"].as<uint16_t>();
    ntpServer = settings["ntpServer"].as<String>();
    ntpParameters = settings["ntpParameters"].as<String>();
    mqttServer = settings["mqttServer"].as<String>();
    mqttPort = settings["mqttPort"].as<uint16_t>();
    mqttUser = settings["mqttUser"].as<String>();
    mqttPasw = settings["mqttPasw"].as<String>();
    mqttSendTopic = settings["mqttSendTopic"].as<String>();
    mqttReceivedTopic = settings["mqttReceivedTopic"].as<String>();
    mqttCommandTopic = settings["mqttCommandTopic"].as<String>();
    mqttLwtTopic = settings["mqttLwtTopic"].as<String>();
    // Use syslog port default value if needed
    if (syslogPort == 0) {
        syslogPort = 514;
    }
    // Use mqtt port default value if needed
    if (mqttPort == 0) {
        mqttPort = 1883;
    }
    // Dump settings on screen
    dumpSettings();
    return true;
}

// Write settings
void writeSettings(void) {
    if (localEnterFlag) enterRoutine(__func__);
    JsonDocument settings;

    // Load settings in JSON
    settings["ssid"] = ssid.c_str();
    settings["pwd"] = pwd.c_str();
    settings["name"] = espName.c_str();
    settings["traceDebug"] = traceDebug;
    settings["traceCode"] = traceCode;
    settings["traceJava"] = traceJava;
    settings["serverLanguage"] = serverLanguage.c_str();
    settings["deviceName"] = deviceName.c_str();
    settings["onCommand"] = onCommand.c_str();
    settings["offCommand"] = offCommand.c_str();
    settings["stateCommand"] = stateCommand.c_str();
    settings["onButton"] = onButton.c_str();
    settings["offButton"] = offButton.c_str();
    settings["onState"] = onState.c_str();
    settings["offState"] = offState.c_str();
    settings["startState"] = startState.c_str();
    settings["allowedNumbers"] = allowedNumbers.c_str();
    settings["syslogServer"] = syslogServer.c_str();
    settings["syslogPort"] = syslogPort;
    settings["ntpServer"] = ntpServer.c_str();
    settings["ntpParameters"] = ntpParameters.c_str();
    settings["mqttServer"] = mqttServer.c_str();
    settings["mqttPort"] = mqttPort;
    settings["mqttUser"] = mqttUser.c_str();
    settings["mqttPasw"] = mqttPasw.c_str();
    settings["mqttSendTopic"] = mqttSendTopic.c_str();
    settings["mqttReceivedTopic"] = mqttReceivedTopic.c_str();
    settings["mqttCommandTopic"] = mqttCommandTopic.c_str();
    settings["mqttLwtTopic"] = mqttLwtTopic.c_str();

    File settingsFile = LittleFS.open(SETTINGS_FILE, "w");          // Open settings file
    if (!settingsFile) {                                            // Error opening?
        trace_error_P("Can't open for write settings file", NULL);
        return;
    }

    uint16_t bytes = serializeJsonPretty(settings, settingsFile);   // Write JSON structure to file
    if (!bytes) {                                                   // Error writting?
        trace_error_P("Can't write settings file", NULL);
    }
    settingsFile.flush();                                           // Flush file
    settingsFile.close();                                           // Close it
    if (traceCode) {
        trace_info_P("Sending settings event", NULL);
    }
    events.send("Ok", "settings");                                  // Send a "settings" (changed) event
}

//  ---- Web server routines ----

//  Perform URL percent decoding
void percentDecode(char *src) {
    char *dst = src;
    while (*src) {
        if (*src == '+') {
            src++;
            *dst++ = ' ';
        } else if (*src == '%') {
            // handle percent escape
            *dst = '\0';
            src++;
            if (*src >= '0' && *src <= '9') {*dst = *src++ - '0';}
            else if (*src >= 'A' && *src <= 'F') {*dst = 10 + *src++ - 'A';}
            else if (*src >= 'a' && *src <= 'f') {*dst = 10 + *src++ - 'a';}
            *dst <<= 4;
            if (*src >= '0' && *src <= '9') {*dst |= *src++ - '0';}
            else if (*src >= 'A' && *src <= 'F') {*dst |= 10 + *src++ - 'A';}
            else if (*src >= 'a' && *src <= 'f') {*dst |= 10 + *src++ - 'a';}
            dst++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

//  Parse an URL parameters list and return each parameter and value in a given table
int parseUrlParams (char *queryString, char *results[][2], const int resultsMaxCt, const boolean decodeUrl) {
    int ct = 0;

    while (queryString && *queryString && ct < resultsMaxCt) {
    results[ct][0] = strsep(&queryString, "&");
    results[ct][1] = strchr(results[ct][0], '=');
    if (*results[ct][1]) *results[ct][1]++ = '\0';
    if (decodeUrl) {
        percentDecode(results[ct][0]);
        percentDecode(results[ct][1]);
    }
    ct++;
    }
    return ct;
}
// Called when /setup is received
void setupReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "setup.htm", "text/html");
    request->send(response);                                        // Send setup.htm
}

// Called when /rest is received
void restReceived (AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    char tempBuffer[700];
    if (request->url().startsWith("/rest/params&") || request->url().startsWith("/rest/send&")) {
        // /rest/params or /rest/send -> send an SMS
        //      &number=0123456789 : phone number to send message to
        //      &message=abcdefg hijklm nopqrs : message to send
        char paramList[600];                                        // Given parameters, excluding initial &
        char *params[3][2];                                         // Will contain 3 couples of param/value
        memset(paramList, 0, sizeof(paramList));                    // Clear param list
        strncpy(paramList,                                          // Copy to param list
            request->url().substring(request->url().indexOf('&')+1).c_str() // From URL
            , sizeof(paramList));                                   // Max size
        trace_debug_P("Params: %s", paramList);
        int nbParams = parseUrlParams(paramList, params, 3, true);  // Extract up to 3 parameters
        String number = "";
        String message = "";
        for (int i = 0; i < nbParams; i++) {                        // Check given params
            if (!strcmp(params[i][0], "number")) {                  // Extract number
                number = String(params[i][1]);
                number.replace(" ","+");
            } else if (!strcmp(params[i][0], "message")) {          // Extract message
                message = String(params[i][1]);
                message.replace("\n", "<br>");
            } else {                                                // Parameter is unknown
                snprintf_P(tempBuffer, sizeof(tempBuffer), PSTR("Unknown topic '%s'"), params[i][0]);
                checkFreeBufferSpace(__func__, __LINE__, "tempBuffer", sizeof(tempBuffer), strlen(tempBuffer));
                trace_debug_P("%s", tempBuffer);
                request->send(400, "text/plain", tempBuffer);
                return;
            }
        }
        if (number != "" && message != "") {                        // Number and message should be given
            sendSms(message.c_str(), number.c_str());               // Ok, sends the SMS
            snprintf_P(tempBuffer, sizeof(tempBuffer), PSTR("ok\n"));
            checkFreeBufferSpace(__func__, __LINE__, "tempBuffer", sizeof(tempBuffer), strlen(tempBuffer));
            request->send(200, "text/plain", tempBuffer);
        } else {                                                    // Load error message
            snprintf_P(tempBuffer, sizeof(tempBuffer), PSTR("Missing number and/or message"));
            checkFreeBufferSpace(__func__, __LINE__, "tempBuffer", sizeof(tempBuffer), strlen(tempBuffer));
            trace_error_P("%s", tempBuffer);
            request->send(400, "text/plain", tempBuffer);
        }
    } else if (request->url() == "/rest/listening") {
        request->send(200, "text/plain", (listeningNodes.length() > 2 ? listeningNodes.substring(2, listeningNodes.length()).c_str(): getServerMessage("emptyData", "[Empty]")));
        return;
    } else if (request->url() == "/rest/restart") {
        request->send(200, "text/plain", "Restarting...");
        delay(1000);
        ESP.restart();
        return;
    }
}

// Called when /settings is received
void settingsReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, SETTINGS_FILE, "application/json");
    request->send(response);                                        // Send settings.json
}

// Called when /debug is received
void debugReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    // Send a json document with interresting variables
    JsonDocument answer;
    answer["version"] = VERSION;
    answer["relayActive"] = relayActive;
    answer["powerMode"] = powerMode.c_str();
    answer["wifiState"] = wifiState.c_str();
    answer["smsState"] = smsState.c_str();
    answer["gsmState"] = gsmState.c_str();
    answer["mqttConnected"] = mqttClient.connected();
    answer["isIdle"] = Sim7000Modem.isIdle();
    answer["isSending"] = Sim7000Modem.isSending();
    answer["isReceiving"] = Sim7000Modem.isReceiving();
    answer["pendingSms"] = pendingMessages();
    answer["localDebugFlag"] = localDebugFlag;
    answer["localTraceFlag"] = localTraceFlag;
    answer["localEnterFlag"] = localEnterFlag;
    answer["Sim7000DebugFlag"] = Sim7000Modem.debugFlag;
    answer["Sim7000TraceFlag"] = Sim7000Modem.traceFlag;
    answer["Sim7000EnterFlag"] = Sim7000Modem.traceEnterFlag;
    answer["lastSentMessage"] = Sim7000Modem.lastSentMessage.c_str();
    answer["lastSentDate"] = Sim7000Modem.lastSentDate.c_str();
    answer["lastSentNumber"] = Sim7000Modem.lastSentNumber.c_str();
    answer["lastReceivedMessage"] = Sim7000Modem.lastReceivedMessage.c_str();
    answer["lastReceivedDate"] = Sim7000Modem.lastReceivedDate.c_str();
    answer["lastReceivedNumber"] = Sim7000Modem.lastReceivedNumber.c_str();
    answer["date"] = getTime();
    #ifdef ESP32
        answer["freeMemory"] = ESP.getFreeHeap();
        answer["largestChunk"] = ESP.getMaxAllocHeap();
        answer["memoryLowMark"] = ESP.getMinFreeHeap();
    #else
        answer["freeMemory"] = ESP.getFreeHeap();
        answer["largestChunk"] = ESP.getMaxFreeBlockSize();
    #endif
    String  buffer;
    serializeJsonPretty(answer, buffer);
    request->send(200, "application/json", buffer);
}

// Called when a /status click is received
void statusReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    // Send a json document with data correponding to current status
    JsonDocument answer;
    answer["state"] = relayActive? onState : offState;
    answer["powerMode"] = powerMode.c_str();
    answer["smsState"] = smsState.c_str();
    #ifdef ESP32
        char buffer[1024];
    #endif
    #ifdef ESP8266
        char buffer[256];
    #endif
    serializeJsonPretty(answer, buffer, sizeof(buffer));
    checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
    request->send(200, "application/json", buffer);
}

// Called when /changed/<variable name>/<variable value> is received
void setChangedReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    String position = request->url().substring(1);
    if (traceCode) {
        trace_info_P("Received %s", position.c_str());
    }
    int separator1 = position.indexOf("/");                         // Position of first "/"
    if (separator1 >= 0) {
        int separator2 = position.indexOf("/", separator1+1);       // Position of second "/"
        if (separator2 > 0) {
            // Extract field name and value
            String fieldName = position.substring(separator1+1, separator2);
            String fieldValue = position.substring(separator2+1);
            // Check against known field names and set value accordingly
            if (fieldName == "traceDebug") {
                traceDebug = (fieldValue == "true");
            } else if (fieldName == "traceCode") {
                traceCode = (fieldValue == "true");
            } else if (fieldName == "traceJava") {
                traceJava = (fieldValue == "true");
            } else if (fieldName == "ssid") {
                ssid = fieldValue;
            } else if (fieldName == "pwd") {
                pwd = fieldValue;
            } else if (fieldName == "name") {
                espName = fieldValue;
            } else if (fieldName == "serverLanguage") {
                serverLanguage = fieldValue;
            } else if (fieldName == "deviceName") {
                deviceName = fieldValue;
            } else if (fieldName == "onCommand") {
                onCommand = fieldValue;
            } else if (fieldName == "offCommand") {
                offCommand = fieldValue;
            } else if (fieldName == "stateCommand") {
                stateCommand = fieldValue;
            } else if (fieldName == "onButton") {
                onButton = fieldValue;
            } else if (fieldName == "offButton") {
                offButton = fieldValue;
            } else if (fieldName == "onState") {
                onState = fieldValue;
            } else if (fieldName == "offState") {
                offState = fieldValue;
            } else if (fieldName == "startState") {
                startState = fieldValue;
            } else if (fieldName == "allowedNumbers") {
                allowedNumbers = fieldValue;
            } else if (fieldName == "syslogServer") {
                #ifdef FF_TRACE_USE_SYSLOG
                    if (fieldValue != "") {
                        if (syslogServer != fieldValue) {
                            syslog.server(fieldValue.c_str(), syslogPort);
                        }
                    }
                #endif
                syslogServer = fieldValue;
            } else if (fieldName == "syslogPort") {
                #ifdef FF_TRACE_USE_SYSLOG
                    if (fieldValue.toInt() > 0 && syslogServer != "") {
                        if (syslogPort != fieldValue.toInt()) {
                            syslog.server(syslogServer.c_str(), fieldValue.toInt());
                        }
                    }
                #endif
                syslogPort = fieldValue.toInt();
            } else if (fieldName == "ntpServer") {
                ntpServer = fieldValue;
                timeSetup();
            } else if (fieldName == "ntpParameters") {
                ntpParameters = fieldValue;
                timeSetup();
            } else if (fieldName == "mqttServer") {
                if (mqttServer != fieldValue) {
                    mqttServer = fieldValue;
                    mqttReconnect();
                }
            } else if (fieldName == "mqttPort") {
                if (fieldValue.toInt() > 0 && mqttPort != fieldValue.toInt()) {
                    mqttPort = fieldValue.toInt();
                    mqttReconnect();
                }
            } else if (fieldName == "mqttUSer") {
                if (mqttUser != fieldValue) {
                    mqttUser = fieldValue;
                    mqttReconnect();
                }
            } else if (fieldName == "mqttPasw") {
                if (mqttPasw != fieldValue) {
                    mqttPasw = fieldValue;
                    mqttReconnect();
                }
            } else if (fieldName == "mqttSendTopic") {
                mqttSendTopic = fieldValue;
            } else if (fieldName == "mqttReceivedTopic") {
                mqttReceivedTopic = fieldValue;
            } else if (fieldName == "mqttCommandTopic") {
                mqttCommandTopic = fieldValue;
            } else if (fieldName == "mqttLwtTopic") {
                mqttLwtTopic = fieldValue;
            } else {
                // This is not a known field
                trace_error_P("Can't set field %s", fieldName.c_str());
                char msg[70];                                       // Buffer for message
                snprintf_P(msg, sizeof(msg),PSTR("<status>Bad field name %s</status>"), fieldName.c_str());
                checkFreeBufferSpace(__func__, __LINE__, "msg", sizeof(msg), strlen(msg));
                request->send(400, "", msg);
                return;
            }
            writeSettings();
        } else {
            // This is not a known field
            trace_error_P("No field name", NULL);
            char msg[70];                                           // Buffer for message
            snprintf_P(msg, sizeof(msg),PSTR("<status>No field name</status>"));
            checkFreeBufferSpace(__func__, __LINE__, "msg", sizeof(msg), strlen(msg));
            request->send(400, "", msg);
            return;
        }
    }
    request->send(200, "", "<status>Ok</status>");
}

// Called when /languages command is received
void languagesReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    String path = "/";
#ifdef ESP32
    File dir = LittleFS.open(path);
#else
    Dir dir = LittleFS.openDir(path);
    path = String();
#endif
    String output = "[";
#ifdef ESP32
    File entry = dir.openNextFile();
    while(entry){
#else
    while(dir.next()){
        fs::File entry = dir.openFile("r");
#endif
        String fileName = String(entry.name());
        if (fileName.startsWith("lang_")) {
            #ifdef ESP32
                fileName = path + fileName;
            #endif
            File languageFile = LittleFS.open(fileName, "r");       // Open language file
            if (languageFile) {                                     // Open ok?
                JsonDocument jsonData;
                auto error = deserializeJson(jsonData, languageFile);       // Read settings
                languageFile.close();                               // Close file
                if (!error) {                                       // Reading JSON ok?
                    if (output != "[") output += ',';
                    output += "{\"code\":\"";
                    output += jsonData["code"].as<String>();
                    output += "\",\"text\":\"";
                    output += jsonData["text"].as<String>();
                    output += "\"}";
                } else {
                    trace_error_P("Can't decode %s", fileName.c_str());
                }
            } else {
                trace_error_P("Can't open %s", fileName.c_str());
            }
        }
        #ifdef ESP32
            entry = dir.openNextFile();
        #else
            entry.close();
        #endif
        }
#ifdef ESP32
    dir.close();
#endif
    output += "]";
    request->send(200, "application/json", output);
    output = String();
}

// Called when /command/<command name>/<commandValue> is received
void commandReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    String position = request->url().substring(1);
    if (traceCode) {
        trace_info_P("Received %s", position.c_str());
    }
    String commandName = "";
    String commandValue = "";
    int separator1 = position.indexOf("/");                         // Position of first "/"
    if (separator1 >= 0) {
        int separator2 = position.indexOf("/", separator1+1);       // Position of second "/"
        if (separator2 > 0) {
            // Extract field name and value
            commandName = position.substring(separator1+1, separator2);
            commandValue = position.substring(separator2+1);
        } else {
            commandName = position.substring(separator1+1);
        }
        // Check against known command names
        if (commandName == "turnOn") {
            activateRelay();
        } else if (commandName == "turnOff") {
            deactivateRelay();
        } else {
            // This is not a known field
            trace_error_P("Can't execute command %s", commandName.c_str());
            char msg[70];                                           // Buffer for message
            snprintf_P(msg, sizeof(msg),PSTR("<status>Bad command name %s</status>"), commandName.c_str());
            checkFreeBufferSpace(__func__, __LINE__, "msg", sizeof(msg), strlen(msg));
            request->send(400, "", msg);
            return;
        }
    } else {
        trace_error_P("No command name", NULL);
        char msg[70];                                               // Buffer for message
        snprintf_P(msg, sizeof(msg),PSTR("<status>No command name</status>"));
        checkFreeBufferSpace(__func__, __LINE__, "msg", sizeof(msg), strlen(msg));
        request->send(400, "", msg);
        return;
    }
    request->send(200, "", "<status>Ok</status>");
}

// Called when /log is received - Send saved log, line by line
void logReceived(AsyncWebServerRequest *request) {
    if (localEnterFlag) enterRoutine(__func__);
    AsyncWebServerResponse *response = request->beginChunkedResponse("text/plain; charset=utf-8", [](uint8_t *logResponseBuffer, size_t maxLen, size_t index) -> size_t {
        // For all log lines
        while (logRequestNextLog < LOG_MAX_LINES) {
            // Get message
            String message = getLogLine(logRequestNextLog++);
            // If not empty
            if (message != "") {
                // Compute message len (adding a "\" at end)
                size_t chunkSize = min(message.length(), maxLen-1)+1;
                // Copy message
                memcpy(logResponseBuffer, message.c_str(), chunkSize-1);
                // Add "\n" at end
                logResponseBuffer[chunkSize-1] = '\n';
                // Return size (and message loaded)
                return chunkSize;
            }
        }
        // That's the end
        return 0;
    });
    logRequestNextLog = 0;
    request->send(response);
}


// Sends a 404 error with requested file name
void send404Error(AsyncWebServerRequest *request) {
    char msg[120];
    snprintf_P(msg, sizeof(msg), PSTR("File %s not found"), request->url().c_str());
    checkFreeBufferSpace(__func__, __LINE__, "msg", sizeof(msg), strlen(msg));
    request->send(404, "text/plain", msg);
    if (traceCode) {
        trace_info(msg, NULL);
    }
}

// Called when a request can't be mapped to existing ones
void notFound(AsyncWebServerRequest *request) {
    send404Error(request);
}

//  ---- OTA routines ----

// Called when OTA starts
void onStartOTA(void) {
    if (localEnterFlag) enterRoutine(__func__);
    if (ArduinoOTA.getCommand() == U_FLASH) {                       // Program update
        if (traceCode) {
            trace_info_P("Starting firmware update", NULL);
        }
    } else {                                                        // File system uodate
        if (traceCode) {
            trace_info_P("Starting file system update", NULL);
        }
        LittleFS.end();
    }
}

// Called when OTA ends
void onEndOTA(void) {
    if (localEnterFlag) enterRoutine(__func__);
    if (traceCode) {
        trace_info_P("End of update", NULL);
    }
}

// Called when OTA error occurs
void onErrorOTA(const ota_error_t erreur) {
    if (localEnterFlag) enterRoutine(__func__);
    String msg = "OTA error(";
    msg += String(erreur);
    msg += ") : Error ";
    if (erreur == OTA_AUTH_ERROR) {
        msg += "authentication";
    } else if (erreur == OTA_BEGIN_ERROR) {
        msg += "starting";
    } else if (erreur == OTA_CONNECT_ERROR) {
        msg += "connecting";
    } else if (erreur == OTA_RECEIVE_ERROR) {
        msg += "receiving";
    } else if (erreur == OTA_END_ERROR) {
        msg += "terminating";
    } else {
        msg += "unknown !";
    }
    trace_error(msg.c_str(), NULL);
}

//  ---- Asynchronous MQTT client ----

// MQTT setup
void mqttSetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    if (mqttServer == "") {                                         // If MQTT server is not defined
        trace_debug_P("MQTT not configured ...", NULL);
        return;
    }
    mqttClient.setServer(mqttServer.c_str(), mqttPort);             // Set server IP (or name), and port
    mqttClient.setClientId(espName.c_str());                        // Set client id (= nodeName)
    mqttClient.setCredentials(mqttUser.c_str(), mqttPasw.c_str());  // Set MQTT user and password
    mqttClient.onMessage(&onMqttMessage);                           // On message (when subscribed item is received) callback
    mqttClient.onConnect(&onMqttConnect);                           // On connect (when MQTT is connected) callback
    mqttClient.onDisconnect(&onMqttDisconnect);                     // On disconnect (when MQTT is disconnected) callback
    if (mqttLwtTopic != ""){
        mqttClient.setWill(mqttLwtTopic.c_str(), 1, true,           // Last will topic
            "{\"state\":\"down\"}");
    }
    if (WiFi.status() == WL_CONNECTED) {                            // If we're conncted to WiFi
        mqttClient.connect();                                       // Connect to MQTT
    }
}

// Loop part of MQTT
void mqttLoop (void) {
    unsigned long now = millis();
    if ((now - lastMqttcheck) > MQTT_CHECK_STATE_EVERY) {
        lastMqttcheck = now;
        if (WiFi.status() == WL_CONNECTED) {                        // Is WiFi connected
        if (!mqttClient.connected()) {                              // Is MQTT client disconnected?
            mqttConnect();                                          // Try to (re)connect
        }
        } else {                                                    // WiFi not connected
            ssid.trim();
            if (ssid != "") {                                       // SSID defined?
                WiFi.disconnect();                                  // Disconnect from WiFi
                WiFi.begin(ssid.c_str(), pwd.c_str());              // Reconnect to existing SSID
            }
        }
    }
}

// Executed when a subscribed message is received
//  Input:
//      topic: topic of received message
//      payload: content of message (WARNING: without ending null character, use len)
//      properties: MQTT properties associated with this message
//      len: payload length
//      index: index of this message (for long messages)
//      total: total message count (for long messages)
static void onMqttMessage(char* topic, char* payloadPtr, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    if (localEnterFlag) enterRoutine(__func__);
    char payload[len+1];                                            // Allocate size for message plus null ending character
    strncpy(payload, payloadPtr, len);                              // Copy message on given len
    payload[len] = 0;                                               // Add zero at end
    String strTopic = String(topic);
    trace_debug_P("Received: %s from %s", payload, topic);
    if (strTopic == mqttSendTopic) {
        // This is a request to send a SMS
        JsonDocument jsonDoc;
        auto error = deserializeJson(jsonDoc, payload);
        if (error) {
            trace_error_P("Failed to parse >%s<. Error: %s", payload, error.c_str());
            return;
        }
        // Analyze payload
        String message = jsonDoc["message"].as<const char *>();
        String number = jsonDoc["number"].as<const char *>();
        if (message == "" || number == "") {    // Check for message and number
            trace_error_P("Message and/or number missing from MQTT payload %s", payload);
            return;
        }
        sendSms(message, number);               // Ok, store the SMS in queue
    } else if (strTopic == mqttCommandTopic) {
        if (!isKnownCommand(String(payload))) {
            trace_error_P("Command %s is unknown", payload);
        }
    } else if (strTopic.startsWith(mqttLwtTopic)) {
        // This is a LWT message from a connected node
        String node = String(topic).substring(mqttLwtTopic.length()+1);   // Extract node part from topic
        if (node != "") {                                           // Is node specified?
            // Does payload exists?
            if (payloadPtr[0]) {
                JsonDocument jsonDoc;
                auto error = deserializeJson(jsonDoc, payload);
                if (error) {
                    trace_error_P("Failed to parse %s. Error: %s", payload, error.c_str());
                    return;
                }
                String state =  jsonDoc["state"].as<const char *>();    // Get state item
                if (state) {
                    trace_info_P("Node >%s< is >%s<", node.c_str(), state.c_str());
                    // Try to locate node in listeningNodes
                    uint8_t startPos = listeningNodes.indexOf(", "+node+":");
                    if (startPos >=0) {
                        uint8_t endPos = listeningNodes.indexOf(", ",startPos+1);
                        // Remove node from list
                        listeningNodes = listeningNodes.substring(0, startPos+1) + listeningNodes.substring(endPos+1);
                    }
                    // Add node at end of list
                    listeningNodes += ", " + node + ":" + state;
                } else {
                    trace_error_P("'State' missing from MQTT payload %s for topic %s", payload, topic);
                }
            } else {
                // There's no payload, node is deleted
                trace_info_P("Node >%s< is deleted", node.c_str());
                // Try to locate node in listeningNodes
                uint8_t startPos = listeningNodes.indexOf(", "+node+":");
                if (startPos >=0) {
                    // Locate end of node name
                    uint8_t endPos = listeningNodes.indexOf(", ",startPos+1);
                    // Remove node from list as it was deleted
                    listeningNodes = listeningNodes.substring(0, startPos+1) + listeningNodes.substring(endPos+1);
                }
            }
        }
    } else {
        trace_error_P("Can't understand %s as topic", topic);
    }
}

//Executed when MQTT is connected
static void onMqttConnect(bool sessionPresent) {
    if (localEnterFlag) enterRoutine(__func__);
    trace_debug_P("MQTT connected", NULL);
    if (mqttLwtTopic != "") {
        String lwtData = "{\"state\":\"up\",\"id\":\"" + espName + "\",\"version\":\"" + String(VERSION) + "\"}";
        uint16_t result = mqttClient.publish(mqttLwtTopic.c_str(), 0, true, lwtData.c_str()); // Last will topic
        lastPublishTime = millis();
        if (!result) {
            trace_error_P("Publish to %s returned %d", mqttLwtTopic.c_str(), result);
        }
    }
    if (mqttSendTopic != "") {
        trace_debug_P("Subscribing to %s", mqttSendTopic.c_str());
        mqttClient.subscribe(mqttSendTopic.c_str(), 0);             // Subscribe to get topic (messages to send)
    }
    if (mqttCommandTopic != "") {
        trace_debug_P("Subscribing to %s", mqttCommandTopic.c_str());
        mqttClient.subscribe(mqttCommandTopic.c_str(), 0);             // Subscribe to command topic (debug commands)
    }
    if (mqttLwtTopic != "") {
        trace_debug_P("Subscribing to %s", (mqttLwtTopic+"/#").c_str());
        mqttClient.subscribe((mqttLwtTopic+"/#").c_str(), 0);       // Subscribe to LWT sub topics
    }
    updateWebServerData();
}

// Reconnect MQTT serverafter parameter change
static void mqttReconnect(void) {
    if (localEnterFlag) enterRoutine(__func__);
    if (mqttClient.connected()) {
        mqttClient.disconnect();
    }
    if (mqttServer != "" && mqttPort) {                             // If at least server name and port are defined
        trace_debug_P("Reconnecting to MQTT...", NULL);
        mqttClient.setServer(mqttServer.c_str(), mqttPort);         // Set server IP (or name), and port
        mqttClient.setClientId(espName.c_str());                    // Set client id (= nodeName)
        mqttClient.setCredentials(mqttUser.c_str(), mqttPasw.c_str()); // Set MQTT user and password
        mqttClient.connect();                                       // Start connection
    }
}

//Executed when MQTT is disconnected
static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    if (localEnterFlag) enterRoutine(__func__);
    trace_error_P("MQTT disconnected, reason %d", (uint8_t) reason);
    updateWebServerData();
}

// Establish connection with MQTT server
static void mqttConnect(void) {
    if (localEnterFlag) enterRoutine(__func__);
    if (mqttServer != "" && !mqttClient.connected()) {              // If not yet connected and server defined
        mqttDisconnectedCount++;                                    // Increment disconnected count
        if (mqttDisconnectedCount > MAX_MQTT_DISCONNECTED) {
            trace_error_P("Restarting after %d MQTT disconnected events", mqttDisconnectedCount);
            ESP.restart();                                          // Restart ESP
        }
        trace_debug_P("Connecting to MQTT...", NULL);
        mqttClient.connect();                                       // Start connection
    }
}

// --- User's routines ---

// Get a message using server language
String getServerMessage(const char *key, const char *defaultValue) {
    String fileName = "/lang_" + serverLanguage + ".json";
    File languageFile = LittleFS.open(fileName, "r");               // Open language file
    if (languageFile) {                                             // Open ok?
        JsonDocument jsonData;
        deserializeJson(jsonData, languageFile);                    // Read settings
        languageFile.close();                                       // Close file
        String dataValue = jsonData[key].as<String>();              // Return data
        if (dataValue != "" && dataValue != NULL) {
            return dataValue;
        }
    }
    return String(defaultValue);
}

// Looks for string into a list of strings
bool inString(const String candidate, const String listOfValues, const String separator) {
    int endPosition = 0;
    int startPosition = 0;
    String allValues = listOfValues + separator;
    while (endPosition >= 0) {
        endPosition = allValues.indexOf(separator, startPosition);
        // Compare sending number with extracted one
        if (candidate.equalsIgnoreCase(allValues.substring(startPosition, endPosition))) {
            return true;
        }
        startPosition = endPosition + 1;
    }
    return false;
}

// Returns part of a string, giving index and delimiter
String extractItem(const String candidate, const uint16_t index, const String separator) {
    int endPosition = 0;
    int startPosition = 0;
    int i = 0;
    String allValues = candidate + separator;
    while (endPosition >= 0) {
        endPosition = allValues.indexOf(separator, startPosition);
        if (i == index) {
            // Return part corresponding to required item
            return allValues.substring(startPosition, endPosition);
        }
        startPosition = endPosition + 1;
    }
    return "";
}

// check for remaining space into a buffer
void checkFreeBufferSpace(const char *function, const uint16_t line, const char *bufferName, const size_t bufferSize, const size_t bufferLen) {
    if ((bufferSize - bufferLen) < 0 || bufferSize <= 0) {
        trace_error_P("Invalid size %d and length %d for %s in %s:%d", bufferSize, bufferLen, bufferName, function, line);
    } else {
        size_t freeSize = bufferSize - bufferLen;
        uint8_t percent = (bufferLen * 100)/ bufferSize;
        if (freeSize < 50 || percent > 90) {
            trace_info_P("%s:%d: '%s' is %d\%% full, %d bytes remaining (size %d, length %d))",
                function, line, bufferName, percent, freeSize, bufferSize, bufferLen);
        } else if (percent < 20) {
            trace_debug_P("%s:%d: '%s' is %d%% only full, %d bytes remaining (size %d, length %d))",
                function, line, bufferName, percent, freeSize, bufferSize, bufferLen);
        }
    }
}

// Execute a command, received either by Serial or MQTT
bool isKnownCommand(const String givenCommand) {
	String command = String(givenCommand);
	if (command.startsWith("AT")) {
		// Send an AT command
		Sim7000Modem.sendAT(command.c_str());
	} else if (command.startsWith("send ")) {
		// Send an SMS message
		sendSms(command.substring(5).c_str());
	} else {
		command.toLowerCase();
		// enable/disable local/modem trace/debug
		if (command == "enable local debug") {
			localDebugFlag = true;
		} else if (command == "disable local debug") {
			localDebugFlag = false;
		} else if (command == "enable local trace") {
			localTraceFlag = true;
		} else if (command == "disable local trace") {
			localTraceFlag = false;
		} else if (command == "enable local enter") {
			localEnterFlag = true;
		} else if (command == "disable local enter") {
			localEnterFlag = false;
		} else if (command == "enable modem debug") {
			Sim7000Modem.debugFlag = true;
		} else if (command == "disable modem debug") {
			Sim7000Modem.debugFlag = false;
		} else if (command == "enable modem trace") {
			Sim7000Modem.traceFlag = true;
		} else if (command == "disable modem trace") {
			Sim7000Modem.traceFlag = false;
		} else if (command == "disable modem enter") {
			Sim7000Modem.traceEnterFlag = false;
		} else if (command == "disable modem enter") {
			Sim7000Modem.traceEnterFlag = false;
		} else if (command == "show modem") {
			// Show modem debug data
			Sim7000Modem.debugState();
		} else {
			return false;
		}
    }
	return true;
}

//  ---- SMS stuff ----

//  Setup SMS modem
void smsSetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    // Initialize Strings with localized values
    setGsmState(getServerMessage("gsmNotStarted", "Not started"));
    smsState = getServerMessage("smsDisconnected", "!!! SMS network *NOT* connected !!!");
    updateWebServerData();

    // SMS server specific setup
    Sim7000Modem.registerSmsCb(readSmsCallback);                    // SMS received callback
    Sim7000Modem.registerSendCb(sendSmsCallback);                   // SMS sent callback
    Sim7000Modem.debugFlag = true;
    Sim7000Modem.traceFlag = false;
    String empty = getServerMessage("emptyData", "[Empty]");
    Sim7000Modem.lastReceivedDate = empty;
    Sim7000Modem.lastReceivedMessage = empty;
    Sim7000Modem.lastReceivedNumber = empty;
    Sim7000Modem.lastSentDate = empty;
    Sim7000Modem.lastSentMessage = empty;
    Sim7000Modem.lastSentNumber = empty;
    // Start Sim7000
    trace_info_P("GSM starting!", NULL);
    Sim7000Modem.begin(SIM7000_MODEM_SPEED, gsmRxPin, gsmTxPin, gsmPowerPin);   // Start Sim7000 communication channel
}

// Load GSM state
void setGsmState(String state){
    if (gsmState != state) {
        gsmState = state;
        updateWebServerData();
    }
}

// Send a SMS
void sendSms(const String message, const String target) {
    if (localEnterFlag) enterRoutine(__func__);
    // Send SMS to target list (default to allowedNumbers)
    uint16_t i = 0;
    while (true) {
        String oneTarget = extractItem(target, i++);
        if (oneTarget == "") {
            break;
        }
        // Stores SMS message and number in queue
        trace_info_P("Sending %s to %s", message.c_str(), oneTarget.c_str());
        messageBuffer += message + String('\255');
        numberBuffer += oneTarget + String('\255');
    }
}

// Receive a SMS
void readSmsCallback(const char* receivedNumber, const char* receivedDate, const char* receivedMessage) {
    if (localEnterFlag) enterRoutine(__func__);
    trace_info(getServerMessage("receivedMessage", "Received %s from %s on %s").c_str(), receivedMessage, receivedNumber, receivedDate);
    updateWebServerData();
    // Check for a known sender phone number
    if (inString(String(receivedNumber), allowedNumbers )) {        // Sender is known
        String purgedMessage(unaccentuate(String(receivedMessage), true));
        purgedMessage.trim();
        // Check for commands into received message
        if (inString(purgedMessage, onCommand)) {
            // We have a "on" command
            activateRelay();
        } else if (inString(purgedMessage, offCommand)) {
            // We have a "off" command
            deactivateRelay();
        } else if (inString(purgedMessage, stateCommand)) {
            // We have a "state" command
            sendCurrentState();
        } else {
            if (mqttServer != "") {
                // Compose jsonMessage
                JsonDocument jsonDoc;
                jsonDoc["number"] = receivedNumber;                 // Set received number
                jsonDoc["date"] = receivedDate;                     // ... date
                jsonDoc["message"] = receivedMessage;               // ... and message
                #ifdef ESP32
                    char buffer[1024];
                #endif
                #ifdef ESP8266
                    char buffer[256];
                #endif
                serializeJson(jsonDoc, buffer, sizeof(buffer));     // Convert json to string
                checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
                #ifdef PRINT_RECEIVED_SMS_ON_SERIAL
                    Serial.println(buffer);                           // Print message on Serial
                #endif
                // Publish received SMS on MQTT
                if (mqttReceivedTopic != "") {
                    trace_info_P("Publishing %s to %s", buffer, mqttReceivedTopic.c_str());
                    int result = mqttClient.publish(mqttReceivedTopic.c_str(), 0, false, buffer);
                    lastPublishTime = millis();
                    if (!result) {
                        trace_error_P("Publish to %s returned %d", mqttReceivedTopic.c_str(), result);
                    }
                }
            } else {
                // Prepare error text
                char buffer[250];
                snprintf(buffer, sizeof(buffer), getServerMessage("unknownCommand", "Can't understart %s, send %s or %s or %s").c_str(), purgedMessage.c_str(), onCommand.c_str(), offCommand.c_str(), stateCommand.c_str());
                checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
                sendSms(buffer);
            }
        }
    } else {
        trace_info_P("Bad sender %s", receivedNumber);              // Sender not in allowed list
    }
}

// A SMS has just been sent
void sendSmsCallback(const char* receivedNumber, const char* receivedDate, const char* receivedMessage) {
    if (localEnterFlag) enterRoutine(__func__);
    updateWebServerData();
}

// Extract one SMS from queue and send it
void sendBufferedSMS(void) {
    if (localEnterFlag) enterRoutine(__func__);
    String message;
    String number;

    size_t pos = messageBuffer.indexOf('\255');
    if (pos != std::string::npos) {
        // We have other messages, extract first one and remove it
        message = messageBuffer.substring(0, pos);
        messageBuffer = messageBuffer.substring(pos + 1);
    } else {
        // This is the last message, load it and clear buffer
        message = messageBuffer;
        messageBuffer = "";
    }
    // Do the same with numberBuffer
    pos = numberBuffer.indexOf('\255');
    if (pos != std::string::npos) {
        number = numberBuffer.substring(0, pos);
        numberBuffer = numberBuffer.substring(pos + 1);
    } else {
        number = numberBuffer;
        numberBuffer = "";
    }
    setGsmState(getServerMessage("gsmSend", "Sending SMS"));
    // Trace
    trace_debug_P("Send SMS to %s: %s", number.c_str(), message.c_str());

    // Send SMS
    Sim7000Modem.sendSMS(number.c_str(), message.c_str());
}

//  Main SMS loop
void smsLoop(void) {
    // User part of loop
    if ((millis() - lastSmsLoopTime) >= 1) {                        // Wait at least 1 ms between sms loops
        Sim7000Modem.doLoop();
        if (smsReady != Sim7000Modem.smsReady) {
            smsReady = Sim7000Modem.smsReady;
            if (smsReady) {
                smsState = getServerMessage("smsConnected", "SMS network connected");
            } else {
                smsState = getServerMessage("smsDisconnected", "!!! SMS network *NOT* connected !!!");
            }
            updateWebServerData();
        }
        if (Sim7000Modem.needRestart()) {
            restartCount++;
            if (restartCount >= MAX_RESTART) {
                setGsmState("Restarting ESP");
                trace_info_P("%s after %d restart", gsmState.c_str(), restartCount);
                #ifdef FF_TRACE_USE_SERIAL
                    Serial.flush();
                #endif
                delay(1000);
                ESP.restart();
            }
            setGsmState(getServerMessage("gsmRestart", "Restarting GSM"));
            trace_error_P("%s, reason %d", gsmState.c_str(), Sim7000Modem.getRestartReason());
            Sim7000Modem.begin(SIM7000_MODEM_SPEED, gsmRxPin, gsmTxPin, gsmPowerPin);   // Start Sim7000 communication channel
        } else {
            if (Sim7000Modem.isIdle()) {
                if (messageBuffer != "") {
                    sendBufferedSMS();
                } else {
                    setGsmState(getServerMessage("gsmWait", "Waiting for SMS"));
                }
            }
        }
        #ifdef ESP32
            // Check remaining space
            uint16_t freeBlockSize = ESP.getMaxAllocHeap();
            if (freeBlockSize < 4096) {
                trace_error_P("Largest free block is %d - Restarting!!", freeBlockSize);
                #ifdef FF_TRACE_USE_SERIAL
                    Serial.flush();
                #endif
                delay(1000);
                ESP.restart();
            }
        #else
            // Check remaining space
            uint16_t freeBlockSize = ESP.getMaxFreeBlockSize();
            if (freeBlockSize < 4096) {
                trace_error_P("Largest free block is %d - Restarting!!", freeBlockSize);
                #ifdef FF_TRACE_USE_SERIAL
                    Serial.flush();
                #endif
                delay(1000);
                ESP.restart();
            }
        #endif
        lastSmsLoopTime = millis();
    }
}

// Return count of pending message
uint8_t pendingMessages(void) {
    uint8_t count = 0;
    if (numberBuffer != "") {
        count += 1;
        size_t ptr = numberBuffer.indexOf("\0");
        while (ptr >= 0) {
            count += 1;
            ptr = numberBuffer.indexOf("\0", ptr+1);
        }
    }
    return count;
}

//  ---- Relay ----

// Read relayState
bool readRelayState(void) {
    if (localEnterFlag) enterRoutine(__func__);
    File relayStateFile = LittleFS.open(RELAY_STATE_FILE, "r");     // Open relayState file
    if (!relayStateFile) {                                          // Error opening?
        trace_error_P("Failed to open relay state file", NULL);
        return false;
    }

    JsonDocument relayState;
    auto error = deserializeJson(relayState, relayStateFile);       // Read relayState
    relayStateFile.close();                                         // Close file
    if (error) {                                                    // Error reading JSON?
        trace_error_P("Failed to parse relay state file", NULL);
        return false;
    }

    // Load all relayState into corresponding variables
    relayActive = relayState["relayActive"].as<bool>();
    return true;
}

// Write relayState
void writeRelayState(void) {
    if (localEnterFlag) enterRoutine(__func__);
    JsonDocument relayState;

    // Load relayState in JSON
    relayState["relayActive"] = relayActive;

    File relayStateFile = LittleFS.open(RELAY_STATE_FILE, "w");     // Open relayState file
    if (!relayStateFile) {                                          // Error opening?
        trace_error_P("Can't open for write relay state file", NULL);
    }
    uint16_t bytes = serializeJsonPretty(relayState, relayStateFile);// Write JSON structure to file
    if (!bytes) {                                                   // Error writting?
        trace_error_P("Can't write relay state file", NULL);
    }
    relayStateFile.flush();                                         // Flush file
    relayStateFile.close();                                         // Close it
    if (traceCode) {
        trace_info_P("Relay state saved", NULL);
    }
}

// Sends a SMS with current relay state
void sendCurrentState(void) {
    sendSms(deviceName + " " + (relayActive? onState : offState));
}

// Update state on web server
void updateWebServerData(void) {
    // Flag update needed
    sendAnUpdateFlag = true;
}

// Send web server data to clients
void sendWebServerData(void) {
    if (localEnterFlag) enterRoutine(__func__);
    // Send new state to connected users
    JsonDocument data;
    data["relayState"] = relayActive? onState : offState;
    data["powerMode"] = powerMode.c_str();
    data["wifiState"] = wifiState.c_str();
    data["smsState"] = smsState.c_str();
    data["gsmState"] = gsmState.c_str();
    data["onButton"] = onButton.c_str();
    data["offButton"] = offButton.c_str();
    data["lastSentMessage"] = Sim7000Modem.lastSentMessage.c_str();
    data["lastSentDate"] = Sim7000Modem.lastSentDate.c_str();
    data["lastSentNumber"] = Sim7000Modem.lastSentNumber.c_str();
    data["lastReceivedMessage"] = Sim7000Modem.lastReceivedMessage.c_str();
    data["lastReceivedDate"] = Sim7000Modem.lastReceivedDate.c_str();
    data["lastReceivedNumber"] = Sim7000Modem.lastReceivedNumber.c_str();
    data["listeningNodes"] = listeningNodes.length() > 2 ? listeningNodes.substring(2, listeningNodes.length()).c_str(): getServerMessage("emptyData", "[Empty]");

    #ifdef ESP32
        char buffer[1024];
    #endif
    #ifdef ESP8266
        char buffer[256];
    #endif
    serializeJson(data, buffer, sizeof(buffer));
    checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
    events.send(buffer, "data");
    sendAnUpdateFlag = false;
}

// Signal connected user(s) a data change
void signalRelayChanged(const bool alsoSendSms) {
    if (localEnterFlag) enterRoutine(__func__);
    if (alsoSendSms) {
        // Send a SMS with relay state
        sendCurrentState();
    }

    // Save relay state to file
    writeRelayState();

    // Update state on web server
    updateWebServerData();

    // Set led state accordingly
    #ifdef LED_PIN
        #ifdef LED_INVERTED
            digitalWrite(LED_PIN, relayActive? LOW : HIGH);
        #else
            digitalWrite(LED_PIN, relayActive? HIGH : LOW);
        #endif
    #endif
}

// Relay setup
void relaySetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    // Set LED mode after turning LED off
    #ifdef LED_PIN
        #ifdef LED_INVERTED
            digitalWrite(LED_PIN, HIGH);
        #else
            digitalWrite(LED_PIN, LOW);
        #endif
        pinMode(LED_PIN, OUTPUT);
    #endif
    // Set initial value depending on options
    String startupState = startState;
    startupState.toLowerCase();
    if (startupState == "on") {
        relayActive = true;
    } else if (startupState == "off") {
        relayActive = false;
    } else {
        readRelayState();
    }
    // Set relay initial state
    if (reversedRelay) {
        digitalWrite(relayPin, relayActive? LOW : HIGH);
    } else {
        digitalWrite(relayPin, relayActive? HIGH : LOW);
    }
    pinMode(relayPin, OUTPUT);
    // Set proper states by don't send SMS
    signalRelayChanged(false);
}

// Used to activate relay
void activateRelay(void) {
    if (localEnterFlag) enterRoutine(__func__);
    relayActive = true;
    signalRelayChanged();
    digitalWrite(relayPin, reversedRelay? LOW : HIGH);
}

// Used to deactivate relay
void deactivateRelay(void) {
    if (localEnterFlag) enterRoutine(__func__);
    relayActive = false;
    signalRelayChanged();
    digitalWrite(relayPin, reversedRelay? HIGH : LOW);
}

// --- Battery routines ---

// Battery setup
void batterySetup(void) {
    if (localEnterFlag) enterRoutine(__func__);
    #ifdef BATTERY_EXISTS
        #ifdef ESP32
            esp_adc_cal_characteristics_t adc_chars;
            esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);    //Check type of calibration value used to characterize ADC
            if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
                trace_info_P("ADC eFuse Vref:%u mV", adc_chars.vref);
                vref = adc_chars.vref;
            } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
                trace_info_P("ADC two Point --> coeff_a:%umV coeff_b:%umV", adc_chars.coeff_a, adc_chars.coeff_b);
            } else {
                trace_info_P("ADC default Vref: 1100mV", NULL);
            }
        #endif
    #else
        powerMode = getServerMessage("onPower", "Connected on grid");
    #endif
}

#ifdef BATTERY_EXISTS
    // Read battery voltage
    float readBatteryVoltage(void) {
        uint16_t v = analogRead(batAdcPin);
        // When connecting USB, the battery detection will return 0,
        //  because the adc detection circuit is disconnected when connecting USB
        return ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    }

    uint8_t getBatteryPercent(const float batteryVolts) {
        return  min(100, max(0, (int) (100 * (batteryVolts-BATTERY_MIN_VOLTAGE/(BATTERY_MAX_VOLTAGE-BATTERY_MIN_VOLTAGE)))));
    }

    // Battery loop: ckeck for power on/off/battery charge
    //      Note that power must be stable for 3 consecutive times to be taken in account
    void batteryLoop(void) {
        unsigned long now = millis();
        String newPowerMode = "";

        if ((now - lastBatteryLoopTime) > BATTERY_LOOP_TIME) {
            lastBatteryLoopTime = now;
            float currentBatteryVoltage = readBatteryVoltage();
            // Debounce power present (battery voltage around zero if power is present)
            bool currentPowerPresent = (currentBatteryVoltage < 0.5);
            // Does the state change?
            if (currentPowerPresent != powerDebounceState) {
                // Clear debounce count
                powerDebounceCount = 0;
                // Save state
                powerDebounceState = currentPowerPresent;
            } else {
                // Increment count
                if (powerDebounceCount <= 2) {
                    powerDebounceCount += 1;
                    // If power state is constant for 2 loops
                    if (powerDebounceCount == 2) {
                        powerDebounceCount += 1;
                        // Change power present state
                        trace_debug_P("PowerPresent changed to %d, battery %f", currentPowerPresent, currentBatteryVoltage);
                        powerPresent = currentPowerPresent;
                        batteryVoltage = currentBatteryVoltage;
                    }
                } else {
                    // Keep saving battery if state is stable
                    batteryVoltage = currentBatteryVoltage;
                }
            }
            // Here, powerPresent has been debounced (stable for few loops)
            if (powerPresent) {
                // Voltage is null, USB is connected
                if (lastPowerLossTime) {
                    // Power was previously down, signal back
                    sendSms(getServerMessage("powerBack", "Power is restored").c_str());
                    lastPowerLossTime = 0;
                }
                newPowerMode = getServerMessage("onPower", "Connected on grid");
            } else {
                uint8_t batteryPercent = getBatteryPercent(batteryVoltage);
                char buffer[200];
                if (!lastPowerLossTime) {
                    // Power was previously present, signal down
                    snprintf(buffer, sizeof(buffer), getServerMessage("powerDown", "!!! Power has been lost - Battery loaded at %d%% !!!").c_str(), batteryPercent);
                    checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
                    sendSms(buffer);
                    // Save power lost time
                    lastPowerLossTime = now;
                    // Set initial battery signal percent
                    nextPercentMessage = BATTERY_SIGNAL_PERCENT;
                    // Decrease it until lower than current percentage
                    if (batteryPercent >= 1) {
                        while (nextPercentMessage >= batteryPercent) {
                            nextPercentMessage -= BATTERY_SIGNAL_STEP;
                        }
                    } else {
                        nextPercentMessage = 0;
                    }
                } else {
                    // Power was lost, are we under next low percentage?
                    if (batteryPercent <= nextPercentMessage) {
                        snprintf(buffer, sizeof(buffer), getServerMessage("powerStillDown", "!!! Still no power - Battery loaded at %d%% !!!").c_str(), batteryPercent);
                        checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
                        sendSms(buffer);
                        // Find next step under current percentage
                        if (batteryPercent >= 1) {
                            while (nextPercentMessage >= batteryPercent) {
                                nextPercentMessage -= BATTERY_SIGNAL_STEP;
                            }
                        } else {
                            nextPercentMessage = 0;
                        }
                    }
                }
                snprintf(buffer, sizeof(buffer), getServerMessage("powerDown", "!!! Power has been lost - Battery loaded at %d%% !!!").c_str(), batteryPercent);
                checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
                newPowerMode = String(buffer);
            }
            // Signal change is power mode has been updated
            if (powerMode != newPowerMode) {
                powerMode = newPowerMode;
                updateWebServerData();
            }
        }
    }
#endif

//          -------------------------------------
//          ---- Program initialization code ----
//          -------------------------------------

void setup(void) {
    localEnterFlag = true;
    logSetup();                                                     // Init log
    traceSetup();                                                   // Register trace
    #ifdef ESP8266
        Serial.begin(74880);
    #else
        Serial.begin(115200);
    #endif

    trace_info_P("Initializing %s V%s", __FILENAME__, VERSION);
    resetCause = getResetCause();                                   // Get reset cause

    #ifdef ESP32
        // Stop Blutooth
        btStop();
    #endif

    // Starts flash file system
    LittleFS.begin();

    // Load preferences
    if (!readSettings()) {
        trace_info_P("No settings, stopping !", NULL);
        while (true) {
            yield();
        }
    };

    batterySetup();                                                 // Start battery related stuff
    relaySetup();                                                   // Init relay

    WiFi.disconnect(false, true);                                   // Keep wifi on, erase NVS
    WiFi.hostname(espName.c_str());                                 // Define this module name for client network
    #ifdef ESP32
        WiFi.onEvent(onWiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(onWiFiStationGotIp, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    #endif
    #ifdef  ESP8266
        onStationModeConnectedHandler = WiFi.onStationModeConnected(&onWiFiStationConnected); // Declare connection callback
        onStationModeGotIPHandler = WiFi.onStationModeGotIP(&onWiFiStationGotIp); // Declare got IP callback
    #endif
    char buffer[80];
    snprintf(buffer, sizeof(buffer), getServerMessage("accessPointActive", "WiFi access point %s active").c_str(), ssid.c_str());
    checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
    wifiState = String(buffer);
    updateWebServerData();

    ssid.trim();
    if (ssid != "") {
        #ifdef ESP32
            WiFi.mode(WIFI_MODE_APSTA);                             // Set both AP & station
        #endif
        #ifdef ESP8266
            WiFi.setAutoReconnect(true);                            // Autoreconnect WiFi
            WiFi.mode(WIFI_AP_STA);                                 // Set both AP & station
        #endif
    }
    #ifdef ESP32
        WiFi.IPv6(false);                                           // Disable IP V6 for AP
    #endif

    // Load this Wifi access point name as ESP name plus ESP chip Id
    #ifdef ESP32
        snprintf_P(buffer, sizeof(buffer),PSTR("%s_%X"), espName.c_str(), getChipId());
    #else
        snprintf_P(buffer, sizeof(buffer),PSTR("%s_%X"), espName.c_str(), ESP.getChipId());
    #endif
    checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
    trace_info_P("Creating %s access point", buffer);
    WiFi.softAP(buffer, "");                                        // Starts Wifi access point
    trace_info_P("Connect to SSID %s with http://%s/ or http://%s/", buffer, WiFi.softAPIP().toString().c_str(), espName.c_str());

    // Start syslog
    syslogSetup();                                                  // Init log

    // OTA trace
    ArduinoOTA.onStart(onStartOTA);
    ArduinoOTA.onEnd(onEndOTA);
    ArduinoOTA.onError(onErrorOTA);

    //ArduinoOTA.setPassword("my OTA password");                    // Uncomment to set an OTA password
    ArduinoOTA.begin();                                             // Initialize OTA

    events.onConnect([](AsyncEventSourceClient *client){            // Routine called when a client connects
        // Set send an update flag
        sendAnUpdateFlag = true;
        JsonDocument data;
        data["relayState"] = relayActive? onState : offState;
        data["powerMode"] = powerMode.c_str();
        data["wifiState"] = wifiState.c_str();
        data["smsState"] = smsState.c_str();
        data["gsmState"] = gsmState.c_str();
        data["onButton"] = onButton.c_str();
        data["offButton"] = offButton.c_str();
        #ifdef ESP32
            char buffer[1024];
        #endif
        #ifdef ESP8266
            char buffer[256];
        #endif
        serializeJson(data, buffer, sizeof(buffer));
        checkFreeBufferSpace(__func__, __LINE__, "buffer", sizeof(buffer), strlen(buffer));
        client->send(buffer, "data");
        // Send last log lines
        for (uint16_t i=0; i < LOG_MAX_LINES; i++) {
            String logLine = getLogLine(i, true);
            if (logLine != "") {
                client->send(logLine, "info");
            }
        }
    });

    // List of URL to be intercepted and treated locally before a standard treatment
    //  These URL can be used as API
    webServer.on("/status", HTTP_GET, statusReceived);              // /status request
    webServer.on("/setup", HTTP_GET, setupReceived);                // /setup request
    webServer.on("/command", HTTP_GET, commandReceived);            // /command request
    webServer.on("/languages", HTTP_GET, languagesReceived);        // /languages request
    webServer.on("/settings", HTTP_GET, settingsReceived);          // /settings request
    webServer.on("/debug", HTTP_GET, debugReceived);                // /debug request
    webServer.on("/rest", HTTP_GET, restReceived);                  // /rest request
    webServer.on("/log", HTTP_GET, logReceived);                    // /log request

    // These URL are used internally by setup.htm - Use them at your own risk!
    webServer.on("/changed", HTTP_GET, setChangedReceived);         // /changed request
    webServer.addHandler(&events);                                  // Define web events
    webServer.addHandler(new LittleFSEditor());                     // Define file system editor
    webServer.onNotFound (notFound);                                // To be called when URL is not known
    webServer.serveStatic("/",LittleFS, "/").setDefaultFile("index.htm"); // Serve "/", default page = index.htm

    webServer.begin();                                              // Start Web server

    if (ssid != "") {                                               // If SSID is given, try to connect to
        trace_info_P("Connecting to %s ", ssid.c_str());
        WiFi.begin(ssid.c_str(), pwd.c_str());                      // Start to connect to existing SSID
        uint16_t loopCount = 0;
        while (WiFi.status() != WL_CONNECTED && loopCount < 30) {   // Wait for connection for 30 seconds
            delay(1000);                                            // Wait for 1 s
            loopCount++;
        }                                                           // Loop
        if (WiFi.status() == WL_CONNECTED) {                        // If we're not connected
            trace_info_P("Connect to SSID %s with http://%s/ or http://%s/ ", ssid.c_str(), WiFi.localIP().toString().c_str(), espName.c_str());
            WiFi.softAPdisconnect(false);                           // Disable AP, don't disable WiFi
        } else {
            trace_info_P("Not yet connected, continue trying...", NULL);
        }
    }

    trace_info_P("Initializing %s V%s", __FILENAME__, VERSION);
    trace_info_P("Reset cause: %s", resetCause.c_str());
    timeSetup();
    smsSetup();
    mqttSetup();
    startupDone = true;
}

//      ------------------------
//      ---- Permanent loop ----
//      ------------------------

void loop(void) {
    #ifdef BATTERY_EXISTS
        batteryLoop();                                              // Battery loop code
    #endif
    smsLoop();                                                      // Work with SMS
    ArduinoOTA.handle();                                            // Give hand to OTA
    #ifdef SERIAL_COMMANDS
        serialLoop();                                               // Scan for serial commands
    #endif
    // Send an update to clients if needed
    if (sendAnUpdateFlag) {
        sendWebServerData();                                        // Send data to clients
    }
    delay(1);                                                       // Slow down a bit main loop
}