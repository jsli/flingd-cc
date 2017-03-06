SET(RECEIVER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/receiver/src)

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

## flint-receiver-sdk-include
SET(flint-receiver-sdk-include
    ${RECEIVER_SRC}
    ${LIBS}/boost
    ${LIBS}/boost-process
    ${LIBS}/rapidjson/include
    ${LIBS}/websocketpp
)

## flint-receiver-sdk-src
SET(flint-receiver-sdk-src
	${RECEIVER_SRC}/utils/StringPiece.cpp
	${RECEIVER_SRC}/utils/LogStream.cpp
	${RECEIVER_SRC}/utils/Logging.cpp
	${RECEIVER_SRC}/utils/UUID.cpp
	${RECEIVER_SRC}/message/MessageBus.cpp
	${RECEIVER_SRC}/message/MessageChannel.cpp
	${RECEIVER_SRC}/FlintReceiverManager.cpp
	${RECEIVER_SRC}/MessageBusManager.cpp
	${RECEIVER_SRC}/ReceiverMessageChannel.cpp
	${RECEIVER_SRC}/IpcMessageChannel.cpp
	${RECEIVER_SRC}/ReceiverMessageBus.cpp
	${RECEIVER_SRC}/random_device.cpp
	${RECEIVER_SRC}/media/MediaManager.cpp
	${RECEIVER_SRC}/media/MediaPlayer.cpp
	${RECEIVER_SRC}/media/MediaInfo.cpp
)

## flint-receiver-sdk-libs
SET(flint-receiver-sdk-libs
    boost-atomic
    boost-system
    boost-thread
    boost-filesystem
    boost-regex
)
IF(CMAKE_BUILD_PLATFORM STREQUAL "linux")
    SET(flint-receiver-sdk-libs
        ${flint-receiver-sdk-libs}
        pthread
        )
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "linux")

IF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")
    SET(flint-receiver-sdk-libs
        ${flint-receiver-sdk-libs}
        rt
    )
ENDIF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")

IF(UXIX AND NOT APPLE)
    SET(flint-receiver-sdk-libs
        ${flint-receiver-sdk-libs}
        rt
    )
ENDIF(UXIX AND NOT APPLE)

## OSX, can override above.
IF (APPLE)
    ADD_DEFINITIONS (-DNDEBUG -Wall)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF ()

## flint-receiver-sdk-cflags
IF(UNIX)
    SET(flint-receiver-sdk-cflags
        ${flint-receiver-sdk-cflags}
        -Wall
        -frtti
        -fPIC
        -fexceptions
        -D_FILE_OFFSET_BITS=64
        -DBOOST_NO_CXX11_NOEXCEPT
        -DENABLE_CPP11=FALSE
        ${flint-receiver-sdk-cxx11-cflags}
    )

    IF(CMAKE_BUILD_TYPE STREQUAL "Release")
        SET(flint-receiver-sdk-cflags
            ${flint-receiver-sdk-cflags}
            -Wno-unused-variable
            -Wno-unused-parameter
            -Wl,-s
            -Wl,-X
        )
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")
ENDIF(UNIX)

STRING(REPLACE ";" " " flint-receiver-sdk-cflags "${flint-receiver-sdk-cflags}")

## set include files
INCLUDE_DIRECTORIES(${flint-receiver-sdk-include})

## set link files
LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(flint-receiver-sdk ${flint-receiver-sdk-src} ${CMAKE_CURRENT_SOURCE_DIR}/receiver/main.cpp)
TARGET_LINK_LIBRARIES(flint-receiver-sdk ${flint-receiver-sdk-libs})

ADD_LIBRARY(flint-receiver-sdk-static STATIC ${flint-receiver-sdk-src})
TARGET_LINK_LIBRARIES(flint-receiver-sdk-static ${flint-receiver-sdk-libs})
SET_TARGET_PROPERTIES(flint-receiver-sdk-static PROPERTIES OUTPUT_NAME "flint-receiver-sdk")

ADD_LIBRARY(flint-receiver-sdk-shared SHARED ${flint-receiver-sdk-src})
TARGET_LINK_LIBRARIES(flint-receiver-sdk-shared ${flint-receiver-sdk-libs})
SET_TARGET_PROPERTIES(flint-receiver-sdk-shared PROPERTIES OUTPUT_NAME "flint-receiver-sdk")

SET_TARGET_PROPERTIES(flint-receiver-sdk PROPERTIES COMPILE_FLAGS "${flint-receiver-sdk-cflags}")