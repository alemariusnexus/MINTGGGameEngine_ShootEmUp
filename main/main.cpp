#include "engine.h"

#include "images.h"

// Remove this if no joystick is connected
#define HAVE_JOYSTICK

// Remove this if no SD card is connected
#define HAVE_SDCARD

using namespace std;

LOG_USE_TAG("main")


enum class Gun {
  Normal,
  Fast,
  Spread
};

enum GameObjectTag {
  TagPlayerBullet   = (1 << 0),
  TagAsteroid       = (1 << 1),
  TagLife           = (1 << 2)
};


// MCP2300X IO Expander
GPIODeviceMCP2300X ioExpander(I2C_SCL, I2C_SDA);

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
timer_mstick_t playerGunResetTime = -1;
timer_mstick_t playerLastShootTime = -1;
timer_mstick_t playerInvincibleEnd = -1;

// Asteroid state
uint16_t asteroidsActiveTarget;
timer_mstick_t asteroidLastSpawnTime = -1;
uint16_t asteroidChosenSpawnInterval = 0;
float asteroidSpeed = 60.0f;



bool isGameOver() {
  return playerNumLives <= 0;
}


void setupBitmaps() {
#ifdef HAVE_SDCARD
  backgroundBmp = Bitmap::loadBMP("/sdcard/background.bmp");
  backgroundGameOverBmp = Bitmap::loadBMP("/sdcard/background-gameover.bmp");
  lifeBmp = Bitmap::loadBMP("/sdcard/heart.bmp");
  scoreBmp = Bitmap::loadBMP("/sdcard/star.bmp");
  playerBmp = Bitmap::loadBMP("/sdcard/player.bmp");
  bulletNormalBmp = Bitmap::loadBMP("/sdcard/bullet-normal.bmp");
  bulletSmallBmp = Bitmap::loadBMP("/sdcard/bullet-small.bmp");
  asteroidSmallBmp = Bitmap::loadBMP("/sdcard/asteroid-small.bmp");
  asteroidLargeBmp = Bitmap::loadBMP("/sdcard/asteroid-large.bmp");
#else
  backgroundBmp = Bitmap(160, 128, epd_bitmap_background);
  backgroundGameOverBmp = Bitmap(160, 128, epd_bitmap_background_gameover);
  lifeBmp = Bitmap(16, 16, epd_bitmap_heart, epd_bitmap_alpha_heart);
  scoreBmp = Bitmap(16, 15, epd_bitmap_star, epd_bitmap_alpha_star);
  playerBmp = Bitmap(24, 22, epd_bitmap_player, epd_bitmap_alpha_player);
  bulletNormalBmp = Bitmap(6, 12, epd_bitmap_bullet_normal, epd_bitmap_alpha_bullet_normal);
  bulletSmallBmp = Bitmap(7, 7, epd_bitmap_bullet_small, epd_bitmap_alpha_bullet_small);
  asteroidSmallBmp = Bitmap(12, 12, epd_bitmap_asteroid_small, epd_bitmap_alpha_asteroid_small);
  asteroidLargeBmp = Bitmap(24, 24, epd_bitmap_asteroid_large, epd_bitmap_alpha_asteroid_large);
#endif
}

void setupAudio() {
  backgroundAudio = createBackgroundAudio();
  shootAudio = createShootAudio();
  asteroidHitAudio = createAsteroidHitAudio();
  playerHitAudio = createPlayerHitAudio();
  gameOverAudio = createGameOverAudio();

  game.audio().setMute(true);
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

  playerProjectileDespawnZone = GameObject::createColliderRect (
      -500, -100, 1000+game.getScreen().getWidth(), 70);
  game.spawnObject(playerProjectileDespawnZone);

  asteroidDespawnZone = GameObject::createColliderRect (
      -500, game.getScreen().getHeight()+30, 1000+game.getScreen().getWidth(), 100);
  game.spawnObject(asteroidDespawnZone);
}

void setupText() {
  debugText = Text(60, 20);
  debugText.setVisible(false);
  debugText.setColor(Color::GREEN);
  game.addText(debugText);

  scoreText = Text(100, 2);
  scoreText.setColor(Color::YELLOW);
  game.addText(scoreText);

  gameOverScoreText = Text(game.getScreen().getWidth()/2, 90);
  gameOverScoreText.setAnchor(Text::Anchor::TopCenter);
  gameOverScoreText.setColor(Color::YELLOW);
  gameOverScoreText.setScaleFactor(2);
  gameOverScoreText.setVisible(false);
  game.addText(gameOverScoreText);
}

void onWeaponChanged() {
  if (playerGun == Gun::Normal) {
      playerGun = Gun::Fast;
    } else if (playerGun == Gun::Fast) {
      playerGun = Gun::Spread;
    } else {
      playerGun = Gun::Normal;
    }
}

void onMuteChanged() {
  game.audio().setMute(!game.audio().isMute());
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

void gameSetup() {
  ioExpander.begin();

  game.input().defineButton("up", 0, ioExpander);
  game.input().defineButton("down", 1, ioExpander);
  game.input().defineButton("left", 2, ioExpander);
  game.input().defineButton("right", 3, ioExpander);
  game.input().defineButton("a", 4, ioExpander);
  game.input().defineButton("b", 5, ioExpander);
  game.input().defineButton("start", 6, ioExpander);
  game.input().defineButton("joy", 7, ioExpander, InputEngine::PinFlagsActiveHigh | InputEngine::PinFlagsPulldown);

#ifdef HAVE_JOYSTICK
  game.input().defineAxis("x", 3, 0.0f, 1.0f);
  game.input().defineAxis("y", 4, 1.0f, 0.0f);
#endif

  game.input().defineButtonCombo({"up", "start"}, onMuteChanged);
  game.input().defineButtonCombo({"joy"}, onMuteChanged);
  game.input().defineButtonCombo({"b"}, onWeaponChanged);

  setupBitmaps();
  setupAudio();
  setupPlayer();
  setupMiscObjects();
  setupText();

  resetGame();
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

void handleGameOver(float dt) {
  if (isGameOver()) {
    gameOverBackground.setVisible(true);

    scoreText.setVisible(false);

    gameOverScoreText.setText(to_string(score));
    gameOverScoreText.setVisible(true);
  } else {
    gameOverBackground.setVisible(false);

    scoreText.setVisible(true);

    gameOverScoreText.setVisible(false);
  }

  if (isGameOver()) {
    if (game.input().isButtonPressed("start")) {
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

void shootPlayer(float dt) {
  auto now = TimerGetTickcountMs();

  if (playerGunResetTime >= 0  &&  now >= playerGunResetTime) {
    playerGun = Gun::Normal;
    playerGunResetTime = -1;
  }

  if (game.input().isButtonPressed("a")) {
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

  float ax = game.randReal((float) -asteroidBmp.getWidth(), (float) game.getScreen().getWidth());
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

  auto now = TimerGetTickcountMs();

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

void handleText(float dt) {
  scoreText.setText(to_string(score));

  auto lifeObjs = game.getGameObjectsWithTag(TagLife);
  if (lifeObjs.size() != playerNumLives) {
    game.despawnObjects(lifeObjs);

    for (int i = 0 ; i < playerNumLives ; i++) {
      GameObject life = GameObject::createBitmap(2 + i*(lifeBmp.getWidth()+2), 2, lifeBmp, false);
      life.setTag(TagLife);
      game.spawnObject(life);
    }
  }
}

void handleInvincible(float dt) {
  if (playerInvincibleEnd >= 0  &&  TimerGetTickcountMs() < playerInvincibleEnd) {
    if ((TimerGetTickcountMs()/200)%2 == 0) {
      player.setVisible(true);
    } else {
      player.setVisible(false);
    }
  } else {
    player.setVisible(true);
  }
}

void movePlayer(float dt) {
  Vec2 moveDir;

  // Analog stick (if installed)
  if (game.input().hasAxis("x")) {
    moveDir.setX(game.input().getAxis("x"));
  }
  if (game.input().hasAxis("y")) {
    moveDir.setY(game.input().getAxis("y"));
  }

  // Movement buttons
  if (game.input().isButtonPressed("left")  &&  !game.input().isButtonPressed("right")) {
    moveDir.setX(-1);
  } else if (game.input().isButtonPressed("right")  &&  !game.input().isButtonPressed("left")) {
    moveDir.setX(1);
  }
  if (game.input().isButtonPressed("up")  &&  !game.input().isButtonPressed("down")) {
    moveDir.setY(-1);
  }
  if (game.input().isButtonPressed("down")  &&  !game.input().isButtonPressed("up")) {
    moveDir.setY(1);
  }

  if (moveDir.lengthSq() > 1.0f) {
    moveDir.normalize();
  }
  moveDir *= dt*playerMoveSpeed;

  player.move(moveDir);

  uint16_t width = player.getSprite().getWidth();
  uint16_t height = player.getSprite().getHeight();

  if (player.getX() < 0) {
    player.setX(0);
  } else if (player.getX() >= game.getScreen().getWidth()-width) {
    player.setX(game.getScreen().getWidth()-width);
  }
  if (player.getY() < 0) {
    player.setY(0);
  } else if (player.getY() >= game.getScreen().getHeight()-height) {
    player.setY(game.getScreen().getHeight()-height);
  }
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

void gameLoop(float dt) {

  // ********** GAME LOGIC **********

  spawnAsteroids(dt);

  handleText(dt);
  handleGameOver(dt);

  if (!isGameOver()) {
    handleDifficulty(dt);
    handleInvincible(dt);
    movePlayer(dt);
    moveAsteroids(dt);
    moveProjectiles(dt);
    shootPlayer(dt);
  }

  string debugStr = "P:"s;
  debugStr += game.getGameObjectsWithTag(TagPlayerBullet).size();
  debugStr += " G:"s;
  switch (playerGun) {
  case Gun::Normal: debugStr += "N"s; break;
  case Gun::Fast:   debugStr += "F"s; break;
  case Gun::Spread: debugStr += "S"s; break;
  }
  debugStr += " A:"s + to_string(game.getGameObjectsWithTag(TagAsteroid).size());
  debugStr += " L:"s + to_string(playerNumLives);
  debugText.setText(debugStr);
}


void postDraw(float dt)
{
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
      if (playerInvincibleEnd < 0  ||  TimerGetTickcountMs() >= playerInvincibleEnd) {
        game.despawnObject(other);
        playerNumLives--;
        player.setPosition(playerStartX, playerStartY);
        playerInvincibleEnd = TimerGetTickcountMs() + 2000;

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
