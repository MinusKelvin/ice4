.PHONY: ice4
ice4:
	sh build.sh "$(EXE)"

.PHONY: check
check:
	g++ -DOPENBENCH -Wall -Wextra -O -fdiagnostics-color=always -fsyntax-only src/main.cpp

.PHONY: logo
logo:
	sh build.sh
	python3 make_ice4_logo.py < ice4
