.PHONY: ice4
ice4:
	sh build.sh "$(EXE)"

.PHONY: check
check:
	g++ -DOPENBENCH -Wall -Wextra -O -fdiagnostics-color=always -fsyntax-only src/main.cpp
