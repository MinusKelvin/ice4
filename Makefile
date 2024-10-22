ifdef EXE
default: $(EXE)
else
default: ice4
EXE = ice4-ob
endif

DEPS := $(wildcard src/** minifier/** Cargo.toml build.rs)

ice4: launcher.sh $(DEPS)
	cargo run --release | ./compress.sh | cat launcher.sh - >ice4
	chmod +x ice4
	@echo Compressed size: `du -b ice4`

ice4-tcec: launcher.sh $(DEPS)
	cargo run --release tcec | ./compress.sh | cat launcher.sh - >ice4-tcec
	chmod +x ice4-tcec
	@echo Compressed size: `du -b ice4-tcec`

ice4.exe: $(DEPS)
	x86_64-w64-mingw32-g++ -Wl,--stack,16777216 -DOPENBENCH -O3 -pthread -static src/main.cpp -o "$@"

$(EXE): $(DEPS)
	g++ -DOPENBENCH -g -O3 -pthread src/main.cpp -o "$@"

ice4-min-ob: $(DEPS)
	cargo run --release openbench | g++ -O3 -pthread -xc++ -o "$@" -

ice4-tunable: $(DEPS)
	g++ -DOPENBENCH -DTUNABLE -g -O3 -pthread src/main.cpp -o "$@"

ice4-asan: $(DEPS)
	g++ -DOPENBENCH -g -fsanitize=address -pthread src/main.cpp -o "$@"

ice4-usan: $(DEPS)
	g++ -DOPENBENCH -g -fsanitize=undefined -pthread src/main.cpp -o "$@"

logo.png: ice4 logo_template.png
	python3 make_ice4_logo.py < ice4

.PHONY: check
check:
	g++ -DOPENBENCH -Wall -Wextra -O -fdiagnostics-color=always -fsyntax-only src/main.cpp

.PHONY: clean
clean:
	rm -f $(EXE) ice4 ice4.exe ice4-tcec logo.png

.PHONY: all
all: $(EXE) ice4 ice4.exe ice4-tcec logo.png
