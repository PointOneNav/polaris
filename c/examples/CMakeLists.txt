# Simple client example with no connection error detection logic.
add_executable(simple_polaris_client simple_polaris_client.c)
target_link_libraries(simple_polaris_client PUBLIC polaris_client)

# Example application including connection timeout and retry logic.
add_executable(connection_retry connection_retry.c)
target_link_libraries(connection_retry PUBLIC polaris_client)
