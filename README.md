# SMS Web server based on ESP32/SIM7xxx with optional relay/Serveur SMS Web à base d'ESP32/SIM7xxx avec relai optionel

[English and French versions in the same document]
[Versions françaises et anglaises dans le même document]

## What's that?/A quoi ça sert?

This module implements a Web server connected to a SIM7xxx GSM module allowing sending and receiving SMS.

When an optional relay in installed, it can be locally (HTTP and API) and remotely (through SMS commands) switched.

With a battery, it can also detect and signal power failure and restoration by SMS.

It can also be connected to MQTT and/or Serial in order to read SMS to be sent and write those received.

Setup is done using an embeded web server, either through a local access point or connected to an existing one.

Log can be requested by API, and/or sent to a Syslog server.

Time can be provided either through GSM network or NTP server.

A Python example is provided, to receive system commands through SMS and send back command's log by SMS or mail.

A Domoticz plug-in is also provided at https://github.com/FlyingDomotic/domoticz-ff_smsserver-plugin

Ce module implémente un serveur Web connecté à modem GSM de type SIM7xxx pour émettre et recevoir des SMS.

Lorsqu'un relai optionnel est installé, il peut être commandé localement (HTTP et API) et à distance (par SMS)

Si une batterie est connectée, il peut aussi détecter et signaler par SMS une coupure et un retour du secteur.

Il peut également être connecté à un serveur MQTT et/ou un lien série pour lire les SMS à envoyer et écrire ceux reçus.

Le paramétrage est réalisé grâce à un serveur web embarqué, au travers d'un point d'accès local ou d'un WiFi existant.

Les traces peuvent être récupérées par une API, et/ou envoyées à un serveur Syslog.

Date et heure peuvent être fournis par le réseau GSM ou par un serveur NTP.

Un exemple en Python permet de recevoir des commandes système par SMS et de renvoyer les logs par SMS ou mail?

Un plug-in Domoticz est disponible à https://github.com/FlyingDomotic/domoticz-ff_smsserver-plugin

## Prerequisites/Prérequis

Can be used directly with Arduino IDE or PlatformIO./Peut être utilisé directement avec l'IDE Arduino ou PlatforIO.

The following libraries are used/Les libraires suivantes sont utilisées:
- https://github.com/ESP32Async/ESPAsyncWebServer
- https://github.com/bblanchon/ArduinoJson.git
- https://github.com/FlyingDomotic/FF_Trace
- https://github.com/FlyingDomotic/FF_SIM7000
- https://github.com/mgaman/PDUlib
- https://github.com/arcao/Syslog
- https://github.com/fbiego/ESP32Time
- https://github.com/marvinroger/async-mqtt-client

## Installation

Clone repository somewhere on your disk
```
cd [where_you_want_to_install_it]
git clone https://github.com/FlyingDomotic/FF_SmsServer32.git FF_SmsServer32
```

Cloner le dépot quelque part sur votre disque
```
cd [là_où_on_veut_l'installer]
git clone https://github.com/FlyingDomotic/FF_SmsServer32.git FF_SmsServer32
```

## Update/Mise à jour

Go to code folder and pull new version:
```
cd [where_you_installed_FF_SmsServer32}
git pull
```

Note: if you did any changes to files and `git pull` command doesn't work for you anymore, you could stash all local changes using:
```
git stash
```
or
```
git checkout [modified file]
```

Warning: as files in data folder should be downloaded on ESP file system, it may be a good idea to check if some of them have been changed be a new version, to update them on your ESP.

Remind also that settings.json is specific to your installation. Don't forget to download it from your ESP to data folder before making a global file system upload.

Aller dans le répertoire où le code a été installé et récupérer la nouvelle version :
```
cd [là_où_on_a_installé_FF_SmsServer32]
git pull
```

Note: si on a modifié des fichiers et que la commande `git pull` se plaint, il est possible d'écraser les modifications par :
```
git stash
```
or
```
git checkout [nom_du_fichier_modifié]
```

