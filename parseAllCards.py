#!/usr/bin/python

import json
import unicodedata

f = open('deckbrew.json')
data = f.read()
f.close()

cards=json.loads(data)

outFile = open('CardData.c','w')
for page in cards:
	for node in page:
		name = unicodedata.normalize('NFKD',node['name']).encode('ascii','ignore')
		outFile.write(name+'\n')
outFile.close()
