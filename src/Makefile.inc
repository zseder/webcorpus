# Dictionaries - change to local files
define DICTS
lang1:filename_without_aff_or_dic \
langn:filename_without_aff_or_dic
endef

# Hunspell binary: set to installed hunspell binary
#HUNSPELL = /usr/bin/hunspell

# Change next to point at textcats dir and uncomment
#TEXTCAT_DIR = 
#TEXTCAT_INC_DIR = $(TEXTCAT_DIR)/src
#TEXTCAT_LIB_DIR =
#TEXTCAT_CONFIG = $(TEXTCAT_DIR)/langclass/conf.txt
#TEXTCAT_LIB_OPT = -L$(TEXTCAT_LIB_DIR) -ltextcat -static

# Change next to point at hunspell lib dir and uncomment
#HUNSPELL_LIB_VER = hunspell-1.3
#HUNSPELL_LIB_DIR = 
#HUNSPELL_LIB_OPT =-l$(HUNSPELL_LIB_VER) -L$(HUNSPELL_LIB_DIR) -static


SRC_DIR ?= .
ifndef HUNSPELL
$(error Define HUNSPELL in $(SRC_DIR)/Makefile.inc before proceeding)
endif

ifndef TEXTCAT_DIR
$(error Define TEXTCAT_DIR in $(SRC_DIR)/Makefile.inc before proceeding)
endif
ifndef TEXTCAT_INC_DIR
$(error Define TEXTCAT_INC_DIR in $(SRC_DIR)/Makefile.inc before proceeding)
endif
ifndef TEXTCAT_LIB_DIR
$(error Define TEXTCAT_LIB_DIR in $(SRC_DIR)/Makefile.inc before proceeding)
endif
ifndef TEXTCAT_CONFIG
$(error Define TEXTCAT_CONFIG in $(SRC_DIR)/Makefile.inc before proceeding)
endif
ifndef TEXTCAT_LIB_OPT
$(error Define TEXTCAT_LIB_OPT in $(SRC_DIR)/Makefile.inc before proceeding)
endif

ifndef HUNSPELL_LIB_VER
$(error Define HUNSPELL_LIB_VER in $(SRC_DIR)/Makefile.inc before proceeding)
endif
ifndef HUNSPELL_LIB_DIR
$(error Define HUNSPELL_LIB_DIR in $(SRC_DIR)/Makefile.inc before proceeding)
endif
ifndef HUNSPELL_LIB_OPT
$(error Define HUNSPELL_LIB_OPT in $(SRC_DIR)/Makefile.inc before proceeding)
endif