Attention: comme les fichiers du répertoire data doivent être téléchargés dans le système de fichier de l'ESPystem, il peut être habile de vérifier si une nouvelle version existe, afin de mettre à jour son ESP.

Penser aussi que le fichier settings.json est spécifique à son installation. Bien penser à le télécharger depuis l'ESP vers le répertoire data avant une mise à jour globale du système de fichiers.

## Hardware/Matériel
- ESP32 with SIM7XXX series GSM board (either LilyGo T SIM7070 or individual components)
- Optional relay
- Optional battery
 
 - ESP32 avec carte GSM de la série SIM7XXX (LilyGo T SIM7070 ou composants indépendants)
 - Relai optionnel
 - Batterie optionnelle
 
## Connections/Connexions

By default, the following pins are defined in preference.h file:

Par défaut, les pinoches suivantes sont définies dans le fichier preference.h :

| Name / Nom     | ESP32 Pins |
| -------------- | ---------- |
| Modem DTR      | 25         |     (not used/non utilisé)
| Modem Rx       | 26         |
| Modem Tx       | 27         |
| Modem PowerKey | 4          |
| BAT ADC        | 35         |
| -------------- | ---------- |
| SOLAR ADC      | 36         |     (not used/non utilisé)
| SD MISO        | 2          |     (not used/non utilisé)
| SD MOSI        | 15         |     (not used/non utilisé)
| SD CLK         | 14         |     (not used/non utilisé)
| SD CS          | 13         |     (not used/non utilisé)
| LED            | 12         |

| Name / Nom     | SIM70xx Pins           |
| -------------- | -----------------------|
| GPS (on modem) | SIM7070(GPIO4)         |     (not used/non utilisé)
|                | SIM7070(GPIO5)         |     (not used/non utilisé)
|                | Physical pin number 48 |     (not used/non utilisé)

## Note:

ESP32 may reboot between USB power loss and before battery gets back. In this case, solder a 100uF capacitor between GND and 3.3V rail (your configuration may need a larger capacitor).
 
L'ESP32 peut redémarrer entre la disparition de l'alimentation USB et le retour de la batterie. Dans ce cas, souder un condensateur de 100 uF entre la masse et le 3.3V (votre configuration peut necéssiter un condensateur plus gros).

## Remark/Remarque:

Code incorporate references to ESP8266. As of now, only basic functions have been tested. In particular, no tests have been done with SIM7xxx modem, as I did only have soldered with an ESP32. Be anyway aware that memory is limited on ESP8266, and OOM messages are likely to occur. However, feedbacks and change requests are welcome!
 
 Le code contient des références à l'ESP8266. A ce jour, seules les fonctions le plus basiques ont été testées. En particulier, aucun test n'a été réalisé avec un modem SIM7xxx, le seul que je possède est soudé sur une carte avec un ESP32. Pas de faux espoirs, la mémoire est limitée sur un ESP8266, et les messages "plus de mémoire disponible" très probables. Cependant, les retours et les demandes de modifications sont bienvenus !

## How to send a SMS?/Comment envoyer un SMS ?

You may send a SMS:
- making an HTTP request at `http://[smsserver name or ip]/rest/send&number=[phone number]&message=[message to send]`. As this is an HTTP request, elements have to be escaped as usual (i.e replace spaces by +, special characters by their equivalents).
- sending `{"number":"[phone number of target]", "message":"[Message to send outside]"}` to MQTT topic `mqttGetTopic` (probably something like `smsServer/toSend`)
- sending messages like `{"number":"[phone number of target]", "message":"[Message to send outside]"}` to Serial. Answer format will be `{"status":"[submission status]"}`.

On peut envoyer un SMS :
- par une requête HTTP à `http://[nom ou IP du serveur SMS]/rest/send&number=[numéro de téléphone]&message=[message à envoyer]`. Etant une requête HTTP, les éléments sont à spécifier comme d'habitude (par exemple, remplacer les espaces par des +, et les caractères spécieux par leur équivalent en hexadécimal).
- en envoyant `{"number":"[numéro de téléphone]", "message":"[message à envoyer]"}` au sujet MQTT `mqttGetTopic` (probablement quelque chose comme `smsServer/toSend`)
- en envoyant `{"number":"[numéro de téléphone]", "message":"[message à envoyer]"}` sur le port série. La réponse seau au format `{"status":"[état de la soumission]"}`.

