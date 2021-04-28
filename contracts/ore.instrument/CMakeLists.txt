add_contract(ore.instrument ore.instrument ${CMAKE_CURRENT_SOURCE_DIR}/src/ore.instrument.cpp)
target_include_directories(ore.instrument.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ore.instrument.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
