##
## PROBE Makefile
## $Id: Makefile,v 1.20 2006/12/01 22:56:10 mina86 Exp $
##


CXX      = g++
CC       = gcc


all: rpn set_expr rel-demo


## RPN BigInt Calculator
bigint.o: bigint.cpp bigint.hpp array.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ bigint.cpp

rpn.o: rpn.cpp bigint.hpp array.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ rpn.cpp

rpn: rpn.o bigint.o
	$(CXX) $(LDFLAGS) -o $@ $^


## BigInt Test module
make-in: make-in.c
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ make-in.c

in.bc: make-in
	./make-in 10000 >in.bc

in.rpn: in.bc
	while read A O B; do echo "$$A $$B $$O p del"; done <in.bc >in.rpn

out.bc: in.bc
	bc <in.bc >out.bc

out.rpn: in.rpn rpn
	./rpn <in.rpn >out.rpn

test-rpn: out.bc out.rpn
	@echo
	@if cmp -s out.bc out.rpn; \
	then echo 'RPN BigInt Calculator passed the test'; \
	else echo 'RPN BigInt Calculator DID NOT pass the test'; exit 1; \
	fi
	@echo


## Set Template
is_expr.o:  set_expr.cpp set.hpp array.hpp policy-compare.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ set_expr.cpp

bis_expr.o:  set_expr.cpp set.hpp array.hpp bigint.hpp policy-compare.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DBIGINT_SET -c -o $@ set_expr.cpp

is_expr: is_expr.o
	$(CXX) $(LDFLAGS) -o $@ $^

bis_expr: bis_expr.o bigint.o
	$(CXX) $(LDFLAGS) -o $@ $^

set_expr: is_expr bis_expr
	@:

gen-set: gen-set.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

test-set: set_expr gen-set
	@echo
	@echo Testing is_expt and bis_expr
	@sh test-set.sh
	@echo


## Relation
rel-demo: rel-demo.cpp relation.hpp set.hpp pair.hpp array.hpp \
          policy-compare.hpp
	$(CXX) $(LDFLAGS) -o $@ rel-demo.cpp


## Documentation
doc/src/head.html:
	mkdir -p doc/src
	doxygen -w html doc/src/head doc/src/foot.html doc/src/doxygen.css
	sed -e s/iso-8859-1/iso-8859-2/ <doc/src/head >doc/src/head.html
	rm -f -- doc/src/head

doc:: doc/src/head.html
	doxygen


## Common targets
clean:
	rm -f -- `grep -v doc <.cvsignore`

docclean:
	rm -rf -- doc/html

distclean: clean docclean


test: test-rpn test-set


help:
	@echo 'make [ CC=cc ] [ CXX=CC ] [ <target> ... ]'
	@echo 'Valid targets:'
	@echo '  file.o             -- build file.o (file ma be bigint, rpn, make-in, '
	@echo '                        is_expr or bis_expr)'
	@echo '  file               -- build executable file (can be rpn, make-in,'
	@echo '                        is_expr, bis_expr, gen-set or rel-demo)'
	@echo '  set_expr           -- build is_expr and bis_expr'
	@echo '  in.bc or in.rpn    -- generate input test files'
	@echo '  out.bc or out.rpn  -- generate output of test files'
	@echo '  test-rpn           -- test RPN BigInt Calculator'
	@echo '  test-set           -- test set_expr'
	@echo '  test               -- both test-rpn and test-set'
	@echo '  doc                -- generate documentation (doxygen required)'
	@echo '  clean              -- remove executable, object and temporary files'
	@echo '  docclean           -- remove generated documentation'
	@echo '  distclean          -- remove all generated and temporary files'
