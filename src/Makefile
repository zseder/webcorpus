BIN_DIR = ../bin
RES_DIR = ../res

CC = gcc
CCPP = g++
CPPFLAGS = -Wall -pedantic -Wno-format -Ishash/src -O3

TEXTCAT_OPT = -I/home/zseder/Sandbox/textcat/libtextcat-2.2/src
TEXTCAT_LIB_OPT =-L/home/zseder/.local/lib -ltextcat -static
CPPFLAGS += $(TEXTCAT_OPT)

HUNSPELL_LIB_OPT =-lhunspell-1.3 -L/home/zseder/.local/lib -static

FLEX_BINS = htmldetag htmlformat get_htmls_from_wire
FLEX_SRC = htmldetag.flex htmlformat.flex get_htmls_from_wire.flex
PYTHON_SCRIPTS = clean_wire_encoding.py wire_to_webcorp.py
CPP_BINS = sentencefilter textcatfilter hunspellfilter docfilter dupfilter neardupfilter clean_encoding html_entity_replacer

all: build bin
	@echo Everything is ready

build: $(FLEX_BINS) $(CPP_BINS) sentence_tokenizer 
	@echo Building complete.

bin: $(BIN_DIR) build
	@echo moving binaries...
	mv $(FLEX_BINS) $(BIN_DIR)
	cp $(PYTHON_SCRIPTS) $(BIN_DIR)
	mv $(CPP_BINS) $(BIN_DIR)
	if [ -e splitcode.py ]; then mv splitcode.py $(BIN_DIR)/; fi
	#cp split-sentences.perl $(BIN_DIR)/
	mv sentence_tokenizer $(BIN_DIR)/
	cp tokenizer.pl $(BIN_DIR)/

clean:
	for f in $(FLEX_BINS) $(PYTHON_SCRIPTS) $(CPP_BINS); do rm -f $(BIN_DIR)/$$f; done
	rm -f $(BIN_DIR)/split-sentences.perl
	rm -f $(BIN_DIR)/sentence_tokenizer
	rm -f $(BIN_DIR)/tokenizer.pl
	rm -f $(BIN_DIR)/*pyc
	cd shash/src; make clean
	@# leftovers
	rm -f *.o
	rm -f *pyc
	rm -f $(FLEX_BINS) $(CPP_BINS)
	@echo Not removing splitcode. Remove by hand if really needed

simhash:
	if [ ! -e shash ]; then git clone https://github.com/vilda/shash.git; fi
	cd shash/src; make

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(FLEX_BINS): splitcode.h $(FLEX_SRC)
	flex -i `basename $@.flex`
	gcc -lfl lex.yy.c -o $@
	rm lex.yy.c

sentence_tokenizer: sentence_tokenizer.flex.template
	python flex_utf8_chardef_replacer.py $(RES_DIR)/latin_small_utf8_hex $(RES_DIR)/latin_capital_utf8_hex $< > $@.flex
	flex $@.flex
	gcc -lfl lex.yy.c -o $@
	rm lex.yy.c
	rm $@.flex

%.o: %.cc
	$(CCPP) -c $(CPPFLAGS) -o $@ $<

sentencefilter docfilter dupfilter clean_encoding html_entity_replacer: sentencefilter.o splitcode.h sentencefilter.o docfilter.o dupfilter.o clean_encoding.o html_entity_replacer.o
	$(CCPP) -o $@ $@.o
	rm $@.o

textcatfilter: textcatfilter.o splitcode.h
	$(CCPP) -o $@ textcatfilter.o $(TEXTCAT_LIB_OPT)
	rm textcatfilter.o

hunspellfilter: hunspellfilter.o splitcode.h
	$(CCPP) -o $@ hunspellfilter.o $(HUNSPELL_LIB_OPT)
	rm hunspellfilter.o

neardupfilter: neardupfilter.o splitcode.h simhash
	$(CCPP) -o $@ neardupfilter.o shash/src/simiw.o shash/src/lookup3.o shash/src/simi.o
	rm neardupfilter.o

splitcode.h: makesplitcode.sh 
	@echo Creating split code...
	./$< $@ splitcode.py

splitcode.py: makesplitcode.sh splitcode.h 

