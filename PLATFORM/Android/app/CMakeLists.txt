cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

#FREETYPE2

set( lib_freetype2_src_DIR ${PROJECT_SOURCE_DIR}/../freetype2 )
set( lib_freetype2_lib_DIR ${lib_freetype2_src_DIR}/lib/${ANDROID_ABI}/libfreetype2-static.a )
add_library( freetype2-lib STATIC IMPORTED )
set_target_properties( freetype2-lib PROPERTIES IMPORTED_LOCATION
                       ${lib_freetype2_lib_DIR} )

include_directories( ${lib_freetype2_src_DIR}/include )
link_directories( ${lib_freetype2_lib_DIR} )

#FMOD

set( lib_mod_src_DIR ${PROJECT_SOURCE_DIR}/../FMOD )
add_library( fmod-lib SHARED IMPORTED )
set_target_properties( fmod-lib PROPERTIES IMPORTED_LOCATION
                       ${lib_mod_src_DIR}/lib/${ANDROID_ABI}/libfmod.so )

add_library( fmodL-lib SHARED IMPORTED )
set_target_properties( fmodL-lib PROPERTIES IMPORTED_LOCATION
                       ${lib_mod_src_DIR}/lib/${ANDROID_ABI}/libfmodL.so )

include_directories( ${lib_mod_src_DIR}/inc )
link_directories( ${lib_mod_src_DIR}/lib/${ANDROID_ABI}/libfmod.so )
link_directories( ${lib_mod_src_DIR}/lib/${ANDROID_ABI}/libfmodL.so )

#CORE

set( lib_core_src_DIR ${PROJECT_SOURCE_DIR}/../../../CORE )
set( lib_server_src_DIR ${PROJECT_SOURCE_DIR}/../../../SERVER )

#GLOBS

file(GLOB FURION_H "${lib_core_src_DIR}/iFurionGL/*.hpp")
file(GLOB FURION_CPP "${lib_core_src_DIR}/iFurionGL/*.cpp")
file(GLOB MATH_H "${lib_core_src_DIR}/Math/*.h")
file(GLOB MATH_CPP "${lib_core_src_DIR}/Math/*.cpp")
file(GLOB AI_H "${lib_core_src_DIR}/AI/*.hpp")
file(GLOB AI_CPP "${lib_core_src_DIR}/AI/*.cpp")
file(GLOB RENDERING_H "${lib_core_src_DIR}/Rendering/*.hpp")
file(GLOB RENDERING_CPP "${lib_core_src_DIR}/Rendering/*.cpp")
file(GLOB GAMELOGIC_H "${lib_core_src_DIR}/GameLogic/*.hpp")
file(GLOB GAMELOGIC_CPP "${lib_core_src_DIR}/GameLogic/*.cpp")
file(GLOB PATHFIND_H "${lib_core_src_DIR}/PathFinding/*.hpp")
file(GLOB PATHFIND_CPP "${lib_core_src_DIR}/PathFinding/*.cpp")

file(GLOB SERVER_UTIL_H "${lib_server_src_DIR}/Utils/*.hpp")
file(GLOB SERVER_UTIL_CPP "${lib_server_src_DIR}/Utils/*.cpp")
file(GLOB SERVER_CLIENT_H "${lib_server_src_DIR}/PFClient/*.hpp")
file(GLOB SERVER_CLIENT_CPP "${lib_server_src_DIR}/PFClient/*.cpp")
file(GLOB SERVER_SOCKET_H "${lib_server_src_DIR}/clsocket/*.h")
file(GLOB SERVER_SOCKET_CPP "${lib_server_src_DIR}/clsocket/*.cpp")

add_library( native-lib

             SHARED

             src/main/cpp/native-lib.cpp
             src/main/cpp/native-lib.h

             ${lib_core_src_DIR}/Core-pch.hpp
             ${lib_core_src_DIR}/Core-pch.cpp
             ${lib_core_src_DIR}/EnumOptionMacro.hpp

             ${FURION_H}
             ${FURION_CPP}
             ${MATH_H}
             ${MATH_CPP}
             ${AI_H}
             ${AI_CPP}
             ${RENDERING_H}
             ${RENDERING_CPP}
             ${GAMELOGIC_H}
             ${GAMELOGIC_CPP}
             ${PATHFIND_H}
             ${PATHFIND_CPP}

             ${SERVER_UTIL_H}
             ${SERVER_UTIL_CPP}
             ${SERVER_CLIENT_H}
             ${SERVER_CLIENT_CPP}
             ${SERVER_SOCKET_H}
             ${SERVER_SOCKET_CPP}

             ${lib_core_src_DIR}/utilities/BasePathExtraction.cpp
             ${lib_core_src_DIR}/utilities/BasePathExtraction.hpp
             ${lib_core_src_DIR}/utilities/CFrustum.hpp
             ${lib_core_src_DIR}/utilities/glMatrix4x4.cpp
             ${lib_core_src_DIR}/utilities/glMatrix4x4.hpp
             ${lib_core_src_DIR}/utilities/glTextureLoader.cpp
             ${lib_core_src_DIR}/utilities/glTextureLoader.hpp
             ${lib_core_src_DIR}/utilities/stb_image.h

             ${lib_core_src_DIR}/utilities/Timer/GameTimer.hpp
             ${lib_core_src_DIR}/utilities/Timer/GameTimer.cpp
             ${lib_core_src_DIR}/utilities/Timer/AndroidTimer.hpp
             ${lib_core_src_DIR}/utilities/Timer/AndroidTimer.cpp

             ${lib_core_src_DIR}/font/FontRender.cpp
             ${lib_core_src_DIR}/font/FontRender.hpp

             ${lib_core_src_DIR}/Shader/ShaderProgram.cpp
             ${lib_core_src_DIR}/Shader/ShaderProgram.hpp

             ${lib_core_src_DIR}/Audio/Audio.cpp
             ${lib_core_src_DIR}/Audio/Audio.hpp )

