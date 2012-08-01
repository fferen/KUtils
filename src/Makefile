##CC = clang++-mp-3.1
##CMP_FLAGS = -std=c++11 -Wall -Wsign-conversion -Wextra -Wno-unused-parameter -Wno-lambda-extensions -I/opt/local/include -stdlib=libc++ -U__STRICT_ANSI__
CC = /opt/local/bin/g++
CMP_FLAGS = -std=c++11 -Wall -Wsign-conversion -Wextra -Wno-unused-parameter -I/opt/local/include
LINK_FLAGS = -L/opt/local/lib -lopencv_flann -lopencv_core -lopencv_calib3d -lopencv_features2d -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_legacy -lopencv_objdetect -lopencv_video -framework ApplicationServices -framework Foundation
OBJS_ = $(patsubst %.cpp, %.o, $(shell ls *.cpp))
LIB_NAME = libkutils.a

PROF_FLAGS = -g -pg
RELEASE_FLAGS = -O2

VERSION_FLAGS = $(RELEASE_FLAGS) # $(PROF_FLAGS)

LIB_DIR = ../lib
OBJ_DIR = ../obj

OBJS = $(patsubst %,$(OBJ_DIR)/%,$(OBJS_))

lib: $(LIB_DIR)/$(LIB_NAME)

$(LIB_DIR)/$(LIB_NAME): $(OBJS)
	rm -f $@
	ar rvs $@ $^

$(OBJ_DIR)/%.o: %.cpp %.hpp
	$(CC) -c -o $@ $(CMP_FLAGS) $(VERSION_FLAGS) $<

clean:
	rm -f $(LIB_DIR)/*
	rm -f $(OBJ_DIR)/*
