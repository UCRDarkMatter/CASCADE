cmake_minimum_required(VERSION 3.1)

project(SBTranslator)

# Find the Geant4 package and include its usage
find_package(Geant4 REQUIRED ui_all vis_all)

add_executable(SBTranslator SBTranslator.cc)

# Link the Geant4 libraries to your executable
target_link_libraries(SBTranslator ${Geant4_LIBRARIES})

# Include the Geant4 header files in your project
target_include_directories(SBTranslator PRIVATE ${Geant4_INCLUDE_DIRS})
