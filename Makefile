ifdef EXE
default: $(EXE)
.PHONY: $(.EXE)
else
default: ice4-ob
endif

DEPS := $(wildcard src/**.cpp minifier/** Cargo.toml build.rs)

ifdef EVALFILE
	EVALPATH := $(realpath $(EVALFILE))
	EVALHASHFILE := $(shell printf '.%s.nnhash' `sha256sum $(EVALPATH) | cut -d' ' -f1`)
	EVALFLAGS = -DEVALFILE='"$(EVALPATH)"'
else
	EVALHASHFILE := .none.nnhash
endif

REPLAY_BUFFER_SIZE = 500000

%.nnhash:
	rm -f .*.nnhash
	touch $@

ice4-tcec: launcher.sh $(DEPS)
	cargo run --release tcec | ./compress.sh | cat launcher.sh - >ice4-tcec
	chmod +x ice4-tcec
	@echo Compressed size: `du -b ice4-tcec`

ice4.exe: $(DEPS) $(EVALHASHFILE)
	x86_64-w64-mingw32-g++ -Wl,--stack,16777216 -DOPENBENCH $(EVALFLAGS) -O3 -pthread -static -std=c++20 src/main.cpp -o "$@"

$(EXE): $(DEPS)
	g++ -DOPENBENCH -DAVOID_ADJUDICATION -DREPLAY_BUFFER_SIZE=$(REPLAY_BUFFER_SIZE) $(EVALFLAGS) -g -O3 -pthread -std=c++20 src/main.cpp -o "$@"

ice4-ob: $(DEPS) $(EVALHASHFILE)
	g++ -DOPENBENCH $(EVALFLAGS) -DREPLAY_BUFFER_SIZE=$(REPLAY_BUFFER_SIZE) -g -O3 -pthread -std=c++20 src/main.cpp -o "$@"

ice4-asan: $(DEPS) $(EVALHASHFILE)
	g++ -DOPENBENCH $(EVALFLAGS) -g -fsanitize=address -pthread -std=c++20 src/main.cpp -o "$@"

ice4-usan: $(DEPS) $(EVALHASHFILE)
	g++ -DOPENBENCH $(EVALFLAGS) -g -fsanitize=undefined -pthread -std=c++20 src/main.cpp -o "$@"

ice4-tsan: $(DEPS) $(EVALHASHFILE)
	g++ -DOPENBENCH $(EVALFLAGS) -g -fsanitize=thread -pthread -std=c++20 src/main.cpp -o "$@"

logo.png: ice4-tcec logo_template.png
	python3 make_ice4_logo.py < ice4-tcec

.PHONY: check
check:
	g++ -DOPENBENCH -Wall -Wextra -O -fdiagnostics-color=always -fsyntax-only -std=c++20 src/main.cpp

.PHONY: clean
clean:
	rm -f ice4-ob ice4.exe ice4-tcec logo.png

.PHONY: all
all: ice4-ob ice4.exe ice4-tcec logo.png
