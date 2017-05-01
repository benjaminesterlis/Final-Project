#!/bin/bash

gcc -g ./unit_tests/KDTree_unit_test.c KDTree.c KDArray.c SPPoint.c KDTree.h KDArray.h SPPoint.h ./unit_tests/unit_test_util.h -o KDTreeTester -lm -Werror -pedantic-errors -DNDEBUG -std=c99
gcc -g ./unit_tests/KDArray_unit_test.c KDArray.c SPPoint.c KDArray.h SPPoint.h ./unit_tests/unit_test_util.h -o KDArrayTester -lm -Werror -pedantic-errors -DNDEBUG -std=c99

zenity --question --text="Do you want Valgrind?" --title="Valgrind Question..."
if [[ $? -eq 0 ]]; then
	zenity --question --text=" --leak-check=full" --title="MemLeak Question..."
	if [[ $? -eq 0 ]]; then
		valgrind --leak-check=full ./KDTreeTester
		zenity --warning --text="Time for you to check what are the MemLeak" --title="Check Time"
		vagrind --leak-check=full ./KDArrayTester
	else
	valgrind ./KDTreeTester
	zenity --warning --text="Time for you to check what are the MemLeak" --title="Check Time"
	valgrind ./KDArrayTester
	fi
else
./KDTreeTester
./KDArrayTester
fi
