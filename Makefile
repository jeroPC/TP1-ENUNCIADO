CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -Werror -pedantic -O2 -g
INCLUDES = -Isrc -I.
LDFLAGS ?=
OBJDIR  = build

TARGET  = tp1
SRC     = main.c src/tp1.c
OBJS    = $(addprefix $(OBJDIR)/,$(SRC:.c=.o))

TEST_TARGET = pruebas
TEST_SRC    = pruebas_alumno.c src/tp1.c
TEST_OBJS   = $(addprefix $(OBJDIR)/,$(TEST_SRC:.c=.o))

.PHONY: all clean run test run-tests valgrind help

all: $(TARGET)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(LDFLAGS) -o $@

run-tests: test
	./$(TEST_TARGET)

valgrind: test
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TARGET)

clean:
	$(RM) -r $(OBJDIR) $(TARGET) $(TEST_TARGET)

help:
	@echo "Targets:"
	@echo "  all (default)  Build main program"
	@echo "  run            Run main program"
	@echo "  test           Build tests binary"
	@echo "  run-tests      Build and run tests"
	@echo "  valgrind       Run tests with Valgrind (if installed)"
	@echo "  clean          Remove build artifacts"
	@echo "  help           Show this help"