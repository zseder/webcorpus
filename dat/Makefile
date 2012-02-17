BIN_DIR = ../bin
SRC_DIR = ../src

# Dictionaries
define DICTS
polish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL
finnish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL
endef

TEXTCAT_CONFIG = /home/zseder/Sandbox/textcat/libtextcat-2.2/langclass/conf.txt
TEXTCAT_CONF_LIMIT = 1.05

#all: $(OUTS)
#	echo Done

# This is for storage.raw files if something is messed up while
# running WIRE and so data cannot be extracted with its wire-info-extract method
%.raw: %.not_extractable_wire
	cat $< | $(BIN_DIR)/get_htmls_from_wire | python $(BIN_DIR)/clean_wire_encoding.py> $@

%.raw: %.wire
	cat $< | python $(BIN_DIR)/wire_to_webcorp.py | python $(BIN_DIR)/clean_wire_encoding.py > $@

%.parsed: %.raw
	cat $< | $(BIN_DIR)/htmldetag | $(BIN_DIR)/htmlformat | python $(BIN_DIR)/html_entity_replace.py > $@

%.senfiltered: %.parsed
	cat $< | $(BIN_DIR)/split-sentences.perl -d $(SRC_DIR) | grep -v "^$$" | grep -v "^<[pP]>$$" | $(BIN_DIR)/sentencefilter > $@

%.langfiltered: %.senfiltered
	lang=`basename $< .senfiltered`;cat $< | $(BIN_DIR)/textcatfilter $(TEXTCAT_CONFIG) $$lang $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@
	
%.dedup: %.langfiltered
	cat $< | $(BIN_DIR)/docfilter | $(BIN_DIR)/dupfilter > $@

%._susp_chars: %.dedup %.bad
	python $(BIN_DIR)/ngrams.py $*.bad 3 > $*.bad.ngrams
	python $(BIN_DIR)/ngrams.py $*.dedup 3 > $*.good.ngrams
	python $(BIN_DIR)/get_susp_chars.py $*.good.ngrams $*.bad.ngrams > $@
	@echo "Choosing suspicious characters from candidates (_susp_chars) has to be done manually"

%.susp_chars: 
	@echo "Choosing suspicious characters from candidates (_susp_chars) has to be done manually"

%._similar: %.susp_chars %.bad.ngrams %.good.ngrams
	python $(BIN_DIR)/similar.py $*.good.ngrams $*.bad.ngrams $*.susp_chars > $@
	rm $*.bad.ngrams $*.good.ngrams

%.similar: %._similar
	@echo "Choosing similar character pairs (to be replaced) from candidates (_similar) has to be done manually"

%.susp_chars_replaced: %.similar %.senfiltered
	cat $*.senfiltered | python $(BIN_DIR)/simple_replacer.py $*.similar > $@

%.tok: %.dedup
	cat $< | $(BIN_DIR)/tokenizer.pl > $@

