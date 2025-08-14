.DEFAULT_GOAL := all

TOPTARGETS := all clean

IGNORELIST := fbcat/. Makefile/. $(wildcard *.bat/.) $(wildcard *.sh/.) $(wildcard *.md/.) $(wildcard *.txt/.)

SUBDIRS := $(filter-out $(IGNORELIST), $(wildcard */.))

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -s -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
