import os
import re
import xml.etree.ElementTree as ET
from datetime import datetime
import zipfile


def extrair_informacoes(arquivo_xml, tags):
    tree = ET.parse(arquivo_xml)
    root = tree.getroot()

    # Define the namespace
    ns = {'ns': 'http://graphml.graphdrawing.org/xmlns'}

    data = {}

    for tag in tags:
        # Include the namespace in the tag
        elementos = root.findall('.//ns:data[@key="{}"]'.format(tag), ns)
        for elemento in elementos:
            texto = elemento.text
            if texto:
                if tag == 'assumption.scope':
                    # Directly set entryfunction with the assumption.scope value
                    data['entryfunction'] = texto
                else:
                    data[tag] = texto

    return data


def create_metadata_file(data, filename):
    metadata = ET.Element('test-metadata')

    for key, value in data.items():
        if key != 'assumption':
            ET.SubElement(metadata, key).text = value

    # Add creationtime tag
    creationtime = datetime.now().isoformat()
    ET.SubElement(metadata, 'creationtime').text = creationtime

    tree = ET.ElementTree(metadata)
    tree.write(filename, encoding='utf-8', xml_declaration=True)


def create_testcase_file(assumptions, filename):
    testcase = ET.Element('testcase')
    testcase.set('coversError', 'true')

    # Add DOCTYPE declaration
    doctype = '<!DOCTYPE testcase PUBLIC "+//IDN sosy-lab.org//DTD test-format testcase 1.1//EN" "https://sosy-lab.org/test-format/testcase-1.1.dtd">\n'

    # Add XML declaration
    xml_declaration = '<?xml version="1.0" encoding="utf-8"?>\n'

    # Use a set to track unique assumption values
    unique_assumptions = set()

    for assumption in assumptions:
        value = assumption.strip()
        number_match = re.search(r'== (\d+)', value)
        if number_match:
            number = number_match.group(1)
            # Check if the assumption value has already been processed
            if number not in unique_assumptions:
                ET.SubElement(testcase, 'input').text = number
                unique_assumptions.add(number)  # Add the assumption value to the set

    # Write to the file
    with open(filename, 'w') as file:
        file.write(xml_declaration)
        file.write(doctype)
        file.write(ET.tostring(testcase, encoding='unicode'))


def create_zip_file(metadata_filename, testcase_filename):
    with zipfile.ZipFile('test-suite.zip', 'w') as zipf:
        zipf.write(metadata_filename, arcname='metadata.xml')
        zipf.write(testcase_filename, arcname='testcase.xml')


tags = [
    'sourcecodelang',
    'producer',
    'specification',
    'programfile',
    'programhash',
    'architecture',
    'assumption',
]

arquivo_xml = 'witness.graphml'
assumptions = []

# Extract assumptions separately
for edge in ET.parse(arquivo_xml).getroot().iter('{http://graphml.graphdrawing.org/xmlns}edge'):
    assumption_element = edge.find('{http://graphml.graphdrawing.org/xmlns}data[@key="assumption"]')
    if assumption_element is not None:  # Check if the element exists
        assumption_data = assumption_element.text
        if assumption_data:
            assumptions.append(assumption_data)

# Create the directory if it doesn't exist
if not os.path.exists('testsuite'):
    os.makedirs('testsuite')

data = extrair_informacoes(arquivo_xml, tags)

# Ensure entryfunction is set to "main" if not already present
if 'entryfunction' not in data:
    data['entryfunction'] = 'main'

create_metadata_file(data, os.path.join('testsuite', 'metadata.xml'))
create_testcase_file(assumptions, os.path.join('testsuite', 'testcase.xml'))
create_zip_file(os.path.join('testsuite', 'metadata.xml'), os.path.join('testsuite', 'testcase.xml'))