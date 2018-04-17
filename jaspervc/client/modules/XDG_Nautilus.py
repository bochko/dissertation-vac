import re
import sys
from subprocess import call

'''
Opens Nautilus from a non-context
'''

WORDS = ["OPEN", "NAUTILUS", "EXPLORER"]

match_reg = r'\bopen nautilus\b|\bnautilus open\b|\bopen file manager\b|\bfile manager open\b|\bopen explorer\b|\bopen file explorer\b|\bfile explorer open\b|\bfile explorer\b|\bnautilus\b'
	

def handle(text, mic, profile):
	mic.say("Opening nautilus")
	call("nautilus")
	

def isValid(text):
	return bool(re.search(match_reg, text, re.IGNORECASE))
	
