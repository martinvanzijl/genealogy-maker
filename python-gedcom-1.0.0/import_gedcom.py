#!/usr/bin/python3

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
    inputFileName = 'tests/files/Musterstammbaum.ged'

    numberOfArgs = len(sys.argv)
    if numberOfArgs >= 2:
        inputFileName = sys.argv[1]

    # Parse the GEDCOM file.
    parser = Parser()
    parser.parse_file(inputFileName)

    # Add the persons into the list.
    for element in parser.get_element_list():
        if isinstance(element, IndividualElement):

            # Store the ID.
            pointer = element.get_pointer()

            # Store the name.
            allNames = element.get_name()
            fullName = " ".join(allNames)
            
            # Store the date of birth.
            birthData = element.get_birth_data()
            dateOfBirth = birthData[0]
            placeOfBirth = birthData[1]

            # Add to the list.
            persons.append(Person(pointer, fullName, dateOfBirth, placeOfBirth))

            # List parents.
            parents = parser.get_parents(element)
            #print("Parents of", fullName, "are", parents)
            #for parent in parents:
                #print("--", parent.get_pointer())
    
    # Create the XML document.
    doc = xml.Document()
    root = doc.createElement("genealogy")
    doc.appendChild(root)
    
    # Add the persons from the list.
    for person in persons:
        element = doc.createElement("item")
        #element.setAttribute("pointer", person.pointer)
        element.setAttribute("name", person.name)
        element.setAttribute("date_of_birth", person.dateOfBirth)
        element.setAttribute("place_of_birth", person.placeOfBirth)
        root.appendChild(element)

    # Get output file name.
    outputFileName = 'output.xml'

    numberOfArgs = len(sys.argv)
    if numberOfArgs >= 3:
        outputFileName = sys.argv[2]

    # Write the XML file.
    outputFile = open(outputFileName, "w")
    outputFile.write(doc.toprettyxml())
    outputFile.close()

if __name__ == "__main__":
    test_parse_file()
