HTLATEX=htlatex
LATEX2HTML=latex2html
PDFLATEX=pdflatex
PERL=perl
THUMBPDF=thumbpdf
VIEWPDF=acroread

CTP2_DEV_SOURCES = ctp2_dev.tex \
	include/eula.tex \
	include/make_win32.tex include/setup_win32.tex \
	../common/images/wallpaper1_640x480_2.jpg \
	include/a_defines.tex include/a_dspconfig.tex \
	include/a_deadcode.tex include/a_file_describe.tex \
	include/a_faq_project.tex include/a_faq_licence.tex

all: doc

doc: ctp2_dev.pdf 
#	ctp2_dev/index.html \
#	ctp2_dev_ht.html

ctp2_dev.pdf: $(CTP2_DEV_SOURCES)
	$(PDFLATEX) $<
	$(PDFLATEX) $<
	$(THUMBPDF) $@
	$(PDFLATEX) $<

ctp2_dev/index.html: $(CTP2_DEV_SOURCES)
	$(LATEX2HTML) $<

ctp2_dev_ht.tex: ctp2_dev.tex
	$(PERL) -e \
		'while (<>) { \
			if ($$_ =~ /\\maketitle/) { \
				print "% " . $$_; \
			} else { \
				print $$_; \
			} \
		}' < $< > $@

ctp2_dev_ht.html: ctp2_dev_ht.tex $(CTP2_DEV_SOURCES)
	$(HTLATEX) $<

clean:
	rm -f ctp2_dev.pdf
	rm -rf ctp2_dev
	rm -f ctp2_dev{,_ht}.{4ct,4tc,css,html,idv,lg,tmp,xref} ctp2_dev{,_ht}0x.gif
	rm -f ctp2_dev_ht.tex
	rm -f *.aux *.dvi *.log *.out *.toc *.tpt
	rm -f include/*.aux include/*.log include/*.out include/*.toc include/*.tpt

view: ctp2_dev.pdf
	$(VIEWPDF) $<

.DELETE_ON_ERROR:
