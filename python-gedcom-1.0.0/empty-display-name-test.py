#!/usr/bin/python3

from gedcom.element.individual import IndividualElement
from gedcom.element.root import RootElement
from gedcom.parser import Parser

def test_parse_file():

    # Parse the GEDCOM file.
    parser = Parser()
    parser.parse_file('gedcom-from-export.ged')

    individuals_in_root_child_elements = 0
    individuals_in_element_list = 0

    for element in parser.get_element_list():
        if isinstance(element, IndividualElement):
            individuals_in_element_list += 1
            allNames = element.get_name()

            # TODO: Is this really the right logic?
            # Also, what does this do to the round trip test?
            print(allNames)
            assert allNames[0] == "New Person"
            assert allNames[1] == ""

if __name__ == "__main__":
    test_parse_file()
