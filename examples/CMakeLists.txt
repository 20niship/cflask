add_executable(flask_test ${CMAKE_CURRENT_SOURCE_DIR}/flask_test.cpp)
target_link_libraries(flask_test dashcpp)

find_package(OpenCV)
if(${OpenCV_FOUND})
  include_directories(${OpenCV_INCLUDE_DIRS})
  add_executable(video ${CMAKE_CURRENT_SOURCE_DIR}/video.cpp)
  target_link_libraries(video dashcpp ${OpenCV_LIBS})
else()
  message("OpenCVが見つからなかったのでVideo streamingはビルドしません")
endif()

add_executable(graph ${CMAKE_CURRENT_SOURCE_DIR}/graph.cpp)
target_link_libraries(graph dashcpp Threads::Threads)

add_executable(draw ${CMAKE_CURRENT_SOURCE_DIR}/drawing.cpp)
target_link_libraries(draw dashcpp Threads::Threads)

add_executable(components ${CMAKE_CURRENT_SOURCE_DIR}/components.cpp)
target_link_libraries(components dashcpp Threads::Threads)

add_executable(rc22 ${CMAKE_CURRENT_SOURCE_DIR}/rc22.cpp)
target_link_libraries(rc22 dashcpp Threads::Threads)

add_executable(widget ${CMAKE_CURRENT_SOURCE_DIR}/widget.cpp)
target_link_libraries(widget dashcpp Threads::Threads)


