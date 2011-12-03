PROJECT(MyPopstation CXX)
SET(CMAKE_C++_COMPILER g++)
ADD_EXECUTABLE(MyPopstation my_popstation.cpp)
TARGET_LINK_LIBRARIES(MyPopstation Zlib)
