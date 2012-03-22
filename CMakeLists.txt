project(Thumbnailer)

cmake_minimum_required(VERSION 2.8)

find_package(ITK)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif()

find_package(SimpleITK)
if(SimpleITK_FOUND)
  include(${SimpleITK_USE_FILE})
endif()

add_executable(Thumbnailer Thumbnailer.cxx)
target_link_libraries(Thumbnailer ${SimpleITK_LIBRARIES} ${ITK_LIBRARIES})

install(TARGETS Thumbnailer
  RUNTIME DESTINATION bin)