## How to get a SMS/Comment recevoir un SMS ??

You may get a copy of each received message by:
- subscribing to MQTT `mqttSendTopic` (probably something like `smsServer/received`). Received message format is `{"number":"[phone number of sender]", "date":"[receive date]", "message":"[Message sent to SMS server]"}`.
- scanning Serial for messages like `{"number":"[phone number of sender]", "date":"[receive date]", "message":"[Message sent to SMS server]"}`

Il est possible de recevoir une copie des SMS reçus en :
- s'abonnant au sujet MQTT `mqttSendTopic` (propablement `smsServer/received`). Le format des messages reçus est `{"number":"[téléphone de l'émetteur]", "date":"[date de réception]", "message":"[Message envoyé au serveur SMS]"}`.
- en repérant sur le port série des messages au format `{"number":"[téléphone de l'émetteur]", "date":"[date de réception]", "message":"[Message envoyé au serveur SMS]"}`.

## Parameters at compile time/Paramètres définis au moment de la compilation

The following parameters are used at compile time:
- FF_SIM7000_USE_SERIAL1: (default=defined) SIM7000 is connected on Serial1
- FF_SIM6000_SET_TIME_FROM_GSM_NETWORK: (default=defined) use time provided by GSM network (optional)
- SERIAL_COMMANDS: (default=defined) use serial commands (optional)
- SEND_SMS_FROM_SERIAL: (default=defined) allow sending SMS from serial (optional)
- PRINT_RECEIVED_SMS_ON_SERIAL: (default=defined) print received messages on Serial (optional)
- LED_PIN: (defaultin preferences.h=12) define pin where relay LED is connected to (optional)
- LED_INVERTED: (default in preferences.h=defined) relay LED command levels are inverted (light when LOW) (optional)
- FF_TRACE_NO_SOURCE_INFO: (default=defined) suppress file name, module name and ine number of traces (optional)
- FF_TRACE_KEEP_ALIVE: (default=5 * 60 * 1000) Trace keep alive timer (optional)
- FF_TRACE_USE_SYSLOG: (default=defined) Send trace messages on Syslog (optional)
- FF_TRACE_USE_SERIAL: (default=defined) Send trace messages on Serial (optional)
- FF_DISABLE_DEFAULT_TRACE: (default=not defined) Disable default trace callback (optional)

Les paramètres suivants sont définis au moment de la compilatiton :
- FF_SIM7000_USE_SERIAL1: (défaut=defined) SIM7000 est connecté sur Serial1
- FF_SIM6000_SET_TIME_FROM_GSM_NETWORK: (défaut=defined) utiliser le temps donné par le réseau GSM (optionnel)
- SERIAL_COMMANDS: (défaut=defined) utiliser les commandes passées sur le lien série (optionnel)
- SEND_SMS_FROM_SERIAL: (défaut=defined) autoriser l'envoi de SMS depuis le lien série (optionnel)
- PRINT_RECEIVED_SMS_ON_SERIAL: (défaut=defined) afficher les messages reçus sur le lien série (optionnel)
- LED_PIN: (défautin preferences.h=12) définit la pinoche sur laquelle la LED de répétition du relais (optionnel)
- LED_INVERTED: (défaut in preferences.h=defined) les niveaux de la LED de répétition du relais sont inversés (allumé au niveau bas) (optionnel)
- FF_TRACE_NO_SOURCE_INFO: (défaut=defined) supprime le nom de fichier, de mudule et le numéro de ligne des traces (optionnel)
- FF_TRACE_KEEP_ALIVE: (défaut=5 * 60 * 1000) Délai de maintient en vie de la trace (optionnel)
- FF_TRACE_USE_SYSLOG: (défaut=defined) Envoie les messages de trace sur syslog (optionnel)
- FF_TRACE_USE_SERIAL: (défaut=defined) Envoie les messages de trace sur le lien série (optionnel)
- FF_DISABLE_DEFAULT_TRACE: (défaut=not defined) Désactive le code de trace par défaut (optionnel)

