cmake_minimum_required(VERSION 3.10)
project(GruvPaper)

# Rechercher les bibliothèques nécessaires
find_package(X11 REQUIRED)
find_package(PNG REQUIRED)

# Ajouter les définitions nécessaires pour CImg
add_definitions(-Dcimg_display=1 -Dcimg_use_png)

# Ajouter les répertoires d'inclusion
include_directories(${X11_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Ajouter les fichiers source
add_executable(gruvpaper src/gruvpaper.cpp)

# Lier les bibliothèques
target_link_libraries(gruvpaper ${X11_LIBRARIES} ${PNG_LIBRARIES})

# Définir le répertoire de sortie pour les binaires
set_target_properties(gruvpaper PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
