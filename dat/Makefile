SCRIPTS = ../bin
ENCODING = LATIN2
SRC_DIR = ../src

# Dictionaries
define DICTS
polish:/home/zseder/Sandbox/HunspellDictionaries/Polish/pl_PL
endef

#all: $(OUTS)
#	echo Done

%.cleaned_coding: %.wire
	cat $< | python $(SCRIPTS)/clean_wire_encoding.py > $@

%.parsed: %.cleaned_coding
	cat $< | python $(SCRIPTS)/wire_to_webcorp.py | $(SCRIPTS)/htmldetag | $(SCRIPTS)/htmlformat | python $(BIN_DIR)/html_entity_replace.py > $@

%.senfiltered: %.parsed
	#iconv -futf-8 -tutf-8 -c $< | $(SCRIPTS)/split-sentences.perl | $(SCRIPTS)/clean_sentence_splitter.sh | $(SCRIPTS)/sentencefilter > $@
	cat $< | $(SCRIPTS)/split-sentences.perl -d $(SRC_DIR) > $@

%.hunspellfiltered: %.senfiltered
	cat $< | python $(SCRIPTS)/vocab.py > /tmp/vocab
	bn=`basename $< .senfiltered`; dict=`echo $(DICTS) | grep $$bn | cut -f2 -d":"`; $(SCRIPTS)/hunspell.sh /tmp/vocab $$dict > /tmp/hunspell_out
	cat $< | python $(SCRIPTS)/filter_by_hunspell.py /tmp/hunspell_out 2>`basename $< .senfiltered`.wrong > $@
	rm /tmp/hunspell_out
	
%.dedup: %.hunspellfiltered
	cat $< | $(SCRIPTS)/docfilter | $(SCRIPTS)/dupfilter > $@

%.repl: %.dedup %.wrong
	python $(SCRIPTS)/ngrams/ngrams.py $*.wrong 3 >/tmp/wrong_ngrams
	python $(SCRIPTS)/ngrams/ngrams.py $*.dedup 3 >/tmp/good_ngrams
	python $(SCRIPTS)/ngrams/get_susp_chars.py /tmp/good_ngrams /tmp/wrong_ngrams > /tmp/susp
	python $(SCRIPTS)/ngrams/similar.py /tmp/good_ngrams /tmp/wrong_ngrams /tmp/susp > $@
	rm /tmp/wrong_ngrams /tmp/good_ngrams

%.tok: %.replaced
	cat $< | $(SCRIPTS)/tokenizer.pl > $@

