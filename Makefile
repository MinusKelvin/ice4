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

$(EXE): $(DEPS)
	g++ -DOPENBENCH -O3 -pthread src/main.cpp -o "$(EXE)"

logo.png: ice4 logo_template.png
	python3 make_ice4_logo.py < ice4

.PHONY: check
check:
	g++ -DOPENBENCH -Wall -Wextra -O -fdiagnostics-color=always -fsyntax-only src/main.cpp

.PHONY: clean
clean:
	rm -f ice4 ice4-tcec logo.png

.PHONY: all
all: $(EXE) ice4 ice4-tcec logo.png
