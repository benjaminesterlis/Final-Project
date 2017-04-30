#!/bin/bash

gcc -g ./unit_tests/KDTree_unit_test.c KDTree.c KDArray.c SPPoint.c KDTree.h KDArray.h SPPoint.h ./unit_tests/unit_test_util.h -o KDTreeTester -lm

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
