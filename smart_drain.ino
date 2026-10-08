// Smart Drain Safety & Early-Warning System - ESP32 firmware
// Reads water level, flow rate and a gas indicator, raises a local buzzer/LED alert
// and serves a live dashboard over Wi-Fi.
// Author: KAMALRAJ G
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"

WebServer server(80);
volatile uint32_t pulseCount = 0;

void IRAM_ATTR onPulse() { pulseCount++; }

struct Reading {
  float levelPct;
  float flowLpm;
  int   gasRaw;
  const char* status;
};

Reading latest = {0, 0, 0, "NORMAL"};
unsigned long lastSample = 0;

// Same decision rules as threshold_logic.py (keep both in sync).
const char* evaluate(float levelPct, float flowLpm, int gasRaw) {
  if (gasRaw >= GAS_ALERT_RAW) return "GAS_ALERT";
  if (levelPct >= LEVEL_ALERT_PCT) return "OVERFLOW_ALERT";
  if (levelPct >= LEVEL_WARN_PCT && flowLpm <= FLOW_LOW_LPM) return "BLOCKAGE_WARNING";
  return "NORMAL";
}

float readLevelPct() {
  int raw = analogRead(PIN_LEVEL);
  float pct = (raw - LEVEL_RAW_EMPTY) * 100.0f / (LEVEL_RAW_FULL - LEVEL_RAW_EMPTY);
  return constrain(pct, 0.0f, 100.0f);
}

void sampleSensors() {
  noInterrupts();
  uint32_t pulses = pulseCount;
  pulseCount = 0;
  interrupts();

  float seconds = SAMPLE_MS / 1000.0f;
  float flowLpm = (pulses / seconds) / FLOW_PULSES_PER_LPM;

  latest.levelPct = readLevelPct();
  latest.flowLpm = flowLpm;
  latest.gasRaw = analogRead(PIN_GAS);
  latest.status = evaluate(latest.levelPct, latest.flowLpm, latest.gasRaw);

  bool alarm = strcmp(latest.status, "NORMAL") != 0;
  digitalWrite(PIN_BUZZER, alarm ? HIGH : LOW);
  digitalWrite(PIN_LED, alarm ? HIGH : LOW);

  Serial.printf("level=%.1f%% flow=%.2fL/min gas=%d status=%s\n",
                latest.levelPct, latest.flowLpm, latest.gasRaw, latest.status);
}

const char PAGE[] PROGMEM = R"HTML(
<!doctype html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart Drain Dashboard</title>
<style>
body{font-family:Arial,sans-serif;margin:24px;background:#f4f8f9;color:#1f2d33}
h1{font-size:22px}.card{background:#fff;border-radius:10px;padding:16px;margin:10px 0;box-shadow:0 1px 4px #0002}
.big{font-size:28px;font-weight:bold}#status{padding:12px;border-radius:10px;font-size:20px;font-weight:bold;color:#fff}
</style></head><body>
<h1>Smart Drain Safety &amp; Early-Warning System</h1>
<div id="status">Loading...</div>
<div class="card">Water level<div class="big" id="level">-</div></div>
<div class="card">Flow rate<div class="big" id="flow">-</div></div>
<div class="card">Gas sensor (raw)<div class="big" id="gas">-</div></div>
<script>
async function tick(){
  try{
    const d=await (await fetch('/data')).json();
    document.getElementById('level').textContent=d.level_pct.toFixed(1)+' %';
    document.getElementById('flow').textContent=d.flow_lpm.toFixed(2)+' L/min';
    document.getElementById('gas').textContent=d.gas_raw;
    const s=document.getElementById('status');
    s.textContent=d.status;
    s.style.background=d.status==='NORMAL'?'#2e8b57':'#c0392b';
  }catch(e){document.getElementById('status').textContent='No connection';}
}
tick();setInterval(tick,2000);
</script></body></html>
)HTML";

void handleRoot() { server.send_P(200, "text/html", PAGE); }

void handleData() {
  char buf[160];
  snprintf(buf, sizeof(buf),
           "{\"level_pct\":%.1f,\"flow_lpm\":%.2f,\"gas_raw\":%d,\"status\":\"%s\"}",
           latest.levelPct, latest.flowLpm, latest.gasRaw, latest.status);
  server.send(200, "application/json", buf);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_FLOW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW), onPulse, FALLING);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nDashboard: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - lastSample >= SAMPLE_MS) {
    lastSample = millis();
    sampleSensors();
  }
}
