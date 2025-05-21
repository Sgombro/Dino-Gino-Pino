#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define LARGHEZZA_SCHERMO 128
#define ALTEZZA_SCHERMO 64
#define RESET_OLED -1

Adafruit_SSD1306 display(LARGHEZZA_SCHERMO, ALTEZZA_SCHERMO, &Wire, RESET_OLED);

const int pulsanteSalto = 14;
const int buzzer = 12;

const char* ssid = "Dino";
const char* password = "";
ESP8266WebServer server(80)

;
volatile bool pulsanteWeb = false; 

// Sprite e variabili originali mantenute intatte
const unsigned char dinoBitmap[] PROGMEM = {
  0x00, 0xfe, 0x01, 0x80, 0x01, 0x41, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x0f, 0x43, 0x02, 0xc6, 0x1e, 0xcc, 0x18, 0xb8, 0x08, 0x90, 0x18, 
  0x80, 0x18, 0x80, 0x10, 0x40, 0x20, 0x60, 0x60, 0x10, 0x40, 0x10, 0x40, 
  0x03, 0x40, 0x05, 0x40, 0x03, 0x40, 0x0f, 0xc0
};

const unsigned char cactusBitmap[] PROGMEM = {
  0x0f, 0x80, 0x08, 0x80, 0x08, 0xb0, 0x08, 0xc8, 0x48, 0xc8, 0x98, 0xc8, 0x98, 0xc8, 0x98, 0xc8,
  0x98, 0x08, 0x98, 0x10, 0x80, 0x80, 0x40, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80,
  0x08, 0x80, 0x0f, 0x80
};

int dinoY = 42;
bool staSaltando = false;
int velocitaSalto = 0;
const int gravita = 3;
const int forzaSalto = -19;
int punteggio = 0;

struct Cactus {
  int x;
  bool attivo;
};

Cactus cactus1 = {LARGHEZZA_SCHERMO, false};
Cactus cactus2 = {LARGHEZZA_SCHERMO, false};

unsigned long ultimoAggiornamento = 0;
const int velocitaGioco = 50;

bool giocoAttivo = true;
bool schermataIniziale = true;
bool primoAvvio = true;

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5);

  pinMode(pulsanteSalto, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Errore display"));
    while(true);
  }

  display.clearDisplay();
  display.display();
  randomSeed(analogRead(A0));

  // config WiFi 
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/salta", handleSalta);
  server.begin();

  generaCactus(cactus1, LARGHEZZA_SCHERMO);
  cactus1.attivo = true;
}

void loop() {
  server.handleClient(); //gestione richieste web

  if (primoAvvio && schermataIniziale) {
    displaySchermataIniziale();
a    if (digitalRead(pulsanteSalto) == LOW || pulsanteWeb) {
      delay(300);
      pulsanteWeb = false;
      schermataIniziale = false;
      primoAvvio = false;
      riavviaGioco();
    }
    return;
  }

  if (!giocoAttivo) {
    if (digitalRead(pulsanteSalto) == LOW || pulsanteWeb) {
      delay(300);
      pulsanteWeb = false;
      riavviaGioco();
      schermataIniziale = false;
      primoAvvio = false;
    }
    return;
  }

  if (millis() - ultimoAggiornamento > velocitaGioco) {
    ultimoAggiornamento = millis();

    bool saltoPremuto = (digitalRead(pulsanteSalto) == LOW) || pulsanteWeb;
    pulsanteWeb = false;

    if (!staSaltando && saltoPremuto) {
      staSaltando = true;
      velocitaSalto = forzaSalto;
      tone(buzzer, 1000, 100);
    }

    if (staSaltando) {
      dinoY += velocitaSalto / 2;
      velocitaSalto += gravita;

      if (dinoY >= 42) {
        dinoY = 42;
        staSaltando = false;
        velocitaSalto = 0;
      }
    }

    muoviCactus(cactus1);
    muoviCactus(cactus2);
    gestisciGenerazioneCactus();

    if ((cactus1.attivo && controlloCollisione(cactus1.x, dinoY)) ||
        (cactus2.attivo && controlloCollisione(cactus2.x, dinoY))) {
      fineGioco();
      return;
    }

    disegnaGioco();
  }
}

