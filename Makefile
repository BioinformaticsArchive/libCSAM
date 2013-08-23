CPP = g++
FLAGS = -O9 -Wall 
#FLAGS = -g -O0 -Wall -DNDEBUG -I 
OBJS = 	./src/basics/io.o \
	./src/coders/Coder.o \
	./src/coders/THuff.o \
	./src/coders/Huffman.o \
	./src/SAM.o \
	./src/Cqual/CQualLL.o\
	./src/Cqual/CQualPBlock.o\
	./src/Cqual/CQualRBlock.o\
	./src/Cqual/CQualBin.o\
	./src/Cqual/CQualNStore.o\
	./src/Cqual/CQual.o\



BIN = CompressQual DecompressQual 

%.o: %.cpp
	@echo " [C++] Compiling $<"
	@$(CPP) $(FLAGS) -c $< -o $@ 

all: clean $(OBJS) $(BIN)


CompressQual: 
	$(CPP) $(FLAGS) $(OBJS) -o CompressQual ./src/tests/CompressQual.cpp -lboost_iostreams

DecompressQual:
	$(CPP) $(FLAGS) $(OBJS) -o DecompressQual ./src/tests/DecompressQual.cpp -lboost_iostreams


doc:
	@echo " [DOC] Generating documentation"
	@doxygen


clean:
	@echo " [CLN] Removing object files"
	@rm -f $(OBJS) $(BIN) *~

