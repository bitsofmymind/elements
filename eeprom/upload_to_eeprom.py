#/usr/bin/python

import argparse
import sys
import os
import termios

parser = argparse.ArgumentParser(description = 'Uploads a file to an AVR Elements\' EEPROM.')
parser.add_argument('cmd', choices = ['format', 'delete', 'append'], help = 'Command to be executed')
parser.add_argument('--tty', type = argparse.FileType('w'), default='/dev/ttyUSB0', help = 'Path to the tty.')
parser.add_argument('--src', type = file, help = 'Path to the file to be uploaded.')
parser.add_argument('--name', help = 'Name of the file on the EEPROM. Maximum length is 13 characters')



args = parser.parse_args()
file = args.src
name = args.name
tty = args.tty
cmd = args.cmd


if len(name) > 13:
    print >> sys.stderr,  'Name is too long, a maximum of 13 characters is supported'
    raise Exception()

try: 
    
    str = 'Preparing to '
    cmd_data = ''
    if cmd is 'format':
        str +=  ' format file _system'
        cmd_data = 'fmt'
    elif cmd is 'delete':
        str += ' delete '+ file.name
        cmd_data = 'del'
    elif cmd is 'append':
        str+= ' append ' \
        + str(os.fstat(file.fileno())[6]) \
        + ' bytes to' \
        + name \
        + ' from ' \
        + file.name
        cmd_data = 'dat'
    
    str += ' through ' + tty.name
    print str
    
    
    print 'Configuring serial port'
    params = termios.tcgetattr(tty.fileno())
    params[2] = 48 # For 8N1
    params[4] = termios.B9600
    params[5] = termios.B9600
    termios.tcsetattr(tty.fileno(), termios.TCSADRAIN, params )
    
    print 'Sending ' + cmd + ' command'
    cmd_data += '\0' + name
    for i in range(20 - len(cmd_data)): cmd_data[i] += '\0' #padding with null chars
    tty.write(cmd_data)
    
    if cmd is 'append':
        progress = 0
        total = os.fstat(file.fileno())[6]
        while True:
            print 'Sending data ( ' + progress + ' / ' + str(total) +' )',
            to_send = file.read(20)
            
            if len(to_send) < 20:
                for i in range(20 - len(to_send)): to_send[i] += '\0' #padding with null chars
                tty.write(to_send)
                break;
            tty.write(to_send)
        
        
    print "Done!"
    
finally:

    tty.close()
    if cmd is 'append': file.close()