// GTML webserver
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{text-align:center;font-family:Arial;} button{padding:15px 30px;font-size:20px;}</style>";
  html += "<script>function salta(){fetch('/salta').then(r=>setTimeout(()=>{},50))}</script>";
  html += "</head><body><h1>Dino Controller</h1>";
  html += "<button onclick='salta()'>SALTA</button>";
  html += "<p>Made with love by Gianella Daniele & Manzo Gabriel</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleSalta() {
  pulsanteWeb = true;
  server.send(200, "text/plain", "");
}


// gestione dei cactus
void muoviCactus(Cactus &c) {
  if (c.attivo) {
    c.x -= 6;                     // Sposta cactus verso sinistra
    if (c.x < -13) {              // Se completamente fuori schermo
      c.attivo = false;           // Disattiva cactus
      punteggio++;                // Incrementa punteggio per cactus evitato
    }
  }
}

void generaCactus(Cactus &c, int posizioneIniziale) {
  c.x = posizioneIniziale;
  c.attivo = true;
}

void gestisciGenerazioneCactus() {
  if (cactus1.attivo && cactus2.attivo) return;

  Cactus *cactusInattivo = !cactus1.attivo ? &cactus1 : (!cactus2.attivo ? &cactus2 : nullptr);
  if (cactusInattivo == nullptr) return;

  Cactus *cactusAttivo = cactus1.attivo ? &cactus1 : &cactus2;

  if (cactusAttivo->x <= 65) {
    int r = random(0, 100);
    if (r < 35) {  // 35% probabilità di generare
      int chanceRandom = random(0, 100);
      if (chanceRandom >= 60) {
        if (cactusAttivo->x < LARGHEZZA_SCHERMO - 84) {
          generaCactus(*cactusInattivo, LARGHEZZA_SCHERMO);
        }
      }
    }
  }
}

//  collisioni
bool controlloCollisione(int xCactus, int yDino) {
  if (xCactusInRange(xCactus) && yDino + 22 > 46) {
    return true;
  }
  return false;
}

bool xCactusInRange(int xCactus) {
  return xCactus < 32 && xCactus > 0;
}

// Funzioni per il disegno e grafica
void disegnaGioco() {
  display.clearDisplay();           // Pulisce buffer

  display.drawBitmap(16, dinoY, dinoBitmap, 16, 22, SSD1306_WHITE);

  if (cactus1.attivo)
    display.drawBitmap(cactus1.x, 46, cactusBitmap, 13, 18, SSD1306_WHITE);
  if (cactus2.attivo)
    display.drawBitmap(cactus2.x, 46, cactusBitmap, 13, 18, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Punteggio: ");
  display.print(punteggio);

  display.display();                // Aggiorna il display
}

//Game over
void fineGioco() {
  noTone(buzzer);                  // Spegni buzzer se acceso
  giocoAttivo = false;             // Ferma gioco

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 10);
  display.print("GAME OVER");
  display.setTextSize(1);
  display.setCursor(30, 35);
  display.print("Punteggio: ");
  display.print(punteggio);
  
  display.setCursor(10, 50);
  display.print("Premi il bottone");
  display.display();

  tone(buzzer, 500, 500);          // Secondi tono
}

// Schermata iniziale
void displaySchermataIniziale() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println(F("Dino Gino"));
  display.setTextSize(1);
  display.setCursor(5, 40);
  display.println(F("Premi il pulsante"));
  display.setCursor(5, 50);
  display.println(F("per saltare"));
  display.display();
}

// Reset gioco
void riavviaGioco() {
  punteggio = 0;
  dinoY = 42;
  staSaltando = false;
  velocitaSalto = 0;
  cactus1.attivo = false;
  cactus2.attivo = false;
  generaCactus(cactus1, LARGHEZZA_SCHERMO);
  giocoAttivo = true;
}
