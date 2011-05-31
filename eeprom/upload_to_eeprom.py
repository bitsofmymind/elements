#/usr/bin/python

#upload_to_eeprom.py - A script for quick transfers to the eeprom_24LCxx resource
#Copyright (C) 2011 Antoine Mercier-Linteau
#
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import sys
import os
import termios
import time

parser = argparse.ArgumentParser(description = 'Uploads a file to an AVR Elements\' EEPROM.')
parser.add_argument('--tty', default='/dev/ttyUSB0', help = 'Path to the tty.')
parser.add_argument('--src', type = file, help = 'Path to the file to be uploaded.')
parser.add_argument('--name', help = 'Name of the file on the EEPROM. Maximum length is 13 characters')
parser.add_argument('cmd', choices = ['format', 'delete', 'append'], help = 'Command to be executed')


args = parser.parse_args()
file = args.src
name = args.name
tty = os.open(args.tty, os.O_RDWR) #We need low-level IO here so we open the file the complicated way
cmd = args.cmd


if  name and len(name) > 21:
    print >> sys.stderr,  'Name is too long, a maximum of 13 characters is supported'
    raise Exception()

try: 
    
    out = 'Preparing to '
    cmd_data = ''
    if cmd == 'format':
        out +=  ' format file system'
        cmd_data = 'fmt'
    elif cmd == 'delete':
        out += ' delete ' + name
        cmd_data = 'del'
    elif cmd == 'append':
        out += ' append ' + str(os.fstat(file.fileno())[6]) + ' bytes to ' + name + ' from ' + file.name
        cmd_data = 'dat'
    
    out += ' through ' + args.tty
    print out
    
    
    print 'Configuring serial port'
    params = termios.tcgetattr(tty)
    params[2] = 48 # For 8N1
    params[4] = termios.B9600
    params[5] = termios.B9600
    termios.tcsetattr(tty, termios.TCSADRAIN, params )
    
    print 'Sending ' + cmd + ' command'
    cmd_data += '\0'
    if name:
        cmd_data += name
    for i in range(24 - len(cmd_data)): cmd_data += '\0' #padding with null chars
    os.write(tty, cmd_data)
    
    def rec_ack():
        #sys.stdin.read(1)
        ack = ''
        timeout = 0
        while(not len(ack)):
            if timeout >= 100:
                print >> sys.stderr,  'timeout'
                raise Exception()
            ack = os.read(tty, 1)
            if len(ack) and ( ord(ack) == 10 ):
                ack = ''
            timeout += 1
            time.sleep(0.001)

        if ack != '1':
             print >> sys.stderr,  'nack'
             raise Exception()

    rec_ack()
    
    if cmd == 'append':
        progress = 0
        total = os.fstat(file.fileno())[6]
        while True:
            sys.stdout.write( 'Sending data ( ' + str(progress) + ' / ' + str(total) +' )\r')
            sys.stdout.flush()   
            if progress >= total: 
                print '' 
                break
            to_send = file.read(24)
            if len(to_send) < 24:
                for i in range(24 - len(to_send)): to_send += ' ' #padding with spaces, null chars get parsed by the XML parser
            os.write(tty, to_send)
            rec_ack()
            progress += 24
        
        
    print "Done!"
    
finally:

    os.close(tty)
    if cmd == 'append': file.close()
