import os
import sys
import re
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-E')
parser.add_argument('input', type=str, default = None, nargs='?')

args = parser.parse_args()

print(args.E)

if not os.isatty(sys.stdin.fileno()) and args.input != None:
    print("too much input")
    exit(1)
    
if os.isatty(sys.stdin.fileno()) and args.input == None:
    print("input needed")
    
    
INPUT = ""
if args.input != None:
    print("file input")
    INPUT = open(args.input, 'r').read()

if not os.isatty(sys.stdin.fileno()):
    INPUT = sys.stdin.read()
    print("std input")
    
# print(INPUT, type(INPUT), sep = ' ')

# for c in INPUT:
#     if c == '\n':
#         print('$')
#     else:
#         print(c, end=' ')
    
