CC = g++
CMP_FLAGS = -std=c++11 -Wall -Wsign-conversion -Wextra -Wno-unused-parameter -I/opt/local/include
LINK_FLAGS = -L/opt/local/lib -lopencv_flann -lopencv_core -lopencv_calib3d -lopencv_features2d -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_legacy -lopencv_objdetect -lopencv_video -framework ApplicationServices -framework Foundation

main: main.cpp ../lib/libkutils.a
	$(CC) -o main $(CMP_FLAGS) $(LINK_FLAGS) $^
