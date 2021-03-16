# Simple NTRIP server library.
add_library(ntrip
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
            request_parser.h)

# An example NTRIP server that forwards corrections from Polaris to connected
# NTRIP clients.
add_executable(ntrip_example ntrip_example.cc)
target_link_libraries(ntrip_example PUBLIC ntrip)
target_link_libraries(ntrip_example PUBLIC polaris_cpp_client)
target_link_libraries(ntrip_example PUBLIC ${Boost_LIBRARIES})

# Simple NTRIP client meant for testing the NTRIP server example.
add_executable(ntrip_example_client ntrip_example_test_client.cc)
target_link_libraries(ntrip_example_client PUBLIC ${GLOG_LIBRARIES})
target_link_libraries(ntrip_example_client PUBLIC ${GFLAGS_LIBRARIES})
target_link_libraries(ntrip_example_client PUBLIC ${Boost_LIBRARIES})
