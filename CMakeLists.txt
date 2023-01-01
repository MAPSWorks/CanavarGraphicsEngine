cmake_minimum_required(VERSION 3.25.1)

project(Canavar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Qt/6.4.1/msvc2019_64/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INCLUDE_DIR Dependencies/qtimgui/include/src 
				Dependencies/qtimgui/include/modules/imgui 
				Dependencies/assimp/include
				Dependencies/JSBSim/include
				)
				
set(LIBS_DIR Dependencies/qtimgui/lib
			 Dependencies/assimp/lib
			 Dependencies/JSBSim/lib
			 )
			 
set(LIBS imgui 
		 implot 
		 qt_imgui_widgets 
		 assimp-vc142-mt 
		 zlibstatic
		 )

link_directories(${LIBS_DIR})
include_directories(${INCLUDE_DIR})

add_subdirectory(Engine Engine)
add_subdirectory(Editor Editor)
add_subdirectory(Simulator Simulator)