## Parameters defined at run time/Paramètres définis lors de l'exécution

The following parameters can be defined, either in json files before loading LittleFS file system, or through internal http server.

Les paramètres suivants peuvent être définis, soit dans le fichier json avant le chargement du système de fichiers, où au travers du serveur HTTP interne.

### In settings.json/Dans settings.json

- ssid: (default="") SSID of existing WiFi to connect to (module will create a local access point if empty)
- pwd: (default="") password of existing WiFi to connect to (no password if empty)
- name: (default="smsServer") network name of ESP module
- traceDebug: (default=true) activate debug traces in log
- traceCode: (default=true) activate code traces in log
- traceJava: (default=false) activate debug java traces in java console
- serverLanguage: (default="en") language to use for module user's messages (can be "en" or "fr", you can add your own creating a lang_xx.json file in flash from either lang_fr.json or lang_en.json and translate values in your language)
- deviceName: (default="My device") device name displayed on HTTP and in SMS messages
- onCommand: (default="on") command to send to turn device on
- offCommand: (default="off") command to send to turn device off
- stateCommand: (deault="state") command to send to receive device state
- onButton: (default="Turn on") name to display into "on" button
- offButton: (default="Turn off") name to display into "off" button
- onState: (default "is on") text to display after device name when it is on
- offState": (default "is off") text to display after device name when it is off
- startState: (default="previous") device state to set when module reboots (can be either "on", "off" or "previous")
- allowedNumbers: (default="+33123456789") comma separated list of phone numbers allowed to send SMS to server
- syslogServer: (default="") name or IP of syslog server
- syslogPort: (default=514) port of syslog server
- ntpServer: (default= "") name or IP of NTP server to use
- ntpParameters: (default="CET-1CEST,M3.5.0,M10.5.0/3") specification of time zone (should be sepcified when using either NTP or GSM network for time synchronization)
- mqttServer: (default="") name or IP of MQTT server
- mqttPort: (default=1883) port of MQTT server
- mqttUser: (default="") user name to use to identify on MQTT server
- mqttPasw: (default="") password to use to identify on MQTT server
- mqttSendTopic: (default="smsServer/toSend") MQTT topic for SMS server to read SMS to send
- mqttReceivedTopic: (default="smsServer/received") MQTT topic for SMS server to write SMS received
- mqttLwtTopic: (default="smsServer/LWT") Root topic for SMS server to write it's state, and base topic to read connected node state

