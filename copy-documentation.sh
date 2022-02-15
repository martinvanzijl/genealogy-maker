#!/bin/bash

# ======================================================================
# Copy the GEDCOM scripts to the "build" directories.
# ======================================================================

# Declare directories.
DIRS="../build-genealogymaker-* ../build-unittest-*"

# Loop through dirs.
for DIR in $DIRS;
do
    # Copy documentation to dir if it exists.
    if [[ -d $DIR ]]
    then
        echo "Copying documentation to $DIR..."
        cp -R doc $DIR
    fi
done
