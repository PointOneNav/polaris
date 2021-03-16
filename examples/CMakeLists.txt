################################################################################
# Check for dependencies.
################################################################################

# Find Boost.
find_package(Boost REQUIRED system date_time)
include_directories(${Boost_INCLUDE_DIRS})

################################################################################
# Example Applications
################################################################################

# Simple example of connecting to the Polaris service.
add_executable(serial_port_example serial_port_example.cc)
target_link_libraries(serial_port_example PUBLIC polaris_cpp_client)
target_link_libraries(serial_port_example PUBLIC ${Boost_LIBRARIES})

# An example of obtaining receiver positions from a serial NMEA-0183 stream and
# forwarding incoming Polaris corrections over the same serial connection.
add_executable(septentrio_example septentrio_example.cc)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party third_party)
target_include_directories(septentrio PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
target_link_libraries(septentrio_example PUBLIC septentrio)
target_link_libraries(septentrio_example PUBLIC polaris_cpp_client)
target_link_libraries(septentrio_example PUBLIC ${Boost_LIBRARIES})

# Example NTRIP server.
add_subdirectory(ntrip)
