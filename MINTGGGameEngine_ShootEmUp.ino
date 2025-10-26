#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <MCP23008.h>
#include <MINTGGGameEngine.h>

#include "defines.h"
#include "audio.h"

using namespace MINTGGGameEngine;


enum class Gun {
  Normal,
  Fast,
  Spread
};

enum GameObjectTag {
  TagPlayerBullet   = 0x0001,
  TagAsteroid       = 0x0002,
  TagLife           = 0x0100
};


// TFT-Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); // CS, DC, RST

// MCP23009 (IO-Erweiterung)
MCP23008 mcp(0x20);

// Base game setup
ScreenST7735 screen(&tft);
Game game;

// Bitmaps
Bitmap backgroundBmp;
Bitmap backgroundGameOverBmp;
Bitmap lifeBmp;
Bitmap scoreBmp;
Bitmap playerBmp;
Bitmap bulletNormalBmp;
Bitmap bulletSmallBmp;
Bitmap asteroidSmallBmp;
Bitmap asteroidLargeBmp;

// Game Objects
GameObject background;
GameObject gameOverBackground;
GameObject scoreIcon;
GameObject player;
GameObject playerProjectileDespawnZone;
GameObject asteroidDespawnZone;

// Texts
Text debugText;
Text scoreText;
Text gameOverScoreText;

// Audio
AudioClip backgroundAudio;
AudioClip shootAudio;
AudioClip asteroidHitAudio;
AudioClip playerHitAudio;
AudioClip gameOverAudio;

// Game state
uint32_t score = 0;

// Player state
float playerStartX;
float playerStartY;
int playerNumLives;
float playerMoveSpeed = 100.0f;
Gun playerGun = Gun::Normal;
long playerGunResetTime = -1;
long playerLastShootTime = -1;
long playerInvincibleEnd = -1;

// Asteroid state
uint16_t asteroidsActiveTarget;
long asteroidLastSpawnTime = -1;
uint16_t asteroidChosenSpawnInterval = 0;
float asteroidSpeed = 60.0f;


bool isGameOver() {
  return playerNumLives <= 0;
}


void setupBitmaps() {
  backgroundBmp = Bitmap::loadBMP("/background.bmp");
  backgroundGameOverBmp = Bitmap::loadBMP("/background-gameover.bmp");
  lifeBmp = Bitmap::loadBMP("/heart.bmp");
  scoreBmp = Bitmap::loadBMP("/star.bmp");
  playerBmp = Bitmap::loadBMP("/player.bmp");
  bulletNormalBmp = Bitmap::loadBMP("/bullet-normal.bmp");
  bulletSmallBmp = Bitmap::loadBMP("/bullet-small.bmp");
  asteroidSmallBmp = Bitmap::loadBMP("/asteroid-small.bmp");
  asteroidLargeBmp = Bitmap::loadBMP("/asteroid-large.bmp");
}

void setupAudio() {
  backgroundAudio = createBackgroundAudio();
  shootAudio = createShootAudio();
  asteroidHitAudio = createAsteroidHitAudio();
  playerHitAudio = createPlayerHitAudio();
  gameOverAudio = createGameOverAudio();
}

void setupPlayer() {
  playerStartX = 79 - playerBmp.getWidth()/2;
  playerStartY = 127-playerBmp.getHeight();
  player = GameObject::createBitmap(playerStartX, playerStartY, playerBmp);
  game.spawnObject(player);
}

void setupMiscObjects() {
  background = GameObject::createBitmap(0, 0, backgroundBmp, false);
  background.setZOrder(ZOrderBackground);
  game.spawnObject(background);

  gameOverBackground = GameObject::createBitmap(0, 0, backgroundGameOverBmp, false);
  gameOverBackground.setZOrder(ZOrderOverlay);
  gameOverBackground.setVisible(false);
  game.spawnObject(gameOverBackground);

  scoreIcon = GameObject::createBitmap(80, 0, scoreBmp, false);
  scoreIcon.setZOrder(ZOrderForeground);
  game.spawnObject(scoreIcon);

  playerProjectileDespawnZone = GameObject::createColliderRect(-500, -100, 1000+screen.getWidth(), 70);
  game.spawnObject(playerProjectileDespawnZone);

  asteroidDespawnZone = GameObject::createColliderRect(-500, screen.getHeight()+30, 1000+screen.getWidth(), 100);
  game.spawnObject(asteroidDespawnZone);
}

void setupText() {
  debugText = Text(60, 20);
  debugText.setVisible(false);
  debugText.setColor(ST77XX_GREEN);
  game.addText(debugText);

  scoreText = Text(100, 2);
  scoreText.setColor(ST77XX_YELLOW);
  scoreText.setSize(2);
  game.addText(scoreText);

  gameOverScoreText = Text(50, 90);
  gameOverScoreText.setColor(ST77XX_YELLOW);
  gameOverScoreText.setSize(3);
  gameOverScoreText.setVisible(false);
  game.addText(gameOverScoreText);
}

