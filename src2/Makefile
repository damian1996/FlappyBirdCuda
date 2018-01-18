CUDA_INSTALL_PATH ?= /usr/local/cuda
VER =
CXX := /usr/bin/g++$(VER)
CC := /usr/bin/gcc$(VER)
LINK := /usr/bin/g++$(VER) -fPIC
CCPATH := ./gcc$(VER)
NVCC  := $(CUDA_INSTALL_PATH)/bin/nvcc -ccbin $(CCPATH)

# Includes
INCLUDES = -I. -I$(CUDA_INSTALL_PATH)/include

# Libraries
LIB_ALLEGRO := -lallegro -lallegro_main -lallegro_acodec -lallegro_audio -lallegro_color -lallegro_font -lallegro_memfile -lallegro_physfs -lallegro_ttf -lallegro_primitives -lallegro_image -lallegro_dialog
LIB_CUDA := -L/usr/lib/nvidia-current -lcuda

# Options
NVCCOPTIONS = -ptx -Wno-deprecated-gpu-targets -std=c++14
CXXOPTIONS = -std=c++14 -g -Wall

# Common flags
COMMONFLAGS += $(INCLUDES)
NVCCFLAGS += $(COMMONFLAGS) $(NVCCOPTIONS)
CXXFLAGS += $(COMMONFLAGS) $(CXXOPTIONS)
CFLAGS += $(COMMONFLAGS)

CUDA_OBJS = Train.ptx
OBJS = NeuralNetwork.cpp.o Train.cpp.o Bot.cpp.o GameLogic.cpp.o GameRendering.cpp.o main.cpp.o
TARGET = solution.x
LINKLINE = $(LINK) -o $(TARGET) $(OBJS) $(LIB_ALLEGRO) $(LIB_CUDA) -lpthread

.SUFFIXES:	.c	.cpp	.cu	.o
%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.ptx: %.cu
	$(NVCC) $(NVCCFLAGS) $< -o $@

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): prepare $(OBJS) $(CUDA_OBJS)
	$(LINKLINE)

clean:
	rm -rf $(TARGET) *.o *.ptx

prepare:
	rm -rf $(CCPATH);\
	mkdir -p $(CCPATH);\
	ln -s $(CXX) $(CCPATH)/g++;\
	ln -s $(CC) $(CCPATH)/gcc;
