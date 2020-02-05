

all : Advance.gv
all : Advance.svg
all : Advance.pdf

%.json : ../ctp2_data/default/gamedata/%.txt
	../tools/txt2json.sh $< > $@

%.gv : %.json
	../tools/json2dot.sh ../ctp2_data/default/gamedata/uniticon.txt < $< \
	| gvpr -c 'N[$$.degree==0]{delete(root, $$)}' > $@ # remove nodes without any edges, e.g. DEFAULT, SUBNEURAL_ADS, MIND_CONTROL

%.svg : %.gv
	dot $< -Tsvg \
	| sed 's|xlink:href="|xlink:href="pic555/|' > $@

%.pdf : %.gv
	dot $< -Tpdf \
	| sed "s|^[[:space:]]*/CreationDate (.*)$$|/CreationDate (D:20190828000000+00'00')|; \
	     s|^[[:space:]]*/ModDate (.*)$$|/ModDate (D:20190828000000+00'00')|; \
	     /^\/ID \[<.*> <.*>\]$$/d" >  $@ # deleting ID instead of replacing: http://superuser.com/questions/130347/how-do-i-produce-bytewise-consistent-documents-with-pdflatex#130804