void setup() {

  // ********** PERIPHERAL SETUP **********

  Serial.begin(115200);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed.");
    while (true);
  }

  Wire.begin();

  mcp.begin();
  mcp.pinMode1(BUTTON_UP, INPUT_PULLUP);
  mcp.pinMode1(BUTTON_DOWN, INPUT_PULLUP);
  mcp.pinMode1(BUTTON_LEFT, INPUT_PULLUP);
  mcp.pinMode1(BUTTON_RIGHT, INPUT_PULLUP);
  mcp.pinMode1(BUTTON_A, INPUT_PULLUP);
  mcp.pinMode1(BUTTON_B, INPUT_PULLUP);
  mcp.pinMode1(BUTTON_START, INPUT_PULLUP);
  mcp.pinMode1(BUTTON_JOY, INPUT_PULLUP);

  screen.begin();
  game.begin(screen);
  game.audio().begin(SPEAKER_PIN);


  // ********** GAME SETUP **********

  game.setCollisionCallback(onCollision);

  setupBitmaps();
  setupAudio();
  setupPlayer();
  setupMiscObjects();
  setupText();

  resetGame();
}


void resetGame() {
  playerNumLives = 3;
  asteroidsActiveTarget = 3;
  score = 0;

  game.despawnObjects(game.getGameObjectsWithTag(TagAsteroid));
  game.despawnObjects(game.getGameObjectsWithTag(TagPlayerBullet));

  player.setPosition(playerStartX, playerStartY);

  game.audio().playClip(backgroundAudio, AudioEngine::Priority::Background, true, true);
}

void shootPlayerProjectile(float dt) {
  if (playerGun == Gun::Normal) {
    float bx = player.getX() + player.getSprite().getWidth()/2 - bulletNormalBmp.getWidth()/2;
    float by = player.getY() - bulletNormalBmp.getHeight();
    GameObject bullet = GameObject::createBitmap(bx, by, bulletNormalBmp);
    bullet.setTag(TagPlayerBullet);
    bullet.setMoveDirection(0, -1);
    game.spawnObject(bullet);
  } else if (playerGun == Gun::Fast) {
    float bx = player.getX() + player.getSprite().getWidth()/2 - bulletSmallBmp.getWidth()/2;
    float by = player.getY() - bulletSmallBmp.getHeight();
    GameObject bullet = GameObject::createBitmap(bx, by, bulletSmallBmp);
    bullet.setTag(TagPlayerBullet);
    bullet.setMoveDirection(0, -1);
    game.spawnObject(bullet);
  } else if (playerGun == Gun::Spread) {
    float playerTopCenterX = player.getX() + player.getSprite().getWidth()/2;
    float playerTopCenterY = player.getY();
    for (int i = -1 ; i <= 1 ; i++) {
      float bx = playerTopCenterX - bulletSmallBmp.getWidth()/2 + i*5;
      float by = playerTopCenterY - bulletSmallBmp.getHeight();
      GameObject bullet = GameObject::createBitmap(bx, by, bulletSmallBmp);
      bullet.setTag(TagPlayerBullet);
      bullet.setMoveDirection(i*0.3f, -1.0f);
      game.spawnObject(bullet);
    }
  }

  game.audio().playClip(shootAudio);
}

void handleGameOver(float dt, int buttonState) {
  if (isGameOver()) {
    gameOverBackground.setVisible(true);

    scoreText.setVisible(false);

    gameOverScoreText.setText(String(score));
    gameOverScoreText.setVisible(true);
  } else {
    gameOverBackground.setVisible(false);

    scoreText.setVisible(true);

    gameOverScoreText.setVisible(false);
  }

  if (isGameOver()) {
    if (buttonState & (1 << BUTTON_START)) {
      resetGame();
    }
  }
}

void handleDifficulty(float dt) {
  if (score > 1000) {
    asteroidsActiveTarget = 12;
    asteroidSpeed = 110.0f;
    backgroundAudio.setTempo(2, 250);
  } else if (score > 800) {
    asteroidsActiveTarget = 9;
    asteroidSpeed = 100.0f;
    backgroundAudio.setTempo(2, 230);
  } else if (score > 500) {
    asteroidsActiveTarget = 7;
    asteroidSpeed = 80.0f;
    backgroundAudio.setTempo(2, 210);
  } else if (score > 300) {
    asteroidsActiveTarget = 5;
    asteroidSpeed = 80.0f;
    backgroundAudio.setTempo(2, 190);
  } else if (score > 150) {
    asteroidsActiveTarget = 4;
    asteroidSpeed = 70.0f;
    backgroundAudio.setTempo(2, 170);
  } else {
    asteroidsActiveTarget = 3;
    asteroidSpeed = 60.0f;
    backgroundAudio.setTempo(2, 150);
  }
}

