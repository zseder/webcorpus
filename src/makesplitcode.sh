#!/bin/bash
# generate random splitcode for batch processing of HTML files
splitcode="$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM"
splitcodelen=$[ $(echo $splitcode | wc -c) - 1 ]
echo "#define SPLITCODE \"$splitcode\"" > $1
echo "#define SPLITCODELEN $splitcodelen" >>$1
echo "SPLITCODE = $splitcode" > $2
