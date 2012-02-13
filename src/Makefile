BIN_DIR = ../bin
HTML_SCRIPTS = htmldetag htmlformat

CC = gcc
CCPP = g++
CPPFLAGS = -Wall -Wno-unknown-pragmas -Wno-long-long -Wno-variadic-macros \
		   -pedantic \
		   -Ilibs \
		   -Wno-format \
		   -g0 -O3
CFLAGS = $(CPPFLAGS) -std=gnu99

TEXTCAT_OPT = -I/home/zseder/Sandbox/textcat/libtextcat-2.2/src
TEXTCAT_LIB_OPT =-L/home/zseder/.local/lib -ltextcat -static
CPPFLAGS += $(TEXTCAT_OPT)

all: build bin
	@echo Everything is ready

build: $(HTML_SCRIPTS) sentencefilter textcatfilter docfilter dupfilter
	@echo Building..

bin: $(BIN_DIR) build
	@echo moving binaries...
	mv htmldetag htmlformat $(BIN_DIR)/
	cp html_entity_replace.py $(BIN_DIR)/
	if [ -e splitcode.py ]; then mv splitcode.py $(BIN_DIR)/; fi
	cp clean_wire_encoding.py file_coding_utils.py $(BIN_DIR)/
	cp wire_to_webcorp.py $(BIN_DIR)/
	cp split-sentences.perl $(BIN_DIR)/
	mv sentencefilter $(BIN_DIR)/
	mv textcatfilter $(BIN_DIR)/
	mv docfilter dupfilter $(BIN_DIR)/

clean:
	for f in $(HTML_SCRIPTS); do rm -f $(BIN_DIR)/$$f; done
	rm -f $(BIN_DIR)/html_entity_replace.py
	rm -f $(BIN_DIR)/clean_wire_encoding.py $(BIN_DIR)/file_coding_utils.py
	rm -f $(BIN_DIR)/wire_to_webcorp.py
	rm -f $(BIN_DIR)/split-sentences.perl
	rm -f $(BIN_DIR)/sentencefilter
	rm -f $(BIN_DIR)/*pyc
	rm -f $(BIN_DIR)/textcatfilter
	rm -f $(BIN_DIR)/docfilter
	rm -f $(BIN_DIR)/dupfilter
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

%.o: %.cc
	$(CCPP) -c $(CPPFLAGS) -o $@ $<

sentencefilter: sentencefilter.o splitcode.h
	$(CCPP) -o $@ sentencefilter.o
	rm sentencefilter.o

docfilter: docfilter.o splitcode.h
	$(CCPP) -o $@ docfilter.o
	rm docfilter.o

dupfilter: dupfilter.o splitcode.h
	$(CCPP) -o $@ dupfilter.o
	rm dupfilter.o

textcatfilter: textcatfilter.o splitcode.h
	$(CCPP) -o $@ textcatfilter.o $(TEXTCAT_LIB_OPT)
	rm textcatfilter.o

splitcode.h: makesplitcode.sh 
	@echo Creating split code...
	./$< $@ splitcode.py

splitcode.py: makesplitcode.sh splitcode.h 



