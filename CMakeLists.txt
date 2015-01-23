CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(sc_test)



INCLUDE_DIRECTORIES(
	./include	
	/Library/SystemC/systemc-2.3.1/include
)

LINK_DIRECTORIES(/Library/SystemC/systemc-2.3.1/lib-macosx64/)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ADD_LIBRARY(sctest ./src/sc_test.cpp)

TARGET_LINK_LIBRARIES(sctest
	systemc	
)