- ssid: (défaut="") SSID du réseau WiFi existant sur lequel se connecter (le module va créer son propore point d'accès si vide)
- pwd: (défaut="") mot de passe du réseau WiFi existant sur lequel se connecter (pas de mot de passe si vide)
- name: (défaut="smsServer") nom réseau de l'ESP
- traceDebug: (défaut=true) active les traces de déverminage dans le log
- traceCode: (défaut=true) active les traces du code dans le log
- traceJava: (défaut=false) active les traces de déverminage Java dans la console Java
- serverLanguage: (défaut="en") langue ) utiliser pour les messages utilisateurs (peut être "en" ou "fr", vous pouvez créer votre propre langue en créant un fichier lang_xx.json dans la flash depuis lang_fr.json ou lang_en.json et les traduire dans votre langue)
- deviceName: (défaut="Mon dispositif") nom du dispositif affiché dans HTTP et dans les messages SMS
- onCommand: (défaut="on") commande à envoyer pour activer le dispositif
- offCommand: (défaut="off") commande à envoyer pour désactiver le dispositif
- stateCommand: (défaut="state") commande à envoyer pour afficher l'état du dispositif
- onButton: (défaut="Turn on") texte à afficher dans le bouton d'activation
- offButton: (défaut="Turn off") texte à afficher dans le bouton de désactivation
- onState: (défaut "is on") texte à afficher derrière le nom du dispositif lorsqu'il est actif
- offState": (défaut "is off") texte à afficher derrière le nom du dispositif lorsqu'il est inactif
- startState: (défaut="previous") état du dispositif lorsque le module redémarre (peut être "on", "off" ou "previous")
- allowedNumbers: (défaut="+33123456789") liste des numéros de téléphone (séparés par des virgules) autorisés à envoyer des SMS au serveur
- syslogServer: (défaut="") nom ou adresse IP du serveur syslog
- syslogPort: (défaut=514) numéro de port du serveur syslog
- ntpServer: (défaut= "") nom ou adresse IP du serveur NTP
- ntpParameters: (défaut="CET-1CEST,M3.5.0,M10.5.0/3") définition du fuseau hoarire (doit être renseigné lorqu'on utilise un serveur NTP ou le réseau GSM pour synchroniser le temps)
- mqttServer: (défaut="") nom ou adresse IP du serveur MQTT
- mqttPort: (défaut=1883) numero de port du serveur MQTT
- mqttUser: (défaut="") utilisateur à utiliser pendant l'autentification au serveur MQTT
- mqttPasw: (défaut="") mot de passe à utiliser pendant l'autentification au serveur MQTT
- mqttSendTopic: (défaut="smsServer/toSend") sujet MQTT utilisé par le serveur SMS pour lire les messages SMS à envoyer
- mqttReceivedTopic: (défaut="smsServer/received") sujet MQTT ou le serveur SMS écrit les SMS reçus
- mqttLwtTopic: (défaut="smsServer/LWT") Sujet MQTT racine où le serveur SMS écrit son état, et sujet de base où on lit l'état des noeuds connectés

## Available URLs/URL disponibles

WebServer answers to the following URLs:
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

Le serveur Web répond aux URL suivantes :
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

## Debug commands/Commandes de déverminage

Debug commands are available to help understanding what happens, and may be a good starting point to help troubleshooting.

Des commandes de déverminage sont disponibles pour aider à comprendre ce qui se passe, et sont un bon point de départ pour aider au diganostic.

### Access/Accès

Debug is available sending the command on Serial and/or MQTT (if MQTT server is defined)
Debug output is available on Serial and Syslog. Note that settings can disable some of these outputs.

Le déverminage est disponible en envoyant des commandes sur le port série et/ou par MQTT (si le serveur MQTT est défini)
Debug output is available on Serial and Syslog. Note that settings can disable some of these outputs.

### Commands/Commandes

The following commands are allowed:
- ATxxx: send AT command to modem
- send xxx: send xxx SMS message to all allowed phone numbers
- show modem: display modem status
- enable local debug: enable code debug flag
- disable local debug: diable code debug flag
- enable local trace: enable code trace flag
- disable local trace: disable code trace flag
- enable local enter: enable code routine entering flag
- disable local enter: disable code routine entering flag
- enable modem debug: enable SIM7000 modem debug flag
- disable modem debug: diable SIM7000 modem debug flag
- enable modem trace: enable SIM7000 modem trace flag
- disable modem trace: disable SIM7000 modem trace flag
- enable modem enter: enable SIM7000 modem routine entering flag
- disable modem enter: disable SIM7000 modem routine entering flag
- any other command will display short help

Les commandes suivantes sont disponibles :
- ATxxx: envoie une commande AT au modem
- send xxx: envoie le SMS xxx à l'ensemble des numéros de téléphone autorisés
- show modem: affiche l'état du modem
- enable local debug: active le déverminage du code
- disable local debug: désactive le déverminage du code
- enable local trace: active la trace du code
- disable local trace: désactive la trace du code
- enable local enter: active la trace des entrées dans les fonctions du code
- disable local enter: désactive la trace des entrées dans les fonctions du code
- enable modem debug: active le déverminage du modem SIM7xxx
- disable modem debug: désactive le déverminage du modem SIM7xxx
- enable modem trace: active la trace du modem SIM7xxx
- disable modem trace: désactive la trace du modem SIM7xxx
- enable modem enter: active la trace des entrées dans les fonctions du modem SIM7xxx
- disable modem enter: désactive la trace des entrées dans les fonctions du modem SIM7xxx
- toute les autres commandes afficheront un message d'aide

## MQTT topics/Sujets MQTT

The following MQTT topics are used:

Les sujets MQTT suivants sont utilisés :

### mqttSendTopic

MQTT topic used by SMS server to write received SMS to
For example: `smsServer/received`
Format: `{"number":"[phone number of sender]", "date":"[receive date"], "message":"[Message sent to SMS server]"}`

Sujet MQTT utilisé par le serveur SMS pour écrire les SMS reçus
Par exemple: `smsServer/received`
Format: `{"number":"[numéro de téléphone de l'émmeteur]", "date":"[date de réception"], "message":"[Message reçu par le serveur SMS]"}`

### mqttGetTopic

MQTT topic used by SMS server to read SMS messages to send to external destination
For example: `smsServer/toSend`
Format: `{"number":"[phone number to send message to]", "message":"[Message to send]"}`

Sujeet MQTT utilisé par le serveur SMS pour lire les SMS à envoyer à l'extérieur
Par exemple: `smsServer/toSend`
Format: `{"number":"[numéro de téléphone du destinatire]", "message":"[Message à envoyer]"}`

### mqttLwtTopic

mqttLwtTopic will be used to:
- write SMS server LWT data (at root level)
- read applications status (on sub level). For example: `smsServer/LWT` (will be followed by node/application name, i.e. `smsServer/LWT/myNode`). Format: `{"state":"[State of node/application]"}`, probably "up" or "down", displayed on main Web server page, and returned to answer of `/rest/listening` request

mqttLwtTopic est utilisé pour :
- écrire l'état du serveur SMS (au niveau de la racine)
- lire l'état des application aux niveaux inférieurs. Par exemple  : `smsServer/LWT` (suivi du nom de noeuds ou d'application, comme `smsServer/LWT/myNode`). Format: `{"state":"[Etat du npeud/de l'application]"}`, certainement "up" ou "down", affiché sur la page d'accueil, et retourné par l'appel à la page `/rest/listening`

