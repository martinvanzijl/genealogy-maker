#!/usr/bin/python3

import gedcom
from gedcom.element.element import Element
from gedcom.element.individual import IndividualElement
from gedcom.parser import Parser
import sys
import xml.dom.minidom as xml

class Person():
    
    def __init__(self, pointer, name, firstName, lastName, dateOfBirth, placeOfBirth, gender):
        self.pointer = pointer
        self.name = name
        self.firstName = firstName
        self.lastName = lastName
        self.dateOfBirth = dateOfBirth
        self.placeOfBirth = placeOfBirth
        self.gender = gender

def test_parse_file():
    # Create list.
    persons = []

    # Get input file.
    #inputFileName = 'test-export/diagram.xml'
    inputFileName = 'output.xml'

    numberOfArgs = len(sys.argv)
    if numberOfArgs >= 2:
        inputFileName = sys.argv[1]

    # Read the input file.
    doc = xml.parse(inputFileName)
    
    # Read the persons.
    personElements = doc.getElementsByTagName("item")

    # Add the persons from the list.
    number = 1
    for element in personElements:

        if element.hasAttribute("pointer"):
            pointer = element.getAttribute("pointer")
        else:
            pointer = "@" + str(number) + "@"

        name = element.getAttribute("name")
        firstName = element.getAttribute("first_name")
        lastName = element.getAttribute("last_name")
        dateOfBirth = element.getAttribute("date_of_birth")
        placeOfBirth = element.getAttribute("place_of_birth")
        gender = element.getAttribute("gender")
        persons.append(Person(pointer, name, firstName, lastName, dateOfBirth, placeOfBirth, gender))
        number += 1

    # Write the GEDCOM file.
    parser = Parser()
    root = parser.get_root_element()

    # Add head element.
    root.add_child_element(Element(level=0, pointer="", tag="HEAD", value=""))

    # Add the persons to the GEDCOM file.
    for person in persons:
        # Create person.
        element = IndividualElement(level=0, pointer=person.pointer, tag=gedcom.tags.GEDCOM_TAG_INDIVIDUAL, value="")

        # Add to parser.
        root.add_child_element(element)

        # Add name.
        root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_NAME, value=person.name))

        # Add last name.
        if len(person.lastName) > 0:
            root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_SURNAME, value=person.lastName))

        # Add first name.
        if len(person.firstName) > 0:
            root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_GIVEN_NAME, value=person.firstName))

        # Add gender.
        if len(person.gender) > 0:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_SEX, value=person.gender))

        # Add birth details.
        root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_BIRTH, value=""))
        root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_DATE, value=person.dateOfBirth))
        if person.placeOfBirth:
            root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_PLACE, value=person.placeOfBirth))

    # Add footer element.
    root.add_child_element(Element(level=0, pointer="", tag="TRLR", value=""))

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
