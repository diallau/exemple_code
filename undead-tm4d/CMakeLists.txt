cmake_minimum_required(VERSION 2.6)

#option de deboguage

project(undead_game C)

option(ENABLE_DEBUG "debug options" ON)

if(ENABLE_DEBUG)
    set(CMAKE_C_FLAGS "-std=c99 -g -Wall")
else()
    set(CMAKE_C_FLAGS "-std=c99 -O3")
endif()


#ajout des executables-----------------------------------------

add_executable(
    undead_text
    undead_text.c
    game.h
    game_io.h
    )

add_executable(
    undead_solve
    solveur.c
    game.c
    game_io.c
    game.h
    game_io.h
    )

add_executable(
    test_el
    test_el.c
    )

add_executable(
    test_DG
    test_DG.c
    )
add_executable(
    test_bo
    test_bo.c
    )
add_executable(
    test_KA
    testKA.c
    )





#creation de la librairie libgama.a------------------------------

add_library(game game.c game_io.c) #

SET_TARGET_PROPERTIES(
    game
    PROPERTIES
    LINKER_LANGUAGE C
    )
#Génération des tests ---------------------------------------
include(CTest)
enable_testing()

add_test(tv1 test_el)
add_test(tv2 test_DG)
add_test(tv3 test_bo)
add_test(tv4 test_KA)


#liason des librairies aux executables-----------------------------------------

target_link_libraries(
    undead_text
    game
    )
target_link_libraries(
    test_el
    game
    )
target_link_libraries(
    test_DG
    game
    )
target_link_libraries(
    test_KA
    game
    )
target_link_libraries(
    test_bo
    game
    )
