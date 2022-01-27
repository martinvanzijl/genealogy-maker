#!/usr/bin/python3

import gedcom
from gedcom.element.element import Element
from gedcom.element.family import FamilyElement
from gedcom.element.individual import IndividualElement
from gedcom.parser import Parser
import sys
import xml.dom.minidom as xml

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
        self.familiesAsSpouse = []
        self.familiesAsChild = []

    def addFamilyAsSpouse(self, family):

        if len(self.familiesAsSpouse) > 0:
            print("Person already has family as spouse:", self)

        self.familiesAsSpouse.append(family)

    def addFamilyAsChild(self, family):

        if family in self.familiesAsChild:
            print("Family already added as child.")
            return

        self.familiesAsChild.append(family)

    def __str__(self):
        return self.pointer + " (" + self.name + ")"

class Family():

    def __init__(self, pointer):
        self.pointer = pointer
        self.husband = None
        self.wife = None
        self.children = []
        self.marriageDate = ""
        self.marriagePlace = ""

    def addChild(self, child):

        if type(child) != str:
            print("Child must be string pointer.")
            return

        if child in self.children:
            #print("Child already in list.")
            return

        self.children.append(child)

    def setHusband(self, husband):

        if type(husband) != str:
            print("Husband must be string pointer.")
            return

        # Debug.
        if self.husband is not None:
            print("\t\tReplacing husband", self.husband, "with", husband, "in", self.pointer)

        self.husband = husband

    def setMarriageDate(self, date):
        self.marriageDate = date

    def setMarriagePlace(self, place):
        self.marriagePlace = place

    def setWife(self, wife):

        if type(wife) != str:
            print("Wife must be string pointer.")
            return

        # Debug.
        if self.wife is not None:
            print("\t\tReplacing wife", self.wife, "with", wife, "in", self.pointer)

        self.wife = wife

    def __str__(self):
        return "Husband: " + str(self.husband) + " Wife: " + str(self.wife) + " Children: " + str(self.children)

class Genealogy():

    def __init__(self):
        self.familyNumber = 1
        self.families = []

    def getFamilyAsSpouse(self, person):
        """Return (create if does not exist) the family where the given
        person is a spouse."""

        # This does not work well if someone was widowed and re-married!

        # Create if does not exist.
        if len(person.familiesAsSpouse) == 0:

            # Debug.
            #print("Creating new family.")

            # Create ID.
            pointer = "@" + str(self.familyNumber) + "@"

            # Create family.
            family = Family(pointer)

            if person.gender == "M":
                family.setHusband(person.pointer)
            elif person.gender == "F":
                family.setWife(person.pointer)
            else:
                print ("Unknown gender for parent. Setting to husband.")
                family.setHusband(person.pointer)

            self.families.append(family)

            # Associate with person.
            person.addFamilyAsSpouse(family)

            # Increment the ID.
            self.familyNumber += 1

        # Return.
        return person.familiesAsSpouse[0]

    def marry(self, husband, wife):
        """Marry the husband to the wife. Ensure that there is a family
        that includes them as spouses. Return the family."""

        # Debug.
        print("\tMarrying:", husband, "and", wife)

        if len(husband.familiesAsSpouse) == 0:
            if len(wife.familiesAsSpouse) == 0:
                family = self.getFamilyAsSpouse(husband)
                wife.addFamilyAsSpouse(family)
            else:
                family = wife.familiesAsSpouse[0]
                husband.addFamilyAsSpouse(family)
        else:
            family = husband.familiesAsSpouse[0]
            wife.addFamilyAsSpouse(family)

        family.setHusband(husband.pointer)
        family.setWife(wife.pointer)

        return family

