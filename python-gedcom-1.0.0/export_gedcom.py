#!/usr/bin/python3

import gedcom
from gedcom.element.element import Element
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
        pointer = element.getAttribute("pointer")
        name = element.getAttribute("name")
        dateOfBirth = element.getAttribute("date_of_birth")
        placeOfBirth = element.getAttribute("place_of_birth")
        persons.append(Person(pointer, name, dateOfBirth, placeOfBirth))

    # Write the GEDCOM file.
    parser = Parser()
    root = parser.get_root_element()

    # Add the persons to the GEDCOM file.
    for person in persons:
        # Create person.
        element = IndividualElement(level=0, pointer=person.pointer, tag=gedcom.tags.GEDCOM_TAG_INDIVIDUAL, value="")

        # Add to parser.
        root.add_child_element(element)

        # Add name.
        root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_NAME, value=person.name))

        # Add birth details.
        root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_BIRTH, value=""))
        root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_DATE, value=person.dateOfBirth))
        root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_PLACE, value=person.placeOfBirth))

    # Get output file name.
    outputFileName = 'output.ged'

    numberOfArgs = len(sys.argv)
    if numberOfArgs >= 3:
        outputFileName = sys.argv[2]

    # Write the XML file.
    #parser.print_gedcom()
    outputFile = open(outputFileName, "w")
    parser.save_gedcom(outputFile)
    outputFile.close()

if __name__ == "__main__":
    test_parse_file()
