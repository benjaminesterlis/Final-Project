#!/bin/bash

gcc -g ./unit_tests/KDTree_unit_test.c KNN_Search.c KDTree.c KDArray.c SPPoint.c SPBPriorityQueue.c -o KDTreeTester -lm -Werror -pedantic-errors -DNDEBUG -std=c99

zenity --question --text="Do you want Valgrind?" --title="Valgrind Question..."
if [[ $? -eq 0 ]]; then
	zenity --question --text=" --leak-check=full" --title="MemLeak Question..."
	if [[ $? -eq 0 ]]; then
		valgrind --leak-check=full ./KDTreeTester
	else
	valgrind ./KDTreeTester

	fi
else
./KDTreeTester
fi
