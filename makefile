PROGRAMM = RECAnalyzer
BUILD_DIR = bin
SRC_DIR = src
THIRDPARTY_DIR = thirdparty

CC = g++
CFLAGS = -std=gnu++17 -Wall -Werror -Wextra -pedantic
LDFLAGS = -lpthread

SRC = 
INCLUDES = 
DIRS = 
DEFS = FMT_HEADER_ONLY
PROG = 

ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), tests unit_tests))
	SRC += $(shell find $(SRC_DIR) -name '*.cpp' -not -path '$(SRC_DIR)/unix/*')
	INCLUDES += $(shell find $(SRC_DIR) -name '*.hpp'  -not -path '$(SRC_DIR)/unix/*')
	DIRS += $(shell find $(SRC_DIR)/ -type d -not -path '$(SRC_DIR)/unix')

	PROG = $(BUILD_DIR)/test_$(PROGRAMM)
	LDFLAGS += -lgtest
	DEFS += TESTS
else 
	SRC += $(shell find $(SRC_DIR) -name '*.cpp' -not -path '$(SRC_DIR)/tests/*')
	INCLUDES += $(shell find $(SRC_DIR) -name '*.hpp' -not -path '$(SRC_DIR)/tests/*')
	DIRS += $(shell find $(SRC_DIR)/ -type d -not -path '$(SRC_DIR)/tests')
	PROG = $(BUILD_DIR)/$(PROGRAMM)
endif

SRC +=  $(patsubst $(THIRDPARTY_DIR)/%, $(SRC_DIR)/$(THIRDPARTY_DIR)/%, $(shell find $(THIRDPARTY_DIR) -name '*.cpp'))
INCLUDES += $(shell find $(THIRDPARTY_DIR) -name '(*.hpp)|(*.h)')
DIRS += $(THIRDPARTY_DIR)/

OBJS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(patsubst %.cpp,%.o,$(SRC)))
DIRFLAGS = $(addprefix -I,$(DIRS))
DEFFLAGS = $(addprefix -D,$(DEFS))

ifeq ($(DEBUG),on)
CFLAGS += -g
DEFS += DEBUG
endif

ifeq ($(OPT),off)
CFLAGS += -O0
endif

all: clean

.PHONY: clean
clean:
	rm -rf bin

.PHONY: unix
unix: $(PROG)

.PHONY: tests
tests: unit_tests

.PHONY: unit_tests
unit_tests: $(PROG)
	$(PROG)

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/$(THIRDPARTY_DIR)/%.o: $(THIRDPARTY_DIR)/%.cpp $(INCLUDES)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(DIRFLAGS) $(DEFFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(DIRFLAGS) $(DEFFLAGS)  -o $@ $<

