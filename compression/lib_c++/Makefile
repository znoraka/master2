CC=g++
EXEC=ImageBase
TMPFOLDER=tmp

all: $(EXEC)



$(EXEC): main.o ImageBase.o
	if [ ! -d $(TMPFOLDER) ]; then mkdir $(TMPFOLDER); fi
	$(CC) $(TMPFOLDER)/main.o $(TMPFOLDER)/ImageBase.o -o $(EXEC)
	

main.o: main.cpp ImageBase.h
	if [ ! -d $(TMPFOLDER) ]; then mkdir $(TMPFOLDER); fi
	$(CC) -o $(TMPFOLDER)/main.o -c main.cpp
	

ImageBase.o: ImageBase.cpp
	if [ ! -d $(TMPFOLDER) ]; then mkdir $(TMPFOLDER); fi
	$(CC) -o $(TMPFOLDER)/ImageBase.o -c ImageBase.cpp
	
clean: 
	rm -rf *.o $(EXEC)
	rm -rf $(TMPFOLDER)/*.o $(EXEC)

