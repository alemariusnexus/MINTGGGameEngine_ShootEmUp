include("${CMAKE_CURRENT_SOURCE_DIR}/../components/MINTGGGameEngine/cmake/MINTGGGameEngine.cmake")

MGG_ComponentPrologue()

set(SRCS
    audio.cpp
    main.cpp
    )

MGG_SetupApplication()
