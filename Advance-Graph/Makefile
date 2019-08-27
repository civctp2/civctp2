

all : Advance.gv
all : Advance.svg
all : Advance.pdf

%.json : ../ctp2_data/default/gamedata/%.txt
	../tools/txt2json.sh $< > $@

%.gv : %.json
	../tools/json2dot.sh < $< > $@

%.svg : %.gv
	dot $< -Tsvg -o $@

%.pdf : %.gv
	dot $< -Tpdf -o $@
