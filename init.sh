#!/bin/env tcsh
echo "Initializing system variables"
if (! $?LD_LIBRARY_PATH) then       
  setenv LD_LIBRARY_PATH "/usr/local/lib/opencv-3.1.0/lib/"
else
  setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:/usr/local/lib/opencv-3.1.0/lib/"
endif

echo "Done!"