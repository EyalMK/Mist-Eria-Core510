file(GLOB sources *.c *.h)

set(cwebsocket_STAT_SRCS
   ${sources}
)

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(cwebsocket STATIC ${cwebsocket_STAT_SRCS})
