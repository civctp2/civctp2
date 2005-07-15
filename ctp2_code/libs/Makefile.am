all: $(ctp2_code)/libs/anet/linux86.tgz

clean:
	$(MAKE) -C $(ctp2_code)/libs/anet $@

distclean install install-data purge:

$(ctp2_code)/libs/anet/linux86.tgz: $(top_builddir)/ctp2_code/libs/Makefile \
		$(ctp2_code)/libs/anet/Makefile
	$(MAKE) -C $(ctp2_code)/libs/anet all
