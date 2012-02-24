BIN_DIR = ../bin
SRC_DIR = ../src

# Dictionaries
define DICTS
polish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL \
spanish:/home/zseder/Sandbox/HunspellDictionaries/Spanish/es_ES
endef

TEXTCAT_CONFIG = /home/zseder/Sandbox/textcat/libtextcat-2.2/langclass/conf.txt
TEXTCAT_CONF_LIMIT = 1.01

HUNSPELL_RATIO_LIMIT = 0.8

NGRAM_LEN = 5

#all: $(OUTS)
#	echo Done

# This is for storage.raw files if something is messed up while
# running WIRE and so data cannot be extracted with its wire-info-extract method
%.raw: %.not_extractable_wire
	@#cat $< | $(BIN_DIR)/get_htmls_from_wire | python $(BIN_DIR)/clean_wire_encoding.py> $@
	cat $< | $(BIN_DIR)/get_htmls_from_wire > $@

%.raw: %.wire
	cat $< | python $(BIN_DIR)/wire_to_webcorp.py | python $(BIN_DIR)/clean_wire_encoding.py > $@

%.parsed: %.raw
	cat $< | $(BIN_DIR)/htmldetag | $(BIN_DIR)/htmlformat | python $(BIN_DIR)/html_entity_replace.py > $@

%.senfiltered: %.parsed
	#cat $< | $(BIN_DIR)/split-sentences.perl -d $(SRC_DIR) | grep -v "^$$" | grep -v "^<[pP]>$$" | $(BIN_DIR)/sentencefilter > $@
	cat $< | $(BIN_DIR)/sentence_tokenizer | grep -v "^$$" | grep -v "^<[pP]>$$" | sed "s/^\s*//g" | sed "s/\s*$//g" | $(BIN_DIR)/sentencefilter > $@

%.langfiltered: %.senfiltered
	@#lang=`basename $< .senfiltered`;cat $< | $(BIN_DIR)/textcatfilter $(TEXTCAT_CONFIG) $$lang $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@
	l=`basename $< .senfiltered`; \
	    d=`for e in $(DICTS); do echo $$e; done | grep "$$l" | cut -f2 -d":"`;\
		cat $< | $(BIN_DIR)/hunspellfilter $$d.aff $$d.dic $(HUNSPELL_RATIO_LIMIT) 2>$$l.bad > $@
	
%.dedup: %.langfiltered
	cat $< | $(BIN_DIR)/docfilter | $(BIN_DIR)/dupfilter > $@

%.tok: %.dedup
	cat $< | $(BIN_DIR)/tokenizer.pl > $@

