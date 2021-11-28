#!/usr/bin/python3

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

class Relationship():
    
    def __init__(self, parent, child):
        self.parent = parent
        self.child = child

def test_parse_file():
    # Create list.
    persons = []
    relationships = []

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
            
            # Store the first and last name.
            firstName = allNames[0]
            lastName = allNames[1]

            # Store the date of birth.
            birthData = element.get_birth_data()
            dateOfBirth = birthData[0]
            placeOfBirth = birthData[1]

            # Store the gender.
            gender = element.get_gender()

            # Add to the list.
            persons.append(Person(pointer, fullName, firstName, lastName, dateOfBirth, placeOfBirth, gender))

            # List parents.
            parents = parser.get_parents(element)
            #print("Parents of", fullName, "are", parents)
            for parent in parents:
                #print("--", parent.get_pointer())
                parentPointer = parent.get_pointer()
                relationships.append(Relationship(parentPointer, pointer))
    
    # Create the XML document.
    doc = xml.Document()
    root = doc.createElement("genealogy")
    doc.appendChild(root)
    
    # Add the persons from the list.
    for person in persons:
        element = doc.createElement("item")
        element.setAttribute("pointer", person.pointer)
        element.setAttribute("name", person.name)
        element.setAttribute("first_name", person.firstName)
        element.setAttribute("last_name", person.lastName)
        element.setAttribute("date_of_birth", person.dateOfBirth)
        element.setAttribute("place_of_birth", person.placeOfBirth)
        element.setAttribute("gender", person.gender)
        root.appendChild(element)

    # Add the relationships from the list.
    for relationship in relationships:
        element = doc.createElement("relationship")
        element.setAttribute("from_pointer", relationship.parent)
        element.setAttribute("to_pointer", relationship.child)
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
