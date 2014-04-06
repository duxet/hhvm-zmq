FIND_PATH(ZMQ_INCLUDE_DIR NAMES zmq.h PATHS /usr/include /usr/local/include)
FIND_LIBRARY(ZMQ_LIBRARY NAMES zmq PATHS /lib /usr/lib /usr/local/lib)

include_directories(${ZMQ_INCLUDE_DIR})

HHVM_EXTENSION(zmq zmq.cpp zmq_common.cpp)
HHVM_SYSTEMLIB(zmq zmq.php)

target_link_libraries(zmq ${ZMQ_LIBRARY})
