add_contract(ore.standard_token ore.standard_token ${CMAKE_CURRENT_SOURCE_DIR}/src/ore.standard_token.cpp)
target_include_directories(ore.standard_token.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ore.standard_token.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