include_directories( ${lib_core_src_DIR} )
include_directories( ${lib_core_src_DIR}/iFurionGL )
include_directories( ${lib_core_src_DIR}/utilities )
include_directories( ${lib_core_src_DIR}/utilities/Timer )
include_directories( ${lib_core_src_DIR}/Math )
include_directories( ${lib_core_src_DIR}/font )
include_directories( ${lib_core_src_DIR}/Shader )
include_directories( ${lib_core_src_DIR}/Rendering )
include_directories( ${lib_core_src_DIR}/AI )
include_directories( ${lib_core_src_DIR}/GameLogic )
include_directories( ${lib_core_src_DIR}/PathFinding )
include_directories( ${lib_core_src_DIR}/Audio )

include_directories( ${lib_server_src_DIR}/Utils )
include_directories( ${lib_server_src_DIR}/PFClient )
include_directories( ${lib_server_src_DIR}/clsocket )

#Resources

set( ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets )
set( FONTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/res/font )

configure_file( ${lib_core_src_DIR}/Resources/Lato-Black.ttf ${ASSETS_DIR}/Lato-Black.ttf COPYONLY )

configure_file( ${lib_core_src_DIR}/Resources/FFFATLAN.TTF ${FONTS_DIR}/fffatlan.ttf COPYONLY )

configure_file( ${lib_core_src_DIR}/Resources/music/menu.mp3 ${ASSETS_DIR}/menu.mp3 COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/music/map.mp3 ${ASSETS_DIR}/map.mp3 COPYONLY )

configure_file( ${lib_core_src_DIR}/Resources/sfx/ok.wav ${ASSETS_DIR}/ok.wav COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/sfx/select.wav ${ASSETS_DIR}/select.wav COPYONLY )

configure_file( ${lib_core_src_DIR}/Resources/maps/Hunt.lgf ${ASSETS_DIR}/Hunt.lgf COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/Hunt.map ${ASSETS_DIR}/Hunt.map COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/pasage.lgf ${ASSETS_DIR}/pasage.lgf COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/pasage.map ${ASSETS_DIR}/pasage.map COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/kingofhill.lgf ${ASSETS_DIR}/kingofhill.lgf COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/kingofhill.map ${ASSETS_DIR}/kingofhill.map COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/tutorial.lgf ${ASSETS_DIR}/tutorial.lgf COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/tutorial.map ${ASSETS_DIR}/tutorial.map COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/waterway.lgf ${ASSETS_DIR}/waterway.lgf COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/maps/waterway.map ${ASSETS_DIR}/waterway.map COPYONLY )

configure_file( ${lib_core_src_DIR}/Resources/shaders/Mobile/tile.fsh ${ASSETS_DIR}/tile.fsh COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/shaders/Mobile/tile.vsh ${ASSETS_DIR}/tile.vsh COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/shaders/Mobile/fontsimple.fsh ${ASSETS_DIR}/fontsimple.fsh COPYONLY )
configure_file( ${lib_core_src_DIR}/Resources/shaders/Mobile/fontsimple.vsh ${ASSETS_DIR}/fontsimple.vsh COPYONLY )


message(STATUS "Coping textures to ${ASSETS_DIR}")

file( GLOB textureFiles "${lib_core_src_DIR}/Resources/textures/*.png" )

foreach(texture ${textureFiles})

    get_filename_component(TEX_NAME ${texture} NAME)

    if(NOT IS_DIRECTORY ${texture})

        configure_file( ${lib_core_src_DIR}/Resources/textures/${TEX_NAME}
                        ${ASSETS_DIR}/${TEX_NAME}
                        COPYONLY )

    endif(NOT IS_DIRECTORY ${texture})

endforeach(${textureFiles})

#NDK Libraries
find_library( log-lib
              log )

#link all libraries

target_link_libraries( native-lib
                       GLESv2
                       ${log-lib}
                       fmod-lib
                       fmodL-lib
                       freetype2-lib
                       EGL
                       -ljnigraphics
                       -landroid)

#COPY SO LIBRARIES

add_custom_command( TARGET native-lib POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${lib_mod_src_DIR}/lib/${ANDROID_ABI}/libfmod.so
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfmod.so )

add_custom_command( TARGET native-lib POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${lib_mod_src_DIR}/lib/${ANDROID_ABI}/libfmodL.so
                    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfmodL.so )