#!/bin/bash

# Test exporting a Genealogy Maker file with an empty display name.

# Export GEDCOM.
echo "Exporting diagram (XML) to GEDCOM."
./export_gedcom.py ../test-files/empty-display-name-test.xml gedcom-from-export.ged

# Compare the two.
echo "Running Python test file."
./empty-display-name-test.py gedcom-from-export.ged
