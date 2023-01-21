#表示用的编译器
CC = g++
 
#表示编译时的动作，-Wall 小心的， -g可调试的。
CC_FLAGS = -Wall -g
 
# 加载动态库
SO = -lpthread
 
#存放编译文件
BUILD_DIR = build
 
#加载编译文件
SRC_DIR = . ./base ./components/fsm ./components/config \
				./components/audio_manage
SRCS = $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))


#加载头文件 
INC_DIR += . ./base  ./components/config ./components/fsm \
			./components/audio_manage
INCS = $(patsubst %, -I%, $(INC_DIR))

 
#执行动作
$(BUILD_DIR)/main:$(SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $^ $(INCS) -o $@ $(SO)

#伪目标
.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)