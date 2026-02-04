#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TRIG_PIN 5
#define ECHO_PIN 18

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebServer server(80);

// AP aberto
const char* ssid = "ESP32-Distancia";

long medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  long distancia = duracao / 58;
  return distancia;
}

void paginaPrincipal() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Ultrassom</title>
<style>
body { background:#111; color:#0f0; font-family:Arial; text-align:center; }
.card { background:#222; padding:20px; margin:20px; border-radius:10px; }
.valor { font-size:48px; }
</style>
</head>
<body>
<h2>ESP32 + HC-SR04</h2>
<div class="card">
<p>Distância</p>
<div class="valor"><span id="d">--</span> cm</div>
</div>

<script>
setInterval(()=>{
  fetch("/dist")
  .then(r=>r.text())
  .then(v=>document.getElementById("d").innerHTML=v);
},500);
</script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void enviarDistancia() {
  server.send(200, "text/plain", String(medirDistancia()));
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(115200);

  // OLED
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Wi-Fi AP aberto
  WiFi.softAP(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", paginaPrincipal);
  server.on("/dist", enviarDistancia);
  server.begin();
}

void loop() {
  long d = medirDistancia();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Distancia:");
  display.setTextSize(3);
  display.setCursor(0,20);
  display.print(d);
  display.println("cm");
  display.display();

  server.handleClient();
  delay(300);
}
