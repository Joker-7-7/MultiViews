cmake_minimum_required(VERSION 3.5)

project(MultiViews VERSION 0.1 LANGUAGES CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Quick
    Multimedia
	Network
)

find_package(VTK 9.1 COMPONENTS REQUIRED)


file (GLOB_RECURSE Sources src/*.cpp)
file (GLOB_RECURSE Headers src/*.h)

file (GLOB         QtResources *.qrc)
#file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Gui/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} WIN32 ${Sources}  ${QtResources} ${Headers} main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Network
    PRIVATE Qt6::Multimedia
    PRIVATE ${VTK_LIBRARIES}
)

vtk_module_autoinit(
    TARGETS ${PROJECT_NAME}
    MODULES ${VTK_LIBRARIES}
)

