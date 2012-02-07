BIN_DIR = ../bin
HTML_SCRIPTS = htmlattrib htmldetag htmlformat

all: build bin
	@echo Everything is ready

build: $(HTML_SCRIPTS)
	@echo Building..

bin: $(BIN_DIR) build
	@echo moving binaries...
	mv htmlattrib htmldetag htmlformat $(BIN_DIR)/
	mv splitcode.py $(BIN_DIR)/

clean:
	rm -f $(HTML_SCRIPTS)
	@echo Not removing splitcode. Remove by hand if really needed

$(BIN_DIR):
	mkdir $(BIN_DIR)

htmlattrib: splitcode.h htmlattrib.flex
	@echo Compiling htmlattrib...
	flex -i htmlattrib.flex # -i -> ignore case, -f -8 fast
	gcc -lfl lex.yy.c -o htmlattrib
	rm lex.yy.c
	@echo htmlattrib done

htmldetag: splitcode.h htmldetag.flex
	@echo Compiling htmldetag...
	flex -i htmldetag.flex # -i -> ignore case, -f -8 fast
	gcc -lfl lex.yy.c -o htmldetag
	rm lex.yy.c
	@echo htmldetag done

htmlformat: splitcode.h htmlformat.flex
	@echo Compiling htmldetag...
	flex -i htmlformat.flex # -i -> ignore case, -f -8 fast
	gcc -lfl lex.yy.c -o htmlformat
	rm lex.yy.c
	@echo htmldetag done

splitcode.h: makesplitcode.sh 
	@echo Creating split code...
	./$< $@ splitcode.py

splitcode.py: makesplitcode.sh splitcode.h 