def test_parse_file():
    # Create lists.
    persons = []
    personsDict = {} # By pointer.
    personsDictById = {} # By XML id.

    # Create overall object.
    genealogy = Genealogy()

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
            #print ("Using pointer from GEDCOM.") # Debug.
            pointer = element.getAttribute("pointer")
        else:
            #print ("Creating new pointer.") # Debug.
            pointer = "@" + str(number) + "@"

        personId = element.getAttribute("id")
        name = element.getAttribute("name")
        firstName = element.getAttribute("first_name")
        lastName = element.getAttribute("last_name")
        dateOfBirth = element.getAttribute("date_of_birth")
        placeOfBirth = element.getAttribute("place_of_birth")
        dateOfDeath = element.getAttribute("date_of_death")
        placeOfDeath = element.getAttribute("place_of_death")
        gender = element.getAttribute("gender")
        person = Person(pointer, name, firstName, lastName, dateOfBirth, placeOfBirth, gender, dateOfDeath, placeOfDeath)
        persons.append(person)

        personsDict[pointer] = person
        personsDictById[personId] = person

        number += 1

    # Read the marriages.
    marriageElements = doc.getElementsByTagName("marriage")

    # Add the marriages from the list.
    number = 1
    for element in marriageElements:

        # Read from XML.
        if element.hasAttribute("left_pointer") and element.hasAttribute("right_pointer"):

            # GEDCOM pointers.
            leftPointer = element.getAttribute("left_pointer")
            rightPointer = element.getAttribute("right_pointer")

            husband = personsDict[leftPointer]
            wife = personsDict[rightPointer]

        elif element.hasAttribute("person_left") and element.hasAttribute("person_right"):

            # XML Ids.
            leftId = element.getAttribute("person_left")
            rightId = element.getAttribute("person_right")

            husband = personsDictById[leftId]
            wife = personsDictById[rightId]

        # Debug.
        print("Processing marriage between", husband, "and", wife)

        # Create family if does not exist.
        family = genealogy.marry(husband, wife)

        # Set marriage details.
        family.setMarriageDate(element.getAttribute("date"))
        family.setMarriagePlace(element.getAttribute("place"))

    # Read the relationships.
    relationshipElements = doc.getElementsByTagName("relationship")

    # Add the relationships from the list.
    for element in relationshipElements:

        # Read from XML.
        if element.hasAttribute("from_pointer") and element.hasAttribute("to_pointer"):

            # GEDCOM pointers.
            fromPointer = element.getAttribute("from_pointer")
            toPointer = element.getAttribute("to_pointer")

        elif element.hasAttribute("from") and element.hasAttribute("to"):

            # XML Ids.
            fromId = element.getAttribute("from")
            toId = element.getAttribute("to")

            # Get pointers.
            fromPointer = personsDictById[fromId].pointer
            toPointer = personsDictById[toId].pointer

        # Get family.
        family = genealogy.getFamilyAsSpouse(personsDict[fromPointer])

        # Add child.
        family.addChild(toPointer)

        # Associate with child.
        personsDict[toPointer].addFamilyAsChild(family)

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

        # Add death details.
        if person.dateOfDeath or person.placeOfDeath:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_DEATH, value=""))
            if person.dateOfDeath:
                root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_DATE, value=person.dateOfDeath))
            if person.placeOfDeath:
                root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_PLACE, value=person.placeOfDeath))

        # Add families.
        for family in person.familiesAsSpouse:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_FAMILY_SPOUSE, value=family.pointer))

        for family in person.familiesAsChild:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_FAMILY_CHILD, value=family.pointer))

    # Add the families to the GEDCOM file.
    for family in genealogy.families:
        #print("Adding family to GEDCOM:", family)

        # Create family element.
        element = FamilyElement(level=0, pointer=family.pointer, tag=gedcom.tags.GEDCOM_TAG_FAMILY, value="")

        # Add to parser.
        root.add_child_element(element)

        # Add husband.
        if family.husband:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_HUSBAND, value=family.husband))

        # Add wife.
        if family.wife:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_WIFE, value=family.wife))

        # Add marriage details.
        if len(family.marriageDate) > 0 or len(family.marriagePlace) > 0:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_MARRIAGE, value=""))

            if (len(family.marriageDate) > 0):
                root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_DATE, value=family.marriageDate))

            if (len(family.marriagePlace) > 0):
                root.add_child_element(Element(level=2, pointer="", tag=gedcom.tags.GEDCOM_TAG_PLACE, value=family.marriagePlace))

        # Add children.
        for child in family.children:
            root.add_child_element(Element(level=1, pointer="", tag=gedcom.tags.GEDCOM_TAG_CHILD, value=child))

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
