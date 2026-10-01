#include "engine.h"


LOG_USE_TAG("engine")


void gameSetup();
void gameLoop(float dt);
void postDraw(float dt);
void onCollision(const GameObjectCollision& coll);

DefaultEngine engine;
Game game;


void EngineSetup()
{
	DelayTaskMs(500);
    engine.earlySetup();
    DelayTaskMs(500);

#if defined(DISPLAY_TYPE_ST7735_SPI)
    ScreenST7735::Config screenCfg;
    ScreenST7735::initConfig(screenCfg);
    screenCfg.engine                = &engine;
    screenCfg.pins.cs               = DISPLAY_CS;
    screenCfg.pins.dc               = DISPLAY_DC;
    screenCfg.pins.rst              = DISPLAY_RST;
    Screen* screen = new ScreenST7735(screenCfg);
#elif defined(DISPLAY_TYPE_ILI9341_8080)
    ScreenILI9341::Config screenCfg;
    ScreenILI9341::initConfig(screenCfg);
    screenCfg.engine        = &engine;
    screenCfg.pins.cs       = DISPLAY_CS;
    screenCfg.pins.dc       = DISPLAY_DC;
    screenCfg.pins.rst      = DISPLAY_RST;
    screenCfg.pins.wr       = DISPLAY_WR;
    screenCfg.pins.rd       = DISPLAY_RD;
    screenCfg.pins.dat[0]   = DISPLAY_DAT0;
    screenCfg.pins.dat[1]   = DISPLAY_DAT1;
    screenCfg.pins.dat[2]   = DISPLAY_DAT2;
    screenCfg.pins.dat[3]   = DISPLAY_DAT3;
    screenCfg.pins.dat[4]   = DISPLAY_DAT4;
    screenCfg.pins.dat[5]   = DISPLAY_DAT5;
    screenCfg.pins.dat[6]   = DISPLAY_DAT6;
    screenCfg.pins.dat[7]   = DISPLAY_DAT7;
    Screen* screen = new ScreenILI9341(screenCfg);
#else
    Screen* screen = new ScreenNull;
#endif

    DefaultEngine::SetupConfig cfg;
    DefaultEngine::initConfig(cfg);
    cfg.game            = &game;
    cfg.appID           = "shootemup_demo";
    cfg.appName         = "Shoot 'Em Up Demo";
    cfg.screen          = screen;
    cfg.pins.spiMISO    = SPI_MISO;
    cfg.pins.spiMOSI    = SPI_MOSI;
    cfg.pins.spiSCK     = SPI_SCK;
    cfg.pins.sdCardCS   = SD_CS;
    cfg.pins.speaker    = SPEAKER_PIN;

    //engine.setPrintFrameStatistics(true);
    engine.setup(&cfg);

    game.setCollisionCallback(&onCollision);

    LogInfo("Running gameSetup()...");
    gameSetup();
    LogInfo("Finished gameSetup().");

    LogInfo("Running main loop...");
}

bool EngineLoop()
{
    return engine.doFrame(&gameLoop, &postDraw);
}

void EngineShutdown()
{
    engine.shutdown();
}


MINTGGGAMEENGINE_STARTUP_CODE()
