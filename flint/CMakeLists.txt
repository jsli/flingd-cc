cmake_minimum_required(VERSION 2.8)
project(flint)
include(CMakeDependentOption)

SET(FLINT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

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

## flint-include
SET(flint-include
    ${FLINT_SRC}
    ${LIBS}/boost
    ${LIBS}/boost-process
    ${LIBS}/rapidjson/include
    ${LIBS}/websocketpp
)

## flint-src
SET(flint-src
	${FLINT_SRC}/FlintDaemon.cpp
	${FLINT_SRC}/core/Looper.cpp
	${FLINT_SRC}/utils/StringPiece.cpp
	${FLINT_SRC}/utils/LogStream.cpp
	${FLINT_SRC}/utils/Logging.cpp
	${FLINT_SRC}/utils/UUID.cpp
	${FLINT_SRC}/utils/StringUtils.cpp
	${FLINT_SRC}/http/HTTPServer.cpp
	${FLINT_SRC}/http/UrlParser.cpp
	${FLINT_SRC}/tcp/TCPServer.cpp
	${FLINT_SRC}/udp/UDPServer.cpp
	${FLINT_SRC}/flint/FlintServer.cpp
	${FLINT_SRC}/handler/HttpHandler.cpp
	${FLINT_SRC}/handler/WebsocketHandler.cpp
	${FLINT_SRC}/flint/handler/DeviceDescHandler.cpp
	${FLINT_SRC}/flint/handler/ApplicationControlHandler.cpp
	${FLINT_SRC}/flint/handler/ReceiverApplicationHandler.cpp
	${FLINT_SRC}/flint/handler/SystemControlHandler.cpp
	${FLINT_SRC}/flint/handler/DialHandler.cpp
	${FLINT_SRC}/platform/Platform.cpp
	${FLINT_SRC}/platform/PlatformUnix.cpp
	${FLINT_SRC}/platform/PlatformDarwin.cpp
	${FLINT_SRC}/platform/PlatformLinux.cpp
	${FLINT_SRC}/platform/PlatformPal.cpp
	${FLINT_SRC}/platform/PlatformFfos.cpp
	${FLINT_SRC}/platform/PlatformAndroid.cpp
	${FLINT_SRC}/discovery/FlintDiscovery.cpp
	${FLINT_SRC}/discovery/ssdp/SSDPServer.cpp
	${FLINT_SRC}/discovery/mdns/MDNSServer.cpp
	${FLINT_SRC}/discovery/mdns/MDNS.cpp
	${FLINT_SRC}/discovery/mdns/Buffer.cpp
	${FLINT_SRC}/discovery/mdns/Reader.cpp
	${FLINT_SRC}/discovery/mdns/Writer.cpp
	${FLINT_SRC}/discovery/mdns/Composer.cpp
	${FLINT_SRC}/flint/application/DialConfig.cpp
	${FLINT_SRC}/flint/application/Application.cpp
	${FLINT_SRC}/flint/application/ApplicationManager.cpp
	${FLINT_SRC}/flint/session/SessionManager.cpp
	${FLINT_SRC}/flint/session/Session.cpp
	${FLINT_SRC}/flint/session/SenderSession.cpp
	${FLINT_SRC}/flint/session/ReceiverSession.cpp
	${FLINT_SRC}/comm/IServer.cpp
	${FLINT_SRC}/ws/WebsocketServer.cpp
	${FLINT_SRC}/ws/WsServer.cpp
	${FLINT_SRC}/peer/PeerServer.cpp
	${FLINT_SRC}/net/NetworkManager.cpp
)



## flint-libs
SET(flint-libs
    boost-atomic
    boost-system
    boost-thread
    boost-filesystem
    boost-regex
)

IF(ANDROID)
    SET(flint-libs
        ${flint-libs}
        log
    )
    
    SET(flint-src 
    	${flint-src} 
    	${FLINT_SRC}/utils/ifaddrs.c
 	)
ENDIF(ANDROID)

IF(FFOS)
    SET(flint-libs
        ${flint-libs}
        log
    )
    
    SET(flint-src 
    	${flint-src} 
    	${FLINT_SRC}/utils/ifaddrs.c
 	)
ENDIF(FFOS)

IF(CMAKE_BUILD_PLATFORM STREQUAL "ffos")
	SET(flint-src 
    	${flint-src} 
    	${FLINT_SRC}/utils/ifaddrs.c
 	)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "ffos")

IF(CMAKE_BUILD_PLATFORM STREQUAL "linux")
	SET(flint-libs
		${flint-libs}
		pthread
		)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "linux")

IF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")
    SET(flint-libs
        ${flint-libs}
        rt
    )
ENDIF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")

IF(UXIX AND NOT APPLE)
    SET(flint-libs
        ${flint-libs}
        rt
    )
ENDIF(UXIX AND NOT APPLE)

IF (APPLE)
    ADD_DEFINITIONS (-DNDEBUG -Wall)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF(APPLE)

## flint-cflags
IF (UNIX)
    SET(flint-cflags
        ${flint-cflags}
        -Wall
        -frtti
        -fexceptions
        -fPIC
        -D_FILE_OFFSET_BITS=64
        -DBOOST_NO_CXX11_NOEXCEPT
        -DENABLE_CPP11=FALSE
    )

    IF(CMAKE_BUILD_TYPE STREQUAL "Release")
        SET(flint-cflags
            ${flint-cflags}
            -Wno-unused-variable
            -Wno-unused-parameter
            -Wl,-s
            -Wl,-X
        )
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")
ENDIF(UNIX)


STRING(REPLACE ";" " " flint-cflags "${flint-cflags}")

INCLUDE_DIRECTORIES(${flint-include})

LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

## executable
#ADD_EXECUTABLE(flint ${flint-src} ${FLINT_SRC}/main.cpp)
#TARGET_LINK_LIBRARIES(flint ${flint-libs})

## static lib
ADD_LIBRARY(flint_static STATIC ${flint-src})
TARGET_LINK_LIBRARIES(flint_static ${flint-libs})
SET_TARGET_PROPERTIES(flint_static PROPERTIES OUTPUT_NAME "flint")
SET_TARGET_PROPERTIES(flint_static PROPERTIES COMPILE_FLAGS "${flint-cflags}")

## shared lib
IF(ANDROID)
    ADD_LIBRARY(flint_shared SHARED ${flint-src} ${FLINT_SRC}/Flint.cpp)
ELSE(ANDROID)
	ADD_LIBRARY(flint_shared SHARED ${flint-src})
ENDIF(ANDROID)
TARGET_LINK_LIBRARIES(flint_shared ${flint-libs})
SET_TARGET_PROPERTIES(flint_shared PROPERTIES OUTPUT_NAME "flint")
SET_TARGET_PROPERTIES(flint_shared PROPERTIES COMPILE_FLAGS "${flint-cflags}")
