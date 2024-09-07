import xml.etree.ElementTree as ET
import os.path
import sys

ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')

filename = 'Sandbox/Sandbox.vcxproj'
# check if file doesnt exist
if not os.path.isfile(filename):
    print(f'File \'{filename}\' does not exist.')
    exit()

# Load the XML file
tree = ET.parse(filename)

print(f'Parsed file \'{filename}\'')

# Get the root element
root = tree.getroot()

# Check if 'UpToDateCheckInput' exists in any tree element
for element in root.iter():
    tag = element.tag.split('}')[-1]
    if tag == 'UpToDateCheckInput':
        print("'UpToDateCheckInput' is already present, exiting")
        exit()

upToDateCheckInputElement = ET.Element('UpToDateCheckInput')
upToDateCheckInputElement.set('Include', 'Assets/**/*.*')

# Create a new element
itemGroupElement = ET.Element('ItemGroup')
itemGroupElement.append(upToDateCheckInputElement)

# Add the new element to the root
root.append(itemGroupElement)
print('Injected element \'UpToDateCheckInput\' with new \'ItemGroup\' into root')

# Indent the tree if we have python 3.9 or newer
if sys.version_info[0] >= 3 and sys.version_info[1] >= 9:
    ET.indent(root)

# Save the modified XML back to the file
tree.write(filename)
print(f'Wrote changes back to \'{filename}\'')
