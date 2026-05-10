# ── CVM++ Makefile ────────────────────────────────────────────────────────────

CXX      := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS  :=

SRC_DIR  := src
BUILD_DIR:= build
TARGET   := cvm

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

# ── Default target ──
all: $(TARGET)

# ── Link ──
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

# ── Compile (with auto-dependency tracking) ──
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ── Include auto-generated dependency files ──
-include $(DEPS)

# ── Debug build ──
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean all

# ── Release build ──
release: CXXFLAGS += -O2 -DNDEBUG
release: clean all

# ── Run tests ──
test: $(TARGET)
	@echo "━━━ Test 1: Arithmetic ━━━"
	@./$(TARGET) examples/01_arithmetic.cvm
	@echo ""
	@echo "━━━ Test 2: Variables ━━━"
	@./$(TARGET) examples/02_variables.cvm
	@echo ""
	@echo "━━━ Test 3: Control Flow ━━━"
	@./$(TARGET) examples/03_control_flow.cvm

# ── Clean ──
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean debug release test
