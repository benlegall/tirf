include Makefile.rules

all: PROJ

PROJ:
	@echo  "\033[33m\n***** COMPILATION *****\033[00m"
	@cd ./source && make
	@echo  "\033[33m***** COMPILATION END *****\n\033[00m"

clean:
	@echo  "\033[33m\n***** CLEANING...\033[00m"
	@find . -name "*.o" -exec rm {} \;
	@find . -name "*.a" -exec rm {} \;
	@find . -name "*.so" -exec rm {} \;
	@find . -name "*~" -exec rm {} \;
	@find . -name "debug.out" -exec rm {} \;
	@find . -name ${PROJ} -exec rm {} \;

distclean: clean
	@echo  "\033[33m\n***** DISTCLEANING...\033[00m\n"
	@find . -name "*~" -exec rm {} \;
	@find . -name "*\#*" -exec rm {} \;
	@find . -name "*core*" -exec rm {} \;
	@find . -name "*.log" -exec rm {} \;
	@$(RM) Makefile.rules

dist: distclean rights
	@echo  "\033[33m\n***** DIST *****\033[00m"
	tar -C .. --exclude IMC$(TAR_EXT)\
	 -cvjf IMC$(TAR_EXT) IMC 
	@echo  "\033[33m***** DIST END *****\033[00m"

distcheck:
	@echo  "\033[33m\n***** DISTCHECK *****\n\033[00m"
	@./configure && make dist
	@tar -xf IMC$(TAR_EXT)
	@cd ./IMC/  && ./configure && make check\
	 && make distclean
	@rm -rf IMC 
	@echo  "\033[33m***** DISTCHECK END *****\n\033[00m"

rights:
	@echo "\033[33m\n***** RIGHTS *****\033[00m"
	@find . -name "*" -exec chmod 750 {} \;
	@find . -name "*.[ch]" -exec chmod 640 {} \;
	@find . -name "Makefile" -exec chmod 640 {} \;
	@chmod 640 "AUTHORS" "README" "TODO"
	@find . -name "Makefile.rules" -exec chmod 640 {} \;
	@echo "\033[33m***** RIGHTS END *****\n\033[00m"

doc::
	@echo  "\033[33m\n***** DOC *****\n\033[00m"
	@cd ./doc && doxygen Doxyfile > /dev/null
	@echo  "\033[33m***** DOC END *****\n\033[00m"

check:: PROJ
	@./check/check.sh

changelog::
	@echo  "\033[33m\n***** CHANGELOG *****\033[00m"
	svn -v log > ChangeLog
	@echo  "\033[33m***** CHANGELOG END *****\n\033[00m"

.PHONY: check
