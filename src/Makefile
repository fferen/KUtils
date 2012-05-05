CC = g++
CMP_FLAGS = -Wall -Wconversion -std=c++11 -I/opt/local/include
LINK_FLAGS = -L/opt/local/lib -lopencv_flann -lopencv_core -lopencv_calib3d -lopencv_features2d -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_legacy -lopencv_objdetect -lopencv_video -framework ApplicationServices -framework Foundation
OBJS_ = osx.o krandom.o cv.o argparse.o str.o ktime.o
LIB_NAME = kutils.a

PROF_FLAGS = -g -pg
RELEASE_FLAGS = -O2

VERSION_FLAGS = $(RELEASE_FLAGS)

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