### mqttCommandTopic

MQTT topic to read debug commands to be executed
For example: `smsServer/command`
Format: `[Command to be executed]`

Topic MQTT où lire les commands à exécuter
Par exemple: `smsServer/command`
Format: `[Commande à exécuter]`

## Associated files/Fichiers associés

Here's list of associated files

Voici la liste des fichiers assocés

### examples/readSms.py

This example reads SMS and send them back to receiver, prefixing them with "Received :".
Basically useless, but a good starting point to be used as example for your own code.

Cet exemple list des SMS et lres renvoie à l'expéditeur, en les préfixant par "Received :".
Pas très utile, mais un bon point de départ pour être utilisé comme exemple pour son propre code..

### examples/smsHandler.py

Reads received SMS through MQTT, to isolate messages starting with this node name. 
When found, rest of message is executed as OS local command.
Result, output and errors are then sent back by mail.
If answer is shorter than 70 characters, it will be also sent back by SMS.
Else result code will be sent back to sender.
Traces are kept in a log file, rotated each week.

MQTT and mail settings are to be set into file before running it, probably as a service.

Lit les SMS reçu par MQTT, en isolant ceux qui commencent par le nom de noeud de la machine sur laquelle il tourne.
Si le messages est pour ce noeud, il est exécuté comme commande système locale.
Résultat et erreurs sont envoyés par SMS.
Si la réponse est inférieure à 70 caractères, elle sera également envoyée par mail.
Sinon, le statut de la commande sera envoy par SMS.
Les traces soont sauvegardées dans un ficher log, renouvellé chaque semaine.

Les paramètres MQTT et mail sont à définir dans un fichier avant de lancer le script, certainement en tant que service.

#### examples/smsHandler.service

Service definition for examples/smsHandler.py.

