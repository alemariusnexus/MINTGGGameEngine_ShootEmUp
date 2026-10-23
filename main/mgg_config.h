#pragma once

#define MGG_APP_ID      "shootemup"
#define MGG_APP_NAME    "Shoot 'Em Up Demo"

#ifdef MINTGGGAMEENGINE_CHIP_ESP32P4
#   include "mgg_config_p4test.h"
#elif defined(MINTGGGAMEENGINE_CHIP_ESP32C3)
#   include "mgg_config_mintboyv1.h"
#elif defined(MINTGGGAMEENGINE_CHIP_ESP32S3)
#   include "mgg_config_s3test.h"
#elif defined(MINTGGGAMEENGINE_PORT_DESKTOP)
#   include "mgg_config_desktop.h"
#endif
