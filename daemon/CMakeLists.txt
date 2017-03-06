SET(FLINT_DAEMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
## options
OPTION(FLINGD_DEBUG           "Debug Mode"        OFF)
OPTION(FLINGD_TRACE           "Trace Mode"        OFF)
OPTION(FLINGD_USE_CXX11       "Use C++11"         ON)
OPTION(FLINGD_USE_THREAD      "Use Threads"       OFF)
OPTION(FLINGD_BUILD_TEST      "Build Tests"       OFF)
OPTION(FLINGD_BUILD_EXAMPLE   "Build Examples"    OFF)

## platform or product definition
IF(CMAKE_BUILD_PLATFORM STREQUAL "linux")
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DLINUX)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "linux")

IF(CMAKE_BUILD_PLATFORM STREQUAL "mac")
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DAPPLE)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "mac")

IF(CMAKE_BUILD_PLATFORM STREQUAL "soundkit")
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DLINUX)
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DSOUNDKIT)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "soundkit")

IF(CMAKE_BUILD_PLATFORM STREQUAL "ffos")
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DFFOS)
    # SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DMATCHSTICK)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "ffos")

## flint-daemon-include
SET(flint-daemon-include
    ${FLINT}/src
    ${LIBS}/rapidjson/include
    ${LIBS}/boost
    ${LIBS}/websocketpp
)

## flint-daemon-src
SET(flint-daemon-src
	${FLINT_DAEMON_SRC}/main.cpp
)

## flint-daemon-libs
SET(flint-daemon-libs
    flint_static
)

IF(ANDROID)
    SET(flint-daemon-libs
        ${flint-daemon-libs}
        log
    )
ENDIF(ANDROID)

IF(CMAKE_BUILD_PLATFORM STREQUAL "linux")
	SET(flint-daemon-libs
		${flint-daemon-libs}
		pthread
		)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "linux")

IF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")
    SET(flint-daemon-libs
        ${flint-daemon-libs}
        rt
    )
ENDIF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")

IF(UXIX AND NOT APPLE)
    SET(flint-daemon-libs
        ${flint-daemon-libs}
        rt
    )
ENDIF(UXIX AND NOT APPLE)

IF (APPLE)
    ADD_DEFINITIONS (-DNDEBUG -Wall)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF(APPLE)

## flint-daemon-cflags
IF (UNIX)
    SET(flint-daemon-cflags
        ${flint-daemon-cflags}
        -Wall
        -frtti
        -fexceptions
        -fPIC
        -D_FILE_OFFSET_BITS=64
        -DBOOST_NO_CXX11_NOEXCEPT
        -DENABLE_CPP11=FALSE
    )

    IF(CMAKE_BUILD_TYPE STREQUAL "Release")
        SET(flint-daemon-cflags
            ${flint-daemon-cflags}
            -Wno-unused-variable
            -Wno-unused-parameter
            -Wl,-s
            -Wl,-X
        )
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")
ENDIF(UNIX)


STRING(REPLACE ";" " " flint-daemon-cflags "${flint-daemon-cflags}")

INCLUDE_DIRECTORIES(${flint-daemon-include})

LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

## executable
ADD_EXECUTABLE(flint-daemon ${flint-daemon-src})
TARGET_LINK_LIBRARIES(flint-daemon ${flint-daemon-libs})
SET_TARGET_PROPERTIES(flint-daemon PROPERTIES COMPILE_FLAGS "${flint-daemon-cflags}")