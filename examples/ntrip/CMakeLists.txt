ADD_LIBRARY(ntrip
connection.cc
connection.h
connection_manager.cc
connection_manager.h
header.h
mime_types.cc
mime_types.h
ntrip_server.cc
ntrip_server.h
reply.cc
reply.h
request.h
request_handler.cc
request_handler.h
request_parser.cc
request_parser.h )

TARGET_COMPILE_DEFINITIONS(ntrip PRIVATE NO_DECRYPTION=1)