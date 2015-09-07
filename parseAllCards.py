#!/usr/bin/python

import json
import unicodedata
import re
import os

downloadImages = False

f = open('deckbrew.json')
data = f.read()
f.close()

cards=json.loads(data)
types=set([
"artifact",
"conspiracy",
"creature",
"enchantment",
"instant",
"land",
"phenomenon",
"planeswalker",
"scheme",
"sorcery",
"tribal",
"vanguard"
])
subtypes=set()

headerFile = open('CardData.h','w')
headerFile.write('#include "MTGCard.h"\n\n')
headerFile.write('typedef struct {\n')
outFile = open('CardData.c','w')
outFile.write('#include "CardData.h"\n\n')
outFile.write('CardData loadCardData() {\n')
outFile.write('CardData cd;\n')
for page in cards:
	for node in page:
		valid = False
		multiverse_id = 0
		for edition in node['editions']:
			if edition['set'] == 'Magic 2015 Core Set':
				valid = True
				multiverse_id = edition['multiverse_id']
				break
		if not valid:
			continue
		name = node['name']
		name = name.replace(u'\xc6','Ae')
		name = unicodedata.normalize('NFKD',name).encode('ascii','ignore')
		variableName = re.sub(r'\W+','',name)
		headerFile.write('MTGCard* '+variableName+';\n')
		outFile.write('cd.'+variableName+'=NewMTGCard("'+variableName+'"); ')
		if downloadImages:
			os.system('curl "http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid='+str(multiverse_id)+'&type=card" -o images/'+variableName+'.jpg')
		if 'types' in node:
			for t in node['types']:
				if t in types:
					outFile.write('cd.'+variableName+'->is_'+t+'=true; ')
				else:
					print t
		if 'subtypes' in node:
			for t in node['subtypes']:
				subtypes.add(t)
				outFile.write('cd.'+variableName+'->is_'+t+'=true; ')
		if 'power' in node and node['power'].isnumeric():
			outFile.write('cd.'+variableName+'->power='+node['power']+';')
		if 'toughness' in node and node['power'].isnumeric():
			outFile.write('cd.'+variableName+'->toughness='+node['toughness']+';')
		cost = node['cost']
		outFile.write("\n")
headerFile.write('} CardData;\n\n')
headerFile.write('CardData loadCardData();\n')
headerFile.close()
outFile.write('return cd;\n}\n')
outFile.close()

#print types
subtypeFile = open('Subtypes.h','w')
subtypeFile.write('#define SUBTYPE_PLACEHOLDER \\\n')
for s in subtypes:
	subtypeFile.write('bool is_'+s+';\\\n')
subtypeFile.write('bool has_multitype;\n')
subtypeFile.close()
