import io
import os
import xmltodict, json

#"C:\Users\ddelam\Desktop\Documents\Balance\Adept.xml"
def GetFiles(directory):
    files = os.listdir(directory)
    return files
    
def ConvertFileXMLtoJSON(file):
    with open(file) as handle:
        xmltext = handle.read()
        parsedxml = xmltodict.parse(xmltext)
        return json.loads(json.dumps(parsedxml))

def GetDictFields (unit):
	# Unwrap the dict we want
	subUnit = unit["unit"]
	#Just the fields we want
	cleanUnit = {}
	cleanUnit['maxHealth'] = subUnit["life"]["@max"]
	cleanUnit['maxShields'] = subUnit["shields"]["@max"] if "shields" in subUnit else "0"
	cleanUnit['movementType'] = subUnit["movement"]["@type"] if "movement" in subUnit else "None"
	return cleanUnit

directory = "Balance\\"
xmlfiles = GetFiles(directory)
stableIds = json.load(open("stableid.json"))
jsonfile = open("unitInfo.json", 'w')
jsonfile.truncate()
units = {}
#Load and convert to json
for file in xmlfiles:
    #json.dump(ConvertFileXMLtoJSON(directory+file),jsonfile)
	unit = ConvertFileXMLtoJSON(directory+file)
	id = unit["unit"]["@id"]
	units[id] = GetDictFields(unit)

for unit in stableIds['Units']:
	key = unit["name"]
    #load corresponding unit data from parsed files
	if key in units:
		unitData = units[key]
		unit["maxHealth"] = int(unitData["maxHealth"])
		unit["maxShields"] = int(unitData["maxShields"])
		unit["movementType"] = 2 if unitData["movementType"] == "Fly" else 1

json.dump(stableIds['Units'], jsonfile)