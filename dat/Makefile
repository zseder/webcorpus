BIN_DIR = ../bin
SRC_DIR = ../src

# Dictionaries
define DICTS
polish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL \
spanish:/home/zseder/Sandbox/HunspellDictionaries/Spanish/es_ES \
catalan:/home/zseder/Sandbox/HunspellDictionaries/Catalan/ca_ES
endef

TEXTCAT_CONFIG = /home/zseder/Sandbox/textcat/libtextcat-2.2/langclass/conf.txt
TEXTCAT_CONF_LIMIT = 1.1

HUNSPELL_RATIO_LIMIT = 0.6

BOILER_PLATE_LIMIT = 20 

#all: $(OUTS)
#	echo Done

# This is for storage.raw files if something is messed up while
# running WIRE and so data cannot be extracted with its wire-info-extract method
%.raw: %.not_extractable_wire
	time cat $< | $(BIN_DIR)/get_htmls_from_wire | python $(BIN_DIR)/clean_wire_encoding.py> $@

%.raw: %.wire
	time cat $< | python $(BIN_DIR)/wire_to_webcorp.py | python $(BIN_DIR)/clean_wire_encoding.py > $@

%.parsed: %.raw
	time cat $< | $(BIN_DIR)/htmldetag | $(BIN_DIR)/boilerplatefilter $(BOILER_PLATE_LIMIT) | $(BIN_DIR)/htmlformat | $(BIN_DIR)/html_entity_replacer > $@

%.senfiltered: %.parsed
	l=`basename $@ .senfiltered`; cd $(SRC_DIR); make sentence_tokenizer LANG=$$l; mv sentence_tokenizer $(BIN_DIR)/
	time cat $< | $(BIN_DIR)/sentence_tokenizer | grep -v "^$$" | grep -v "^<[pP]>$$" | sed "s/^\s*//g" | sed "s/\s*$$//g" | $(BIN_DIR)/sentencefilter | $(BIN_DIR)/docfilter > $@

%.langfiltered: %.senfiltered
	lang=`basename $< .senfiltered`; \
		d=`for e in $(DICTS); do echo $$e; done | grep "$$lang" | cut -f2 -d":"`;\
		if [ -z "$$d" ]; then \
		    time cat $< | $(BIN_DIR)/textcatfilter $(TEXTCAT_CONFIG) $$lang $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@; \
		else \
			time cat $< | $(BIN_DIR)/hunspellfilter $$d.aff $$d.dic $(HUNSPELL_RATIO_LIMIT) 2>$${lang}.bad > $@ ;\
	    fi

%.cleaned: %.langfiltered %.senfiltered
	@echo "WARNING: After cleaning, one has to run lang filtering again with cleaned_langfiltered target"
	l=`basename $@ .cleaned`; time cat $$l.senfiltered | $(BIN_DIR)/clean_encoding $$l.langfiltered > $@

%.cleaned_langfiltered: %.cleaned
	lang=`basename $< .cleaned`; \
		d=`for e in $(DICTS); do echo $$e; done | grep "$$lang" | cut -f2 -d":"`;\
		if [ -z "$$d" ]; then \
		    time cat $< | $(BIN_DIR)/textcatfilter $(TEXTCAT_CONFIG) $$lang $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@; \
		else \
			time cat $< | $(BIN_DIR)/hunspellfilter $$d.aff $$d.dic $(HUNSPELL_RATIO_LIMIT) 2>$${lang}.bad > $@ ;\
	    fi
	
%.dedup: %.cleaned_langfiltered
	time cat $< | $(BIN_DIR)/dupfilter > $@

#%.dedup: %.langfiltered
#	@echo WARNING: when running dupfilter after 1st phase of langfilter,
#	@echo there maybe wrong coded characters in the data
#	@echo It's not recommended to use this target
#	cat $< | $(BIN_DIR)/dupfilter > $@

%.neardedup: %.dedup
	time $(BIN_DIR)/neardupfilter $< > $@

%.tok: %.neardedup
	time cat $< | $(BIN_DIR)/word_tokenizer | sed "s/\ \ /\ /g" | sed "s/^\s*//g" | sed "s/\s*$$//g" > $@

