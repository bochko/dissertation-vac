import re
import sys
import os
from subprocess import call

'''
Open the last edited file
'''

WORDS = ["OPEN", "EDITED", "VIEWED", "LAST", "FILE"]

match_reg = r'\bopen last file\b|\bopen edited file\b|\bopen last edited\b|\bopen last viewed\b|\blast file edited open\b|\blast file\b|\bflast edited file\b|\blast viewed file\b|\bedited\b'




def handle(text, mic, profile):
	LEF = ""
	database_path = "/home/boyan/Dissertation/dissertation-main/.hidden/data.db"

	with open(database_path, "r") as fp:
		mic.say("Searching for files")
		line = fp.readline()
		while line:
			print(line)
			if "LEF " in line:
				LEF = line[4:]
				print("LEF is: %s" % LEF)
				break
			else:
				line = fp.readline()

	if LEF == "":
		mic.say("I found no recently edited files")
	else:
		mic.say("Opening your last edited file")
		os.system(("xdg-open " + LEF))
	
def isValid(text):
	return bool(re.search(match_reg, text, re.IGNORECASE))
