add_contract(ore.rights_registry ore.rights_registry ${CMAKE_CURRENT_SOURCE_DIR}/src/ore.rights_registry.cpp)
target_include_directories(ore.rights_registry.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ore.rights_registry.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
