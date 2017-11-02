#
#	configuration variables for the example
CXXFLAGS += -std=c++11 -g -Og -Igtest/include 
LDFLAGS += -Lgtest/lib64 
LIBS += -lgtest -lpthread

## Main application file
MAIN = unittest
DEPH = 
DEPCPP = 

