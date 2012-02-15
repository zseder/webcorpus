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

%.raw: %.wire
	cat $< | python $(SCRIPTS)/clean_wire_encoding.py | python $(SCRIPTS)/wire_to_webcorp.py > $@

%.parsed: %.raw
	cat $< | $(SCRIPTS)/htmldetag | $(SCRIPTS)/htmlformat | python $(SCRIPTS)/html_entity_replace.py > $@

%.senfiltered: %.parsed
	cat $< | $(SCRIPTS)/split-sentences.perl -d $(SRC_DIR) | grep -v "^$$" | grep -v "^<[pP]>$$" | $(SCRIPTS)/sentencefilter > $@

%.langfiltered: %.senfiltered
	lang=`basename $< .senfiltered`;cat $< | $(SCRIPTS)/textcatfilter $(TEXTCAT_CONFIG) $$lang $(TEXTCAT_CONF_LIMIT) 2>$${lang}.bad > $@
	
%.dedup: %.langfiltered
	cat $< | $(SCRIPTS)/docfilter | $(SCRIPTS)/dupfilter > $@

%.repl: %.dedup %.bad
	python $(SCRIPTS)/ngrams/ngrams.py $*.bad 3 >/tmp/bad_ngrams
	python $(SCRIPTS)/ngrams/ngrams.py $*.dedup 3 >/tmp/good_ngrams
	python $(SCRIPTS)/ngrams/get_susp_chars.py /tmp/good_ngrams /tmp/bad_ngrams > /tmp/susp
	python $(SCRIPTS)/ngrams/similar.py /tmp/good_ngrams /tmp/bad_ngrams /tmp/susp > $@
	rm /tmp/bad_ngrams /tmp/good_ngrams

%.tok: %.dedup
	cat $< | $(SCRIPTS)/tokenizer.pl > $@

