TARGET = unload_profile

USER_C = ${TARGET:=.c}

all: $(TARGET)
.PHONY: all

$(TARGET): $(USER_C)
	gcc -Wall -o $(TARGET) $(USER_C) -L/root/libbpf/src -l:libbpf.a -lelf -lz

clean:
	- rm $(TARGET)
