.PHONY: ice4
ice4:
	./build.sh "$(EXE)"

.PHONY: check
check:
	g++ -DOPENBENCH -Wno-narrowing -Wall -Wextra -O -fdiagnostics-color=always -fsyntax-only src/main.cpp
