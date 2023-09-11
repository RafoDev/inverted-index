project(inverted-index)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
add_executable(counter-inverted-index counter-inverted-index.cc)
add_executable(inverted-index-mutex inverted-index-mutex.cc)
add_executable(inverted-index-merge inverted-index-merge.cc)
add_executable(test1 test1.cc)