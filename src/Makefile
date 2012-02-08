BIN_DIR = ../bin
HTML_SCRIPTS = htmldetag htmlformat

all: build bin
	@echo Everything is ready

build: $(HTML_SCRIPTS)
	@echo Building..

bin: $(BIN_DIR) build
	@echo moving binaries...
	mv htmldetag htmlformat $(BIN_DIR)/
	cp html_entity_replace.py $(BIN_DIR)/
	if [ -e splitcode.py ]; then mv splitcode.py $(BIN_DIR)/; fi
	cp wire_to_webcorp.py $(BIN_DIR)/
	cp clean_wire_encoding.py file_coding_utils.py $(BIN_DIR)/
	cp split-sentences.perl $(BIN_DIR)/

clean:
	rm -f $(BIN_DIR)/$(HTML_SCRIPTS)
	rm -f $(BIN_DIR)/wire_to_webcorp.py
	rm -f $(BIN_DIR)/clean_wire_encoding.py $(BIN_DIR)/file_coding_utils.py
	rm -f $(BIN_DIR)/split-sentences.perl
	@echo Not removing splitcode. Remove by hand if really needed

$(BIN_DIR):
	mkdir $(BIN_DIR)

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



