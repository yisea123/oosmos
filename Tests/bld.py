import os

oosmos_dir = r'..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c  = oosmos_dir+r'\Source\oosmos.c'
pin_c     = oosmos_dir+r'\Classes\pin.c'
btn_c     = oosmos_dir+r'\Classes\btn.c'

os.system(oosmos_dir + r'\Gen\gen.exe tests.json')

oosmos.cWindows.Compile(oosmos_dir, ['ObjectThreads.c',           oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['History.c',                 oosmos_c, pin_c, btn_c], '-DbtnMaxButtons=4')
oosmos.cWindows.Compile(oosmos_dir, ['TimeConversions.c',         oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['FreeRunningMicroseconds.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['ThreadComplete.c',          oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['Action.c',                  oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['ThreadEvents.c',            oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['Poll.c',                    oosmos_c], '-Doosmos_ORTHO')
