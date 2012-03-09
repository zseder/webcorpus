BIN_DIR = ../bin
SRC_DIR = ../src

# Dictionaries
define DICTS
polish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL \
spanish:/home/zseder/Sandbox/HunspellDictionaries/Spanish/es_ES
endef

TEXTCAT_CONFIG = /home/zseder/Sandbox/textcat/libtextcat-2.2/langclass/conf.txt
TEXTCAT_CONF_LIMIT = 1.3

HUNSPELL_RATIO_LIMIT = 0.8

BOILER_PLATE_LIMIT = 20 

#all: $(OUTS)
#	echo Done

# This is for storage.raw files if something is messed up while
# running WIRE and so data cannot be extracted with its wire-info-extract method
%.raw: %.not_extractable_wire
	cat $< | $(BIN_DIR)/get_htmls_from_wire | python $(BIN_DIR)/clean_wire_encoding.py> $@

%.raw: %.wire
	cat $< | python $(BIN_DIR)/wire_to_webcorp.py | python $(BIN_DIR)/clean_wire_encoding.py > $@

%.parsed: %.raw
	cat $< | $(BIN_DIR)/htmldetag | $(BIN_DIR)/boilerplatefilter $(BOILER_PLATE_LIMIT) | $(BIN_DIR)/htmlformat | $(BIN_DIR)/html_entity_replacer > $@

%.senfiltered: %.parsed
	#cat $< | $(BIN_DIR)/split-sentences.perl -d $(SRC_DIR) | grep -v "^$$" | grep -v "^<[pP]>$$" | $(BIN_DIR)/sentencefilter > $@
	cat $< | $(BIN_DIR)/sentence_tokenizer | grep -v "^$$" | grep -v "^<[pP]>$$" | sed "s/^\s*//g" | sed "s/\s*$$//g" | $(BIN_DIR)/sentencefilter | $(BIN_DIR)/docfilter > $@

%.langfiltered: %.senfiltered
	lang=`basename $< .senfiltered`; \
		d=`for e in $(DICTS); do echo $$e; done | grep "$$lang" | cut -f2 -d":"`;\
		if [ -z "$$d" ]; then \
		    cat $< | $(BIN_DIR)/textcatfilter $(TEXTCAT_CONFIG) $$lang $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@; \
		else \
			cat $< | $(BIN_DIR)/hunspellfilter $$d.aff $$d.dic $(HUNSPELL_RATIO_LIMIT) 2>/dev/null > $@ ;\
	    fi

%.cleaned: %.langfiltered %.senfiltered
	@echo "After cleaning, we run lang filtering again"
# HACK Next line is copied from %.langiltered target, if we change that, change this, too!
	l=`basename $@ .cleaned`; \
	    d=`for e in $(DICTS); do echo $$e; done | grep "$$l" | cut -f2 -d":"`;\
		cat $$l.senfiltered | $(BIN_DIR)/clean_encoding $$l.langfiltered | $(BIN_DIR)/hunspellfilter $$d.aff $$d.dic $(HUNSPELL_RATIO_LIMIT) 2>/dev/null > $@
	
%.dedup: %.cleaned
	cat $< | $(BIN_DIR)/dupfilter > $@

%.neardedup: %.dedup
	cat $< | $(BIN_DIR)/neardupfilter > $@

%.tok: %.neardedup
	cat $< | $(BIN_DIR)/tokenizer.pl > $@

