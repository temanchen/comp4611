CC = g++
TARGET = main
OBJS = GlobalBranchPredictor.o LocalBranchPredictor.o main.o MIPSComputer.o
${TARGET}: ${OBJS}
	${CC} -o ${TARGET} $^
GlobalBranchPredictor.o: GlobalBranchPredictor.cpp GlobalBranchPredictor.h
	${CC} -c GlobalBranchPredictor.cpp
LocalBranchPredictor.o: LocalBranchPredictor.cpp LocalBranchPredictor.h
	${CC} -c LocalBranchPredictor.cpp
main.o: main.cpp MIPSComputer.h GlobalBranchPredictor.h LocalBranchPredictor.h
	${CC} -c main.cpp
MIPSComputer.o: MIPSComputer.cpp MIPSComputer.h GlobalBranchPredictor.h LocalBranchPredictor.h
	${CC} -c MIPSComputer.cpp
clean:
	rm -f ${TARGET} ${OBJS}