void shootPlayer(float dt, int buttonState) {
  long now = millis();

  if (playerGunResetTime >= 0  &&  now >= playerGunResetTime) {
    playerGun = Gun::Normal;
    playerGunResetTime = -1;
  }

  if (buttonState & (1 << BUTTON_A)) {
    if (playerGun == Gun::Normal) {
      const long fireSpeed = 500;

      if (playerLastShootTime < 0  ||  now-playerLastShootTime >= fireSpeed) {
        shootPlayerProjectile(dt);
        playerLastShootTime = now;
      }
    } else if (playerGun == Gun::Fast) {
      const long fireSpeed = 250;

      if (playerLastShootTime < 0  ||  now-playerLastShootTime >= fireSpeed) {
        shootPlayerProjectile(dt);
        playerLastShootTime = now;
      }
    } else if (playerGun == Gun::Spread) {
      const long fireSpeed = 750;

      if (playerLastShootTime < 0  ||  now-playerLastShootTime >= fireSpeed) {
        shootPlayerProjectile(dt);
        playerLastShootTime = now;
      }
    }
  }
}

void spawnAsteroid() {
  const float largeChance = 0.2f;

  Bitmap asteroidBmp;
  if (game.randReal(1.0f) < largeChance) {
    asteroidBmp = asteroidLargeBmp;
  } else {
    asteroidBmp = asteroidSmallBmp;
  }

  float ax = game.randReal((float) -asteroidBmp.getWidth(), (float) screen.getWidth());
  float ay = -asteroidBmp.getHeight() - game.randReal(0.0f, 20.0f);

  FlipDir flipDir;
  switch (game.randInt(3)) {
  case 0: flipDir = FlipDir::None; break;
  case 1: flipDir = FlipDir::Horizontal; break;
  case 2: flipDir = FlipDir::Vertical; break;
  case 3: flipDir = FlipDir::Both; break;
  default: flipDir = FlipDir::None;
  }

  GameObject asteroid = GameObject::createBitmap(ax, ay, asteroidBmp);
  asteroid.setFlipDir(flipDir);
  asteroid.setTag(TagAsteroid);
  asteroid.setMoveDirection(game.randReal(-0.5f, 0.5f), 1.0f);
  game.spawnObject(asteroid);
}

void spawnAsteroids(float dt) {
  uint16_t asteroidsActive = game.getGameObjectsWithTag(TagAsteroid).size();

  if (asteroidsActive >= asteroidsActiveTarget) {
    // At or over target -> don't spawn anything
    return;
  }

  long now = millis();

  //                      numTooFew       1       2       3       4       5+
  const uint16_t minSpawnIntervals[] = {  500,    300,    200,    100,    50    };
  const uint16_t maxSpawnIntervals[] = {  1000,   750,    500,    300,    100   };
  const size_t numSpawnIntervals = sizeof(minSpawnIntervals)/sizeof(uint16_t);

  uint16_t numTooFew = asteroidsActiveTarget - asteroidsActive;
  size_t intervalIdx = (numTooFew >= numSpawnIntervals) ? numSpawnIntervals-1 : numTooFew-1;

  uint16_t minInterval = minSpawnIntervals[intervalIdx];
  uint16_t maxInterval = maxSpawnIntervals[intervalIdx];

  if (asteroidChosenSpawnInterval == 0  ||  (asteroidChosenSpawnInterval < minInterval  ||  asteroidChosenSpawnInterval > maxInterval)) {
    asteroidChosenSpawnInterval = game.randInt(minInterval, maxInterval);
  }

  if (asteroidLastSpawnTime < 0  ||  (now >= asteroidLastSpawnTime+asteroidChosenSpawnInterval)) {
    spawnAsteroid();
    asteroidLastSpawnTime = now;
    asteroidChosenSpawnInterval = 0;
  }
}

void handleText(float dt, int buttonState) {
  scoreText.setText(String(score));

  auto lifeObjs = game.getGameObjectsWithTag(TagLife);
  if (lifeObjs.size() != playerNumLives) {
    game.despawnObjects(lifeObjs);

    for (int i = 0 ; i < playerNumLives ; i++) {
      GameObject life = GameObject::createBitmap(2 + i*(lifeBmp.getWidth()+2), 2, lifeBmp, false);
      life.setTag(TagLife);
      game.spawnObject(life);
    }
  }

  if (buttonState & (1 << BUTTON_B)) {
    if (playerGun == Gun::Normal) {
      playerGun = Gun::Fast;
    } else if (playerGun == Gun::Fast) {
      playerGun = Gun::Spread;
    } else {
      playerGun = Gun::Normal;
    }
  }
}

