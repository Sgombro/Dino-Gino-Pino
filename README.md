# Dino Gino Game

## Descrizione
"Dino Gino" è un gioco semplice basato su un dinosauro che salta per evitare cactus. Il gioco è implementato su una scheda ESP8266 e utilizza un display OLED per la visualizzazione. Gli utenti possono controllare il salto del dinosauro sia tramite un pulsante fisico che tramite un'interfaccia web.

## Requisiti
- Scheda ESP8266
- Display OLED (SSD1306)
- Librerie:
  - Adafruit GFX
  - Adafruit SSD1306
  - ESP8266WiFi
  - ESP8266WebServer

## Configurazione
1. **Installazione delle librerie**: Assicurati di avere installato le librerie necessarie nel tuo ambiente di sviluppo Arduino.
2. **Configurazione WiFi**: Modifica le variabili `ssid` e `password` nel codice per configurare la tua rete WiFi.
3. **Collegamenti hardware**: Collega il display OLED e il pulsante ai pin specificati nel codice.

## Funzionalità
- **Controllo del salto**: Il dinosauro salta quando viene premuto il pulsante fisico o il pulsante nell'interfaccia web.
- **Generazione di cactus**: I cactus vengono generati casualmente e si muovono verso sinistra.
- **Punteggio**: Il punteggio aumenta ogni volta che il dinosauro evita un cactus.
- **Game Over**: Il gioco termina se il dinosauro collide con un cactus, mostrando il punteggio finale.

## Funzionamento
1. **Avvio del gioco**: Quando il gioco inizia, viene visualizzata una schermata iniziale. Premere il pulsante per iniziare.
2. **Gioco attivo**: Il dinosauro salta per evitare i cactus. Il punteggio viene visualizzato in tempo reale.
3. **Game Over**: Se il dinosauro collide con un cactus, il gioco termina e il punteggio finale viene mostrato.

## Note
- Il codice è diviso in sezioni per facilitare la comprensione e la modifica.
- È possibile estendere il gioco aggiungendo nuove funzionalità o migliorando la grafica.

## Contributi
Se desideri contribuire al progetto, sentiti libero di fare un fork e inviare una pull request con le tue modifiche.

## Licenza
Questo progetto è open-source e può essere utilizzato e modificato liberamente.
