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
keywords=[
#'Absorb',
#'Affinity',
#'Amplify',
#'Banding',
#'Bestow',
#'Bloodthirst',
#'Buyback',
#'Cipher',
'Convoke',
#'Dash',
'Deathtouch',
'Defender',
#'Delve',
#'Devoid',
#'Devour',
'Double strike',
#'Dredge',
#'Enchant',
#'Entwine',
#'Fading',
'First strike',
'Flash',
#'Flashback',
'Flying',
#'Forbidden',
#'Fuse',
'Haste',
'Hexproof',
#'Hidden agenda',
'Indestructible',
#'Infect',
'Intimidate',
#'Kicker',
#'Landhome',
'Lifelink',
#'Megamorph',
#'Morph',
#'Offering',
#'Overload',
#'Phasing',
#'Protection',
#'Prowl',
'Reach',
#'Retrace',
#'Shroud',
#'Splice',
#'Split second',
#'Sunburst',
#'Totem armor',
'Trample',
#'Tribute',
#'Unleash',
#'Vanishing',
'Vigilance',
#'Wither'
]
keywordSet=set()
targetSets=[
"Magic 2015 Core Set",
"Magic Origins"
]
cardslist=[]

headerFile = open('CardData.h','w')
headerFile.write('#pragma once\n\n')
headerFile.write('#include "MTGCard.h"\n')
headerFile.write('typedef struct {\n')
outFile = open('CardData.c','w')
outFile.write('#include "CardData.h"\n\n')
outFile.write('Ability* ab;\n')
outFile.write('CardData loadCardData() {\n')
outFile.write('CardData cd;\n')
for page in cards:
	for node in page:
		valid = False
		multiverse_id = 0
		for edition in node['editions']:
			if edition['set'] in targetSets:
				valid = True
				multiverse_id = edition['multiverse_id']
				break
		if not valid:
			continue
		name = node['name']
		name = name.replace(u'\xc6','Ae')
		name = unicodedata.normalize('NFKD',name).encode('ascii','ignore')
		variableName = re.sub(r'\W+','',name)
		cardslist.append(variableName)
		headerFile.write('MTGCard* '+variableName+';\n')
		outFile.write('cd.'+variableName+'=NewMTGCard("'+variableName+'",'+str(node['cmc'])+'); ')
		if downloadImages:
			os.system('curl "http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid='+str(multiverse_id)+'&type=card" -o images/'+variableName+'.jpg')
		if 'colors' in node:
			for color in node['colors']:
				outFile.write('cd.'+variableName+'->subtypes.is_'+color+'=true; ')
		if 'types' in node:
			for t in node['types']:
				if t in types:
					outFile.write('cd.'+variableName+'->subtypes.is_'+t+'=true; ')
				else:
					print t
		if 'subtypes' in node:
			for t in node['subtypes']:
				subtypes.add(t)
				outFile.write('cd.'+variableName+'->subtypes.is_'+t+'=true; ')
		for k in keywords:
			if k in node['text']:
				k_ = k.lower().replace(' ','_')
				keywordSet.add(k_)
				outFile.write('cd.'+variableName+'->subtypes.is_'+k_+'=true; ')
		has_instant_ability = False
		for s in node['text'].split('\n'):
			if re.match(r'\{[\w\{\}, ]*\}',s):
				outFile.write('ab=NewAbility(); ')
				req = re.match(r'\{[\w\{\}, ]*\}',s).group(0)
				is_instant = True
				for r in req:
					if r.isnumeric():
						outFile.write('AppendToList(ab->manaCost,colorlessMana('+r+')); ')
					elif r=='T':
						outFile.write('ab->needs_tap=true; ')
						is_instant = False
					elif r.isalpha():
						outFile.write('AppendToList(ab->manaCost,'+r+'_Mana(1)); ')
				if is_instant:
					has_instant_ability = True
				outFile.write('AppendToList(cd.'+variableName+'->abilities,ab); ')
		if has_instant_ability:
			outFile.write('cd.'+variableName+'->subtypes.has_instant=true; ')
		if 'power' in node and node['power'].isnumeric():
			outFile.write('cd.'+variableName+'->power='+node['power']+'; ')
		if 'toughness' in node and node['toughness'].isnumeric():
			outFile.write('cd.'+variableName+'->toughness='+node['toughness']+'; ')
		if 'loyalty' in node:
			outFile.write('cd.'+variableName+'->loyalty='+str(node['loyalty'])+'; ')
		cost = node['cost']
		if cost:
			costList = cost[1:-1].split('}{')
			i=0
			while i < len(costList):
				if costList[i].isnumeric():
					outFile.write('AppendToList(cd.'+variableName+'->manaCost,colorlessMana('+costList[i]+')); ')
				else:
					count=1
					while i+1 < len(costList) and costList[i] == costList[i+1]:
						count += 1
						i += 1
					outFile.write('AppendToList(cd.'+variableName+'->manaCost,'+costList[i]+'_Mana('+str(count)+')); ')
				i+=1
		outFile.write("\n")
headerFile.write('} CardData;\n\n')
headerFile.write('CardData loadCardData();\n')
headerFile.write('void freeCardData(CardData* cd);\n')
headerFile.close()
outFile.write('return cd;\n}\n\n')
outFile.write('void freeCardData(CardData* cd) {\n')
for c in cardslist:
	outFile.write('DeleteMTGCard(cd->'+c+');\n')
outFile.write('}\n')
outFile.close()

#print types
subtypeFile = open('Subtypes.h','w')
subtypeFile.write('#define SUBTYPE_PLACEHOLDER \\\n')
for s in subtypes:
	subtypeFile.write('bool is_'+s+';\\\n')
for s in keywordSet:
	subtypeFile.write('bool is_'+s+';\\\n')
subtypeFile.write('bool has_instant;\n')
subtypeFile.close()
