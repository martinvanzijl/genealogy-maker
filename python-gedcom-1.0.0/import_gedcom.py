#!/usr/bin/python3

import datetime
import gedcom
from gedcom.element.individual import IndividualElement
from gedcom.parser import Parser
import sys
import xml.dom.minidom as xml

def stringToDate(string):
    """Convert string to date."""

    # Must be in format "01 JUNE 2001".
    try:

        return datetime.datetime.strptime(string, "%d %b %Y")

    except ValueError as error:

        # ~ print("Could not read date for marriage:", error)
        return None

class Person():
    
    def __init__(self, pointer, name, firstName, lastName, dateOfBirth, placeOfBirth, gender, dateOfDeath, placeOfDeath):
        self.pointer = pointer
        self.name = name
        self.firstName = firstName
        self.lastName = lastName
        self.dateOfBirth = dateOfBirth
        self.placeOfBirth = placeOfBirth
        self.gender = gender
        self.dateOfDeath = dateOfDeath
        self.placeOfDeath = placeOfDeath

class Family():

    def __init__(self, pointer):
        self.pointer = pointer
        self.husband = None
        self.wife = None
        # ~ self.children = []
        self.marriageDate = ""
        self.marriagePlace = ""

    # ~ def addChild(self, child):
        # ~ self.children.append(child)

    def setHusband(self, husband):

        # Debug.
        if self.husband:
            print("Replacing husband", self.husband, "with", husband)

        self.husband = husband

    def setMarriageDate(self, date):
        self.marriageDate = date

    def setMarriagePlace(self, place):
        self.marriagePlace = place

    def setWife(self, wife):

        # Debug.
        if self.wife:
            print("Replacing wife", self.wife, "with", wife)

        self.wife = wife

    def __str__(self):
        # ~ return "Husband: " + str(self.husband) + " Wife: " + str(self.wife) + " Children: " + str(self.children)
        return "Husband: " + str(self.husband) + " Wife: " + str(self.wife) + " Marriage date: " + self.marriageDate

class Relationship():
    
    def __init__(self, parent, child):
        self.parent = parent
        self.child = child

def test_parse_file():
    # Create lists.
    persons = []
    relationships = []
    familiesDict = {}

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

            # Store the details of death.
            deathData = element.get_death_data()
            dateOfDeath = deathData[0]
            placeOfDeath = deathData[1]

            # Add to the list.
            persons.append(Person(pointer, fullName, firstName, lastName, dateOfBirth, placeOfBirth, gender, dateOfDeath, placeOfDeath))

            # List parents.
            parents = parser.get_parents(element)
            #print("Parents of", fullName, "are", parents)
            for parent in parents:
                #print("--", parent.get_pointer())
                parentPointer = parent.get_pointer()
                relationships.append(Relationship(parentPointer, pointer))

            # Check for marriage.

            # Returns marriage details.
            marriages = parser.get_marriages(element)
            # ~ print("Marriages for " + pointer + ":", marriages)

            familyElements = parser.get_families(element)
            #print("Families:", familyElements)

            for familyElement in familyElements:

                # Get ID.
                familyPointer = familyElement.get_pointer()

                # Add to dictionary.
                if familyPointer not in familiesDict:
                    family = Family(familyPointer)
                    familiesDict[familyPointer] = family

                    # Hack to set spouses and marriage details.
                    for element in familyElement.get_child_elements():
                        if element.get_tag() == gedcom.tags.GEDCOM_TAG_HUSBAND:
                            family.setHusband(element.get_value())
                        elif element.get_tag() == gedcom.tags.GEDCOM_TAG_WIFE:
                            family.setWife(element.get_value())

                        if element.get_tag() == gedcom.tags.GEDCOM_TAG_MARRIAGE:
                            for marriage_data in element.get_child_elements():
                                if marriage_data.get_tag() == gedcom.tags.GEDCOM_TAG_DATE:
                                    family.setMarriageDate(marriage_data.get_value())
                                if marriage_data.get_tag() == gedcom.tags.GEDCOM_TAG_PLACE:
                                    family.setMarriagePlace(marriage_data.get_value())

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
        if (person.dateOfDeath):
            element.setAttribute("date_of_death", person.dateOfDeath)
        if (person.placeOfDeath):
            element.setAttribute("place_of_death", person.placeOfDeath)
        root.appendChild(element)

    # Add the relationships from the list.
    for relationship in relationships:
        element = doc.createElement("relationship")
        element.setAttribute("from_pointer", relationship.parent)
        element.setAttribute("to_pointer", relationship.child)
        root.appendChild(element)

    # Order the marriages by date for each person.
    latestMarriages = {}

    for pointer in familiesDict:
        family = familiesDict[pointer]
        # ~ print ("Family:", family)

        if family.husband and family.wife:

            date = stringToDate(family.marriageDate)

            if not family.husband in latestMarriages:
                latestMarriages[family.husband] = family
            elif date is not None:
                latestDate = stringToDate(latestMarriages[family.husband].marriageDate)
                if latestDate is None or date > latestDate:
                    latestMarriages[family.husband] = family

            if not family.wife in latestMarriages:
                latestMarriages[family.wife] = family
            elif date is not None:
                latestDate = stringToDate(latestMarriages[family.wife].marriageDate)
                if latestDate is None or date > latestDate:
                    latestMarriages[family.wife] = family

    for item in latestMarriages:
        print("Latest marriage for", item, "=>", latestMarriages[item])

    # Add the marriages from the list.
    for pointer in familiesDict:
        family = familiesDict[pointer]

        if family.husband and family.wife:

            # Only store marriage if it is the latest for both the
            # husband and the wife.
            if not (latestMarriages[family.husband] == family and latestMarriages[family.wife] == family):

                # Skip this marriage.
                continue

            element = doc.createElement("marriage")
            element.setAttribute("left_pointer", family.husband)
            element.setAttribute("right_pointer", family.wife)

            if len(family.marriageDate) > 0:
                element.setAttribute("date", family.marriageDate)

            if len(family.marriagePlace) > 0:
                element.setAttribute("place", family.marriagePlace)

            root.appendChild(element)

            # Debug.
            #print("Importing marriage", pointer, "between", family.husband, "and", family.wife)

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
