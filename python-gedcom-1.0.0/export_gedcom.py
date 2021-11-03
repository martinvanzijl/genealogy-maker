#!/usr/bin/python3

import gedcom
from gedcom.element.individual import IndividualElement
from gedcom.parser import Parser
import sys
import xml.dom.minidom as xml

class Person():
    
    def __init__(self, pointer, name, dateOfBirth, placeOfBirth):
        self.pointer = pointer
        self.name = name
        self.dateOfBirth = dateOfBirth
        self.placeOfBirth = placeOfBirth

def test_parse_file():
    # Create list.
    persons = []

    # Get input file.
    inputFileName = 'test-export/diagram.xml'

    numberOfArgs = len(sys.argv)
    if numberOfArgs >= 2:
        inputFileName = sys.argv[1]

    # Read the input file.
    doc = xml.parse(inputFileName)
    
    # Read the persons.
    personElements = doc.getElementsByTagName("item")

    # Add the persons from the list.
    for element in personElements:
        personId = element.getAttribute("id")
        name = element.getAttribute("name")
        dateOfBirth = element.getAttribute("date_of_birth")
        placeOfBirth = element.getAttribute("place_of_birth")
        persons.append(Person(personId, name, dateOfBirth, placeOfBirth))

    # Write the GEDCOM file.
    parser = Parser()

    # Add the persons to the GEDCOM file.
    for person in persons:
        element = IndividualElement(level=1, pointer=person.pointer, tag=gedcom.tags.GEDCOM_TAG_INDIVIDUAL, value="")
        parser.get_root_element().add_child_element(element)

    # Get output file name.
    outputFileName = 'output.ged'

    numberOfArgs = len(sys.argv)
    if numberOfArgs >= 3:
        outputFileName = sys.argv[2]

    # Write the XML file.
    parser.print_gedcom()
    #outputFile = open(outputFileName, "w")
    #parser.save_gedcom(outputFile)
    #outputFile.close()

if __name__ == "__main__":
    test_parse_file()
