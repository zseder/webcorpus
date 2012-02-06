HTML_SCRIPTS = htmlattrib htmldetag htmlformat

all: $(HTML_SCRIPTS) 
	@echo Everything is ready

htmlattrib: splitcode.h htmlattrib.flex
	@echo Compiling htmlattrib...
	flex -i htmlattrib.flex # -i -> ignore case, -f -8 fast
	gcc -lfl lex.yy.c -o htmlattrib
	@echo htmlattrib done

htmldetag: splitcode.h htmldetag.flex
	@echo Compiling htmldetag...
	flex -i htmldetag.flex # -i -> ignore case, -f -8 fast
	gcc -lfl lex.yy.c -o htmldetag
	@echo htmldetag done

htmlformat: splitcode.h htmlformat.flex
	@echo Compiling htmldetag...
	flex -i htmlformat.flex # -i -> ignore case, -f -8 fast
	gcc -lfl lex.yy.c -o htmlformat
	@echo htmldetag done

splitcode.h: makesplitcode.sh 
	@echo Creating split code...
	./$< > $@

