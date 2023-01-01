cmake_minimum_required(VERSION 3.25.1)

project(CanavarGraphicsEngineEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Qt/6.4.1/msvc2019_64/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)

add_compile_options(/bigobj)

file(GLOB SOURCES Src/*.cpp Include/*.h)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_executable(CanavarGraphicsEngineEditor ${SOURCES})

target_link_libraries(CanavarGraphicsEngineEditor CanavarGraphicsEngine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent ${LIBS})

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Editor/Resources\""
	"\"$(OutDir)Resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Editor/Resources\""
	"\"$(BuildDir)Resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Engine/Resources\""
	"\"$(OutDir)Resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Engine/Resources\""
	"\"$(BuildDir)Resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Dependencies/assimp/bin\""
	"\"$(OutDir)*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Dependencies/assimp/bin\""
	"\"$(BuildDir)*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND C:/Qt/6.4.1/msvc2019_64/bin/windeployqt.exe 
	"\"$(OutDir)$(TargetName)$(TargetExt)\""
)