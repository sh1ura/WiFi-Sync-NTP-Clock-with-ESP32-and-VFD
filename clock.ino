#include <WiFi.h>          
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>          //　Install WiFiManager by tzapu using Library Manager of Arduino IDE.
#include <Preferences.h>

// switch between 24H (12 rotors) / 12H (10 rotors)
#define HOUR12 false

// NTP settings
// find the name of timezone from https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
char timeZone[40] = "JST-9";
char ntpServer[40] = "pool.ntp.org";

#define SEG_N 8
#define GRID_N 6
int segment[SEG_N] = {32, 12, 21, 16, 22, 25, 27, 17}; // GPIO ports of 7-segment anodes
int grid[GRID_N] = {26, 18, 19, 23, 5, 13}; //GPIO ports of the grid of each digit

int content[GRID_N]; // on / off of each segment

int numeric[10] = { // 7-segment font from '0' to '9'
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

//////////////////////////////
// WiFi and NTP section
//////////////////////////////

void getNTP(void) {
  for(int i = 0; WiFi.status() != WL_CONNECTED; i++) {
    if(i > 30) {
      ESP.restart();
    }
    Serial.println("Waiting for WiFi connection..");
    delay(1000);
  }

  configTime(0, 0, ntpServer);
  setenv("TZ", timeZone, 1);
  tzset();

  printLocalTime();
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %Y-%m-%d %H:%M:%S");
}

bool shouldSaveConfig = false;
void saveConfigCallback() {
  shouldSaveConfig = true;
}

void wifiSetup() {
  content[0] = 0;
  content[1] = 0x38; // L
  content[2] = 0x77; // A
  content[3] = 0x37; // N
  content[4] = 0;
  content[5] = 0;

  WiFiManager wifiManager;
  Preferences preferences;

  //reset saved settings
  //wifiManager.resetSettings();

  preferences.begin("ntp_config", false);
  strcpy(ntpServer, preferences.getString("ntp_server", ntpServer).c_str());
  strcpy(timeZone, preferences.getString("time_zone", timeZone).c_str());

  WiFiManagerParameter ntp_server("ntp_server", "NTP server", ntpServer, 40);
  WiFiManagerParameter time_zone("time_zone", "Time zone", timeZone, 40);
  wifiManager.addParameter(&ntp_server);
  wifiManager.addParameter(&time_zone);

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setTimeout(180);
  wifiManager.autoConnect("VFD Clock");

  strcpy(ntpServer, ntp_server.getValue());
  strcpy(timeZone, time_zone.getValue());

  if (shouldSaveConfig) {
    preferences.putString("ntp_server", ntpServer);
    preferences.putString("time_zone", timeZone);
  }

  //if you get here you have connected to the WiFi
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

hw_timer_t *timer = NULL;

// dynamic drive of VFD display
void IRAM_ATTR onTimer(void) {
  static int currentGrid;

  digitalWrite(grid[currentGrid], LOW);

  currentGrid = (currentGrid + 1) % GRID_N;
  for(int s = 0; s < SEG_N; s++) {
    digitalWrite(segment[s], (content[currentGrid] >> s) & 0x01 ? HIGH : LOW);
  }
  digitalWrite(grid[currentGrid], HIGH);
}

void setup() {
  Serial.begin(115200);
  Serial.println("start");

  for(int g = 0; g < GRID_N; g++) {
    pinMode(grid[g], OUTPUT);
  }
  for(int s = 0; s < SEG_N; s++) {
    pinMode(segment[s], OUTPUT);
  }

  timer = timerBegin(1000000L);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000, true, 0); // 1ms for each digit * 6 digit = 6ms cycle (167 Hz dynamic drive)

  wifiSetup();
  getNTP(); // get current time
}

void loop() {
  static int prevSec = -1;
  struct tm tmtime;
  static unsigned long ms;
  bool blink;

  getLocalTime(&tmtime);

  if(prevSec != tmtime.tm_sec) {
    prevSec = tmtime.tm_sec;
    ms = millis();
  }
  blink = (millis() - ms > 500) ? false : true;

#if HOUR12
  tmtime.tm_hour %= 12;
  if(tmtime.tm_hour == 0)
    tmtime.tm_hour = 12;
#endif

  content[0] = numeric[tmtime.tm_hour / 10];
  content[1] = numeric[tmtime.tm_hour % 10];
  content[2] = numeric[tmtime.tm_min / 10];
  content[3] = numeric[tmtime.tm_min % 10] + (blink ? 0x80 : 0); // blinks decimal point
  content[4] = numeric[tmtime.tm_sec / 10];
  content[5] = numeric[tmtime.tm_sec % 10];

  if(tmtime.tm_hour == 4 &&
    tmtime.tm_min == 0 &&
    tmtime.tm_sec == 0) {// get current time via NTP everyday
      getNTP();
  }
}
