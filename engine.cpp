#include "engine.h"

// TFT-Display
SPIClass spi = SPIClass(SPI_BUS);
Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);

// Basisspiel
ScreenST7735 screen(&tft);
Game game;

// SD-Karte
bool sdCardInitialized = false;


// In der Sketchdatei zu definieren!
void gameSetup();
void gameLoop(float dt);
void onCollision(const GameObjectCollision& coll);


void setup() {

  // ********** PERIPHERAL SETUP **********

  Serial.begin(115200);
  
  Serial.println("Serial setup successful.");
  
#if I2C_SCL >= 0  &&  I2C_SDA >= 0
  Serial.println("Initializing I2C...");
  Wire.begin(I2C_SDA, I2C_SCL);
#else
  //Wire.begin();
#endif

  Serial.println("Initializing SPI...");
  spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, TFT_CS);

  // SD-Karte initialisieren (falls vorhanden)
#if SD_CS >= 0
  Serial.println("Initializing SD card...");
  if (SD.begin(SD_CS, spi)) {
    sdCardInitialized = true;
  } else {
    Serial.println("SD init failed.");
  }
#else
    Serial.println("Skipping SD card (not configured).");
#endif

  // Engine starten
  Serial.println("Starting engine...");
  screen.begin();
  game.begin(screen);
  game.audio().begin(SPEAKER_PIN);
  game.input().begin(10);


  // ********** GAME SETUP **********

  // Funktion festlegen, die bei Kollision zweiter GameObject aufgerufen wird
  game.setCollisionCallback(onCollision);

  Serial.println("Running game setup...");
  gameSetup();
  
  Serial.println("Setup done. Entering game loop.");
}

void loop() {
  // ********** GAME LOGIC **********

  // Vergangene Zeit seit letztem Frame berechnen (delta time, in Sekunden)
  float dt = game.getFrameTime() * 1e-3f;

  gameLoop(dt);

  game.checkCollisions(); // Kollisionsprüfung
  game.draw(); // GameObjects zeichnen
  game.sleepNextFrame(); // Warten bis zum nächsten Frame
}
