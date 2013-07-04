#include Makefile.inc
include Makefile.inc.sample

BIN_DIR = ../bin
RES_DIR = ../res

CC = gcc
CCPP = g++
CPPFLAGS = -Wall -pedantic -Wno-format -Ishash/src -O3

TEXTCAT_OPT = -I/$(TEXTCAT_INC_DIR)
CPPFLAGS += $(TEXTCAT_OPT)

FLEX_BINS = htmldetag htmlformat get_htmls_from_wire
FLEX_SRC = htmldetag.flex htmlformat.flex get_htmls_from_wire.flex
PYTHON_SCRIPTS = clean_wire_encoding.py wire_to_webcorp.py arctowire_std.py join_stemdict_freq.py freq.py
CPP_BINS = sentencefilter textcatfilter hunspellcoverage hunspellfilter docfilter dupfilter neardupfilter clean_encoding html_entity_replacer boilerplatefilter

LANG = undefined

all: build bin
	@echo Everything is ready

build: $(FLEX_BINS) $(CPP_BINS) sentence_tokenizer word_tokenizer 
	@echo Building complete.

bin: $(BIN_DIR) build
	@echo moving binaries...
	mv $(FLEX_BINS) $(BIN_DIR)
	cp $(PYTHON_SCRIPTS) $(BIN_DIR)
	mv $(CPP_BINS) $(BIN_DIR)
	if [ -e splitcode.py ]; then mv splitcode.py $(BIN_DIR)/; fi
	mv sentence_tokenizer $(BIN_DIR)/
	mv word_tokenizer $(BIN_DIR)/

clean:
	for f in $(FLEX_BINS) $(PYTHON_SCRIPTS) $(CPP_BINS); do rm -f $(BIN_DIR)/$$f; done
	rm -f $(BIN_DIR)/sentence_tokenizer
	rm -f $(BIN_DIR)/word_tokenizer
	rm -f $(BIN_DIR)/tokenizer.pl
	rm -f $(BIN_DIR)/*pyc
	cd shash/src; make clean
	@# leftovers
	rm -f *.o
	rm -f *pyc
	rm -f $(FLEX_BINS) $(CPP_BINS)
	rm -f lex.yy.*
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
	python flex_utf8_chardef_replacer.py $(RES_DIR)/latin_small_utf8_hex $(RES_DIR)/latin_capital_utf8_hex $(RES_DIR)/$(LANG).nbp $< > $@.flex
	flex $@.flex
	gcc -lfl lex.yy.c -o $@
	rm lex.yy.c
	rm $@.flex

word_tokenizer: word_tokenizer.flex.template
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

hunspellcoverage: hunspellcoverage.o splitcode.h
	$(CCPP) -o $@ hunspellcoverage.o $(HUNSPELL_LIB_OPT)
	rm hunspellcoverage.o

neardupfilter: neardupfilter.o splitcode.h simhash
	$(CCPP) -o $@ neardupfilter.o shash/src/simiw.o shash/src/lookup3.o shash/src/simi.o
	rm neardupfilter.o

splitcode.h: makesplitcode.sh 
	@echo Creating split code...
	./$< $@ splitcode.py

splitcode.py: makesplitcode.sh splitcode.h 

