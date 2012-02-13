SCRIPTS = ../bin
SRC_DIR = ../src

# Dictionaries
define DICTS
polish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL
finnish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL
endef

TEXTCAT_CONF = /home/zseder/Sandbox/textcat/libtextcat-2.2/langclass/conf.txt

#all: $(OUTS)
#	echo Done

%.cleaned_coding: %.wire
	cat $< | python $(SCRIPTS)/clean_wire_encoding.py > $@

%.parsed: %.cleaned_coding
	cat $< | python $(SCRIPTS)/wire_to_webcorp.py | $(SCRIPTS)/htmldetag | $(SCRIPTS)/htmlformat | python $(SCRIPTS)/html_entity_replace.py > $@

%.senfiltered: %.parsed
	cat $< | $(SCRIPTS)/split-sentences.perl -d $(SRC_DIR) | grep -v "^$$" | grep -v "^<[pP]>$$" | $(SCRIPTS)/sentencefilter > $@

%.langfiltered: %.senfiltered
	lang=`basename $< .senfiltered`;cat $< | $(SCRIPTS)/textcatfilter $(TEXTCAT_CONF) $$lang 2>$${lang}.wrong > $@
	
%.dedup: %.langfiltered
	cat $< | $(SCRIPTS)/docfilter | $(SCRIPTS)/dupfilter > $@

%.repl: %.dedup %.wrong
	python $(SCRIPTS)/ngrams/ngrams.py $*.wrong 3 >/tmp/wrong_ngrams
	python $(SCRIPTS)/ngrams/ngrams.py $*.dedup 3 >/tmp/good_ngrams
	python $(SCRIPTS)/ngrams/get_susp_chars.py /tmp/good_ngrams /tmp/wrong_ngrams > /tmp/susp
	python $(SCRIPTS)/ngrams/similar.py /tmp/good_ngrams /tmp/wrong_ngrams /tmp/susp > $@
	rm /tmp/wrong_ngrams /tmp/good_ngrams

%.tok: %.replaced
	cat $< | $(SCRIPTS)/tokenizer.pl > $@

