PROJECT=FlatProject

TARGETS= global

PROPAGATE= \
for rule in $(TARGETS) ; do \
  make PROJECT=$(PROJECT) DESTDIR=$(DESTDIR) MODULE=$$rule -f Rules.$$rule $@ || exit $$?;\
done


all:
	@+$(PROPAGATE)

install:
	@+$(PROPAGATE)
	$(SU0) install -d $(DESTDIR)/usr/include/$(PROJECT)/
	$(SU0) install -m 644 *.H $(DESTDIR)/usr/include/$(PROJECT)/

clean:
	@+$(PROPAGATE)

