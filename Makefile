# MonitorMe make

# define any directories containing header files other than /usr/include
INCLUDES= -I/usr/local/include/opencv -I/usr/local/include/opencv2

# define library paths in addition to /usr/lib
LFLAGS= -L/usr/local/lib

# define any libraries to link into executable
LDLIBS= -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml \
		-lopencv_video -lopencv_features2d -lopencv_calib3d \
		-lopencv_objdetect -lopencv_contrib -lopencv_legacy \
		-lopencv_stitching -Libconfig -lconfig

.PHONY : all

all: server_run.exe 

server_run.exe: server_run.o ./lib/motion_detect.o
	g++ \
		 server_run.cpp \
		./src/opencv_dev/motion_detect.cpp \
		-o server_run.exe \
		$(INCLUDES) $(LFLAGS) $(LIBS)

server_run2.exe: server_run.o ./lib/motion_detect.o
	g++ \
		server_run.o \
		./lib/motion_detect.o \
		-o server_run.exe \
		$(INCLUDES) $(LFLAGS) $(LIBS)

server_run.o: server_run.cpp  
	g++ \
		-c server_run.cpp \
		-o server_run.o

motion_detect.o: ./src/opencv_dev/motion_detect.cpp
	g++ \
		-c ./src/opencv_dev/motion_detect.cpp \
		-o ./lib/motion_detect.o \
		$(INCLUDES) $(LFLAGS) $(LIBS)
clean:
	rm -f server_run.o
