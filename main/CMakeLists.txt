include("${CMAKE_CURRENT_SOURCE_DIR}/../components/MINTGGGameEngine/cmake/MINTGGGameEngine.cmake")

MINTGGGameEngine_ComponentPrologue()

set(SRCS
        audio.cpp
        engine.cpp
        main.cpp
)

MINTGGGameEngine_SetupApplication()
