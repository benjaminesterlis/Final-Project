CC = gcc
CPP = g++

OBJS = SPPoint.o SPBPriorityQueue.o SPLogger.o SPConfig.o KDArray.o KDTree.o SPImageProc.o main_aux.o main.o KNN_Search.o

#The executabel filename
EXEC = SPCBIR

INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG -g

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG -g

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@ -lm

main.o: main.cpp main_aux.h SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPBPriorityQueue.h KDTree.h KDArray.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp -lm

main_aux.o: main_aux.cpp main_aux.h SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPBPriorityQueue.h KDTree.h KDArray.h KNN_Search.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp

KNN_Search.o: KNN_Search.c KNN_Search.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c -lm

SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c -lm

SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c -lm

SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c -lm

KDArray.o: KDArray.c KDArray.h SPPoint.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDTree.o: KDTree.c KDTree.h KDArray.h SPPoint.h SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)

