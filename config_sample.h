/* 
    Create a script on scripts.google.com to fetch your calendar requests. The script is included in the github project.
*/

// WiFi
// Enter your SSID and your Password
const char WIFI_SSID[] = "";
const char WIFI_PASSWORD[] = "";

// Get calendar
// Enter your google API-Key from the public script
const String googleAPI = "";
const String calendarRequest = "https://script.google.com/macros/s/" + googleAPI + "/exec"; //do not change!

// Time Settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 0;

/* 
    No changes from here on
*/