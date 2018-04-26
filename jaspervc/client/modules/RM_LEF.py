import re
import sys
import os
from subprocess import call

'''
Open the last edited file
'''

WORDS = ["DELETE", "EDITED", "LAST"]

match_reg = r'\bdelete last file\b|\bdelete edited file\b|\bdelete last edited\b|\bdelete last viewed\b|\blast file edited delete\b|\bdelete last file\b|\bdelete last edited file\b|\bremove\b|\bdelete\b'




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
		mic.say("If this was real life I would delete %s" % LEF)
	
def isValid(text):
	return bool(re.search(match_reg, text, re.IGNORECASE))
