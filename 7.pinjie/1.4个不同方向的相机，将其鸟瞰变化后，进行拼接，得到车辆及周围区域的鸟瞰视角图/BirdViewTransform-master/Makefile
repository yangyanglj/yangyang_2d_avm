BIN = main
SRC = $(wildcard *.c *.cpp)

CFLAGS += -I/usr/local/include \
	  -L/usr/local/lib 
#INCLUDEPATH +=/usr/local/lib/libopencv_features2d.so
CFLAGS +=  -lopencv_imgcodecs -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc \
-lopencv_calib3d -lopencv_features2d
CC = g++
$(BIN):$(SRC)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -f $(BIN)
