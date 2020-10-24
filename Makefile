<<<<<<< HEAD
# $Id: Makefile,v 1.39 2020-10-22 15:53:56-07 - - $

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = check lint ci clean spotless 
=======
# $Id: Makefile,v 1.34 2019-10-16 15:17:26-07 - - $

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
>>>>>>> 59385574f89ffdd8f27a50b960ba54642a84e3d9
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory
GPPWARN     = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS     = ${GPPWARN} -fdiagnostics-color=never
COMPILECPP  = g++ -std=gnu++17 -g -O0 ${GPPOPTS}
MAKEDEPCPP  = g++ -std=gnu++17 -MM ${GPPOPTS}
UTILBIN     = /afs/cats.ucsc.edu/courses/cse111-wm/bin

MODULES     = commands debug file_sys util
CPPHEADER   = ${MODULES:=.h}
CPPSOURCE   = ${MODULES:=.cpp} main.cpp
EXECBIN     = yshell
OBJECTS     = ${CPPSOURCE:.cpp=.o}
MODULESRC   = ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.cpp}
OTHERSRC    = ${filter-out ${MODULESRC}, ${CPPHEADER} ${CPPSOURCE}}
ALLSOURCES  = ${MODULESRC} ${OTHERSRC} ${MKFILE}
LISTING     = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
<<<<<<< HEAD
	${COMPILECPP} -c $<

check : ${ALLSOURCES}
	- ${UTILBIN}/checksource ${ALLSOURCES}

lint : ${CPPSOURCE}
	- ${UTILBIN}/cpplint.py.perl ${CPPSOURCE}

ci : ${ALLSOURCES}
	- ${UTILBIN}/cid -is ${ALLSOURCES}
=======
	- ${UTILBIN}/cpplint.py.perl $<
	- ${UTILBIN}/checksource $<
	${COMPILECPP} -c $<

ci : ${ALLSOURCES}
	- ${UTILBIN}/checksource ${ALLSOURCES}
	${UTILBIN}/cid -is ${ALLSOURCES}
>>>>>>> 59385574f89ffdd8f27a50b960ba54642a84e3d9

lis : ${ALLSOURCES}
	${UTILBIN}/mkpspdf ${LISTING} ${ALLSOURCES} ${DEPFILE}

clean :
	- rm ${OBJECTS} ${DEPFILE} core ${EXECBIN}.errs

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}


dep : ${CPPSOURCE} ${CPPHEADER}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} : ${MKFILE}
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all lis

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

