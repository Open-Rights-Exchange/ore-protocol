add_contract(ore.usage_log ore.usage_log ${CMAKE_CURRENT_SOURCE_DIR}/src/ore.usage_log.cpp)

set_target_properties(ore.usage_log.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
