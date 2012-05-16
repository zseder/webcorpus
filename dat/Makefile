BIN_DIR = ../bin
SRC_DIR = ../src

include $(SRC_DIR)/Makefile.inc

TEXTCAT_CONF_LIMIT = 1.1

HUNSPELL_RATIO_LIMIT = 0.6

# used in docfilter
NUM_LINES_LIMIT = 4

BOILER_PLATE_LIMIT = 20 

#all: $(OUTS)
#	echo Done

# This is for storage.raw files if something is messed up while
# running WIRE and so data cannot be extracted with its wire-info-extract method
%.raw: %.not_extractable_wire
	time cat $< | $(BIN_DIR)/get_htmls_from_wire | python $(BIN_DIR)/clean_wire_encoding.py> $@

%.raw: %.wire
	time cat $< | python $(BIN_DIR)/wire_to_webcorp.py | python $(BIN_DIR)/clean_wire_encoding.py > $@

%.raw: %.arc.gz
	time zcat $< | python $(BIN_DIR)/arctowire_std.py | python $(BIN_DIR)/wire_to_webcorp.py | python $(BIN_DIR)/clean_wire_encoding.py > $@

%.raw: %.arc
	time cat $< | python $(BIN_DIR)/arctowire_std.py | python $(BIN_DIR)/wire_to_webcorp.py | python $(BIN_DIR)/clean_wire_encoding.py > $@
%.parsed: %.raw
	time cat $< | $(BIN_DIR)/htmldetag | $(BIN_DIR)/boilerplatefilter $(BOILER_PLATE_LIMIT) | $(BIN_DIR)/htmlformat | $(BIN_DIR)/html_entity_replacer > $@

%.senfiltered: %.parsed
	l=`basename $@ .senfiltered`; cd $(SRC_DIR); make sentence_tokenizer LANG=$$l; mv sentence_tokenizer $(BIN_DIR)/
	time cat $< | $(BIN_DIR)/sentence_tokenizer | grep -v "^$$" | grep -v "^<[pP]>$$" | sed "s/^\s*//g" | sed "s/\s*$$//g" | $(BIN_DIR)/sentencefilter | $(BIN_DIR)/docfilter $(NUM_LINES_LIMIT) > $@

%.langfiltered: %.senfiltered
	lang=`basename $< .senfiltered`; \
		d=`for e in $(DICTS); do echo $$e; done | grep "$$lang" | cut -f2 -d":"`;\
		if [ -z "$$d" ]; then \
		    time cat $< | $(BIN_DIR)/textcatfilter $(TEXTCAT_CONFIG) $${lang} $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@; \
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
		    time cat $< | $(BIN_DIR)/textcatfilter $(TEXTCAT_CONFIG) $${lang} $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@; \
		else \
			time cat $< | $(BIN_DIR)/hunspellfilter $$d.aff $$d.dic $(HUNSPELL_RATIO_LIMIT) 2>$${lang}.bad > $@ ;\
	    fi
	
%.dedup: %.cleaned_langfiltered
	time cat $< | $(BIN_DIR)/dupfilter > $@

%.dedup: %.langfiltered
	@echo WARNING: when running dupfilter after 1st phase of langfilter,
	@echo there maybe wrong coded characters in the data
	@echo see README for details in the root of repository
	cat $< | $(BIN_DIR)/dupfilter > $@

%.neardedup: %.dedup
	time $(BIN_DIR)/neardupfilter $< > $@

%.tok: %.neardedup
	time cat $< | $(BIN_DIR)/word_tokenizer | sed "s/\s+/\ /g" | sed "s/^\s*//g" | sed "s/\s*$$//g" > $@

%.freq: %.tok
	time cat $< | python $(BIN_DIR)/freq.py > $@

%.stemdict: %.freq
	lang=`basename $< .freq`; \
		d=`for e in $(DICTS); do echo $$e; done | grep "$$lang" | cut -f2 -d":"`;\
		if [ -z "$$d" ]; then \
		    echo "ERROR: if there is no hunspell dict provided, stemdict file has to be created manually!"; \
		else \
		    analyzed=`mktemp`; cat $< | awk '{print $2}' - | $(HUNSPELL) -s -d $$d -i utf-8 > $$analyzed; \
			words=`mktemp`; grep " " $$analyzed | cut -f1 -d" " > $$words; \
			encoding=`grep "^SET" $$d.aff | awk '{print $2}' -`; \
			stems=`mktemp`; grep " " $$analyzed | cut -f2 -d" " | iconv -f$$encoding -tutf-8 > $$stems; \
			pasted=`mktemp`; paste $$words $$stems > $$pasted; \
			python $(BIN_DIR)/join_stemdict_freq.py $$pasted $< > $@; \
			rm $$analyzed; rm $$words; rm $$stems; rm $$pasted; \
	    fi

