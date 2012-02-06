#!/bin/bash
# generate random splitcode for batch processing of HTML files
splitcode="$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM"
splitcodelen=$[ $(echo $splitcode | wc -c) - 1 ]
echo "#define SPLITCODE \"$splitcode\""
echo "#define SPLITCODELEN $splitcodelen"


