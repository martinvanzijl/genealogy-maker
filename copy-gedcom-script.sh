#!/bin/bash

# ======================================================================
# Copy the GEDCOM scripts to the "build" directories.
# ======================================================================

# Declare directories.
DIRS="../build-genealogymaker-* ../build-unittest-*"

# Loop through dirs.
for DIR in $DIRS;
do
    # Copy script to dir if it exists.
    if [[ -d $DIR ]]
    then
        echo "Copying script to $DIR..."
        cp -R python-gedcom-1.0.0 $DIR
    fi
done