Définition du service pour le script examples/smsHandler.py.

#### How to install smsHandler.service?/Comment installer smsHandler.service ?
- copy smsHandler.service, smsHandler.py and smsServerParameters.json [where ever you want]
- cd [where ever you want]
- chmod +x *.py
- nano smsHandler.service
	- locate `User=` line and replace `pi` by user you want to run smsHandler.service
	- locate `ExecStart=` line and replace `/home/pi` by location where you installed smsHandler.service
	- save modified file
- sudo mv smsHandler.service /lib/systemd/system/
- sudo chmod 644 /lib/systemd/system/smsHandler.service
- sudo systemctl enable smsHandler.service
- sudo systemctl start smsHandler.service

- copier smsHandler.service, smsHandler.py et smsServerParameters.json [là où on veut]
- cd [là où on veut]
- chmod +x *.py
- nano smsHandler.service
	- trouver la ligne`User=` et remplacer `pi` par le nomd'utilisateur qui va lancer smsHandler.service
	- trouver la ligne `ExecStart=` et remplacer `/home/pi` par le répertoir où smsHandler.service est install"
	- Enregistrer le fichier modifié
- sudo mv smsHandler.service /lib/systemd/system/
- sudo chmod 644 /lib/systemd/system/smsHandler.service
- sudo systemctl enable smsHandler.service
- sudo systemctl start smsHandler.service

### examples/smsServerTest.py
Check if SMS server is working correctly. It sends an SMS (using SMS server) to itself, and checks if it receive it back within a minute. If not, it sends a mail with error, and if smsServerRestartUrl is defined, sends a restart to SMS server.

Vérifie si le serveur SMS fonctionne correctement. Il s'envoie un SMS (en utilisant le serveur SMS) et vérifie qu'il reçoit bien les SMS qu'il s'est auto-émis dans la minute. Sinon, il envoie un mail avec l'erreurn et si smsServerRestartUrl est défini, envoie une demande de rédémarrage au serveur SMS.

### examples/smsServerTest.json
JSON configuration file for examples/smsServerTest.py. Contains the following lines:
- "mqttServer": IP address or name of MQTT server
- "mqttPort": Port number of MQTT server (often 1883)
- "mqttUser": Username to use with MQTT server (or empty if no username needed)
- "mqttPassword": Password of MQTT server (or empty if no password needed)
- "mqttReceiveTopic": SMS server receive topic (default to "smsServer/received")
- "mqttSendTopic": SMS server send topic (default to "smsServer/toSend")
- "mailSender": mail address of sender/receiver 
- "mailServer": IP address or name of mail server
- "smsServerNumber": SMS server phone number. Note that this number should be in authorized phone list in SMS server parameters
- "smsServerRestartUrl": SMS server restart URL (like "http://<IP address or name of SMS server>/rest/restart") or empty, if no restarted requested.

Fichier de configuration JSON pour examples/smsServerTest.py. Contient les lignes suivantes :
- "mqttServer": Nom ou adresse IP du server MQTT
- "mqttPort": numéro de port du serveur MQTT (souvent 1883)
- "mqttUser": Nom d'utilisateur pour l'autentification au serveur MQTT (vide si pas nécessaire)
- "mqttPassword": Mot de passe pour l'autentification au serveur MQTT (vide si pas nécessaire)
- "mqttReceiveTopic": Topic MQTT où le serveur SMS écrit les SMS reçus(par défaut "smsServer/received")
- "mqttSendTopic": Topic MQTT où le serveur SMS lit les SMS à émettre (par défaut "smsServer/toSend")
- "mailSender": adresse mail d'émission/réception
- "mailServer": adresse IP ou nom du serveur de mail
- "smsServerNumber": numéro de téléphone du serveur de SMS. Noter que ce numéro doit être dans la liste des numéros autorisés du serveur SMS.
- "smsServerRestartUrl": URL de redémarrage du serveyr SMS (ldu genre "http://<adresse IP ou nom du serveur SMS>/rest/restart") ou vide si aucune redémarrage n'est souhaité.
