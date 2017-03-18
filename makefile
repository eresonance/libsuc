
WARNINGS:= -Wall -Wextra -Wpointer-arith -Wno-sign-compare -Wcast-align -Werror


%: %.c %.h
	gcc -g -posix ${WARNINGS} -DSUC_TEST_MAIN -o $@ $< && ./$@

drmemory: sda_test.exe
	/c/usr/drmemory/bin/drmemory.exe -v sda_test.exe

clean:
	rm suc_*.exe

.PHONY:=drmemory clean
