# this py will be called twice while refrsh FSP.fd to enabled FSP building and Normal BIOS building
# check fbuild.bat for more details

import sys
from os.path import exists


boardStepping="A2"
# argument 1 as setting control
if len(sys.argv)>1:
    boardStepping=sys.argv[1]

#     2 fields for Token control to re-fresh FSP.fd:
#     Token file name, Token Name
switchTokens = ["make_ComHpcAlt.sh, BOARD_STEPPING"]

for tokenFile in switchTokens:
    fileToken=tokenFile.split(',')
    if (exists(fileToken[0])):
        with open(fileToken[0], 'r+') as tokenFileH:
            tokenFileLines=tokenFileH.readlines()
            tokenFileH.seek(0)
            newlines=[]
            changeValue=False

            for line in tokenFileLines:
                if (fileToken[1].strip() in line and not changeValue):
                    words=line.split('=')
                    line=line.replace(words[1], boardStepping.strip()+'\n')
                    changeValue=True
                newlines.append(line)

            tokenFileH.writelines(newlines)
