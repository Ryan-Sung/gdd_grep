from atexit import register
import os
import sys
import re
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-E', help = 'need a QUOTED regex input : \'pattern\'')
parser.add_argument('input', type=str, default = None, nargs='?')

args = parser.parse_args()

print(args.E)
# print(args)

if not os.isatty(sys.stdin.fileno()) and args.input != None:
    print("too much input")
    exit(1)
    
if os.isatty(sys.stdin.fileno()) and args.input == None:
    print("input needed")
    exit(1)
    
    
INPUT = ""
if args.input != None:
    print("file input")
    INPUT = open(args.input, 'r').read()

if not os.isatty(sys.stdin.fileno()):
    print("std input")
    INPUT = sys.stdin.read()
    
regex = args.E
if regex != None:
    regex = r'\b.*' + regex + r'.*\n'
    res = re.findall(regex, INPUT, re.MULTILINE)
    print(*res, sep='\n')
