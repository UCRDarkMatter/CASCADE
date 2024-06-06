cmake_minimum_required(VERSION 3.1)

project(ASCIITranslator)

# Find the Geant4 package and include its usage
find_package(Geant4 REQUIRED ui_all vis_all)

add_executable(ASCIITranslator ASCIITranslator.cc)

# Link the Geant4 libraries to your executable
target_link_libraries(ASCIITranslator ${Geant4_LIBRARIES})

# Include the Geant4 header files in your project
target_include_directories(ASCIITranslator PRIVATE ${Geant4_INCLUDE_DIRS})
