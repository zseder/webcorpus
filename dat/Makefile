SCRIPTS = ../bin
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
	cat $< | $(SCRIPTS)/get_htmls_from_wire | python $(SCRIPTS)/clean_wire_encoding.py> $@

%.raw: %.wire
	cat $< | python $(SCRIPTS)/wire_to_webcorp.py | python $(SCRIPTS)/clean_wire_encoding.py > $@

%.parsed: %.raw
	cat $< | $(SCRIPTS)/htmldetag | $(SCRIPTS)/htmlformat | python $(SCRIPTS)/html_entity_replace.py > $@

%.senfiltered: %.parsed
	cat $< | $(SCRIPTS)/split-sentences.perl -d $(SRC_DIR) | grep -v "^$$" | grep -v "^<[pP]>$$" | $(SCRIPTS)/sentencefilter > $@

%.langfiltered: %.senfiltered
	lang=`basename $< .senfiltered`;cat $< | $(SCRIPTS)/textcatfilter $(TEXTCAT_CONFIG) $$lang $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@
	
%.dedup: %.langfiltered
	cat $< | $(SCRIPTS)/docfilter | $(SCRIPTS)/dupfilter > $@

%._susp_chars: %.dedup %.bad
	python $(SCRIPTS)/ngrams.py $*.bad 3 > $*.bad.ngrams
	python $(SCRIPTS)/ngrams.py $*.dedup 3 > $*.good.ngrams
	python $(SCRIPTS)/get_susp_chars.py $*.good.ngrams $*.bad.ngrams > $@

%.susp_chars:
	@echo "Choosing suspicious characters from candidates (_susp_chars) has to be done manually"

%._similar: %.susp_chars %.bad.ngrams %.good.ngrams
	python $(SCRIPTS)/similar.py $*.good.ngrams $*.bad.ngrams $*.susp_chars > $@
	rm $*.bad.ngrams $*.good.ngrams

%.similar:
	@echo "Choosing similar character pairs (to be replaced) from candidates (_similar) has to be done manually"

%.susp_chars_replaced: %.similar %.senfiltered
	cat $*.senfiltered | python $(SCRIPTS)/simple_replacer.py $*.similar > $@

%.tok: %.dedup
	cat $< | $(SCRIPTS)/tokenizer.pl > $@

