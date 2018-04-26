import re
import sys
import os
from subprocess import call

'''
Open the last edited file
'''

WORDS = ["CREATED", "LAST", "FILE"]

match_reg = r'\blast created file\b|\bopen created file\b|\bopen last created\b|\bopen last created\b|\blast file created open\b|\blast created\b|\bflast created file\b|\blast created file\b|\bcreated\b'




def handle(text, mic, profile):
	LCF = ""
	database_path = "/home/boyan/Dissertation/dissertation-main/.hidden/data.db"

	with open(database_path, "r") as fp:
		mic.say("Searching for files")
		line = fp.readline()
		while line:
			print(line)
			if "LCF " in line:
				LEF = line[4:]
				print("LEF is: %s" % LEF)
				break
			else:
				line = fp.readline()

	if LEF == "":
		mic.say("I found no recently created files")
	else:
		mic.say("Opening your last created file")
		os.system(("xdg-open " + LEF))
	
def isValid(text):
	return bool(re.search(match_reg, text, re.IGNORECASE))