void handleInvincible(float dt) {
  if (playerInvincibleEnd >= 0  &&  millis() < playerInvincibleEnd) {
    if ((millis()/200)%2 == 0) {
      player.setVisible(true);
    } else {
      player.setVisible(false);
    }
  } else {
    player.setVisible(true);
  }
}

void movePlayer(float dt, int buttonState) {
  Vec2 moveDir;

  if (buttonState & (1 << BUTTON_LEFT)  &&  !(buttonState & (1 << BUTTON_RIGHT))) {
    moveDir.setX(-1);
  } else if (buttonState & (1 << BUTTON_RIGHT)  &&  !(buttonState & (1 << BUTTON_LEFT))) {
    moveDir.setX(1);
  }
  if (buttonState & (1 << BUTTON_UP)  &&  !(buttonState & (1 << BUTTON_DOWN))) {
    moveDir.setY(-1);
  }
  if (buttonState & (1 << BUTTON_DOWN)  &&  !(buttonState & (1 << BUTTON_UP))) {
    moveDir.setY(1);
  }

  moveDir.normalize();
  moveDir *= dt*playerMoveSpeed;

  player.move(moveDir);
}

void moveAsteroids(float dt) {
  for (GameObject go : game.getGameObjectsWithTag(TagAsteroid)) {
    go.move(asteroidSpeed*dt);
  }
}

void moveProjectiles(float dt) {
  const float playerProjectileSpeed = 100.0f;

  for (GameObject go : game.getGameObjectsWithTag(TagPlayerBullet)) {
    go.move(playerProjectileSpeed*dt);
  }
}

void loop() {
  
  // ********** PERIPHERAL STATE **********

  int buttonState = ~mcp.read8();



  // ********** GAME LOGIC **********

  float dt = game.getFrameTime() * 1e-3f; // delta time (seconds)

  spawnAsteroids(dt);

  handleText(dt, buttonState);
  handleGameOver(dt, buttonState);

  if (!isGameOver()) {
    handleDifficulty(dt);
    handleInvincible(dt);
    movePlayer(dt, buttonState);
    moveAsteroids(dt);
    moveProjectiles(dt);
    shootPlayer(dt, buttonState);
  }

  String debugStr = "P:";
  debugStr += game.getGameObjectsWithTag(TagPlayerBullet).size();
  debugStr += " G:";
  switch (playerGun) {
  case Gun::Normal: debugStr += "N"; break;
  case Gun::Fast:   debugStr += "F"; break;
  case Gun::Spread: debugStr += "S"; break;
  }
  debugStr += " A:" + String(game.getGameObjectsWithTag(TagAsteroid).size());
  debugStr += " L:" + String(playerNumLives);
  debugText.setText(debugStr);

  game.checkCollisions();

  game.draw();

  game.sleepNextFrame();

}



void onCollision(const GameObjectCollision& coll) {
  if (coll.isInvolved(playerProjectileDespawnZone)) {
    GameObject projectile = coll.getOther(playerProjectileDespawnZone);
    if (projectile.hasTag(TagPlayerBullet)) {
      game.despawnObject(projectile);
    }
  }
  if (coll.isInvolved(asteroidDespawnZone)) {
    GameObject asteroid = coll.getOther(asteroidDespawnZone);
    if (asteroid.hasTag(TagAsteroid)) {
      game.despawnObject(asteroid);
    }
  }
  if (coll.isInvolved(player)) {
    GameObject other = coll.getOther(player);
    if (other.hasTag(TagAsteroid)) {
      if (playerInvincibleEnd < 0  ||  millis() >= playerInvincibleEnd) {
        game.despawnObject(other);
        playerNumLives--;
        player.setPosition(playerStartX, playerStartY);
        playerInvincibleEnd = millis() + 2000;

        if (isGameOver()) {
          game.audio().stopClip(backgroundAudio);
          game.audio().playClip(gameOverAudio);
        } else {
          game.audio().playClip(playerHitAudio);
        }
        return;
      }
    }
  }
  if (coll.isTagInvolved(TagPlayerBullet)) {
    GameObject bullet = coll.getByTag(TagPlayerBullet);
    GameObject target = coll.getOther(bullet);
    if (target.hasTag(TagAsteroid)) {
      game.despawnObject(bullet);
      game.despawnObject(target);
      if (target.getSprite().getBitmap() == asteroidLargeBmp) {
        score += 20;
      } else {
        score += 10;
      }
      game.audio().playClip(asteroidHitAudio);
      return;
    }
  }
}
