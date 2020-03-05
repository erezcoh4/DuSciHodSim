#
# **************************************************************
# *                                                            *
# **************************************************************
#

CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror -std=c++11

INCFLAGS  = -I.                       #Include itself

LDFLAGS  := -L/usr/lib -lstdc++ -lm
LDFLAGS += -L$(shell root-config --libdir) -lEG -lSpectrum -lHist -lTree
LDFLAGS += -L$(shell root-config --cflags --libs) -lPhysics -lGeom -lCFITSIO -lGraf3D -lXMLParser -lXMLIO
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := program
INCLUDE  := -Iinclude/ -I/usr/local/Cellar/root/6.18.04_1/include/root/

SRC      :=                      		\
   $(wildcard src/FaceModule/*.cpp) 	\
   $(wildcard src/BarModule/*.cpp) 		\
   $(wildcard src/WaveguideModule/*.cpp) 		\
   $(wildcard src/WaveguideMarginModule/*.cpp) 		\
   $(wildcard src/PhotonModule/*.cpp) 	\
   $(wildcard src/ProtonModule/*.cpp) 	\
   $(wildcard src/*.cpp)         		\

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) -o $(APP_DIR)/$(TARGET) $(OBJECTS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
