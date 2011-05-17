#/usr/bin/python

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


if  name and len(name) > 13:
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
    for i in range(20 - len(cmd_data)): cmd_data += '\0' #padding with null chars
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
#                print ack
            timeout += 1
            time.sleep(0.001)
            
        print ord(ack)

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
            to_send = file.read(20)
            if len(to_send) < 20:
                for i in range(20 - len(to_send)): to_send += '\0' #padding with null chars
            os.write(tty, to_send)
            rec_ack()
            progress += 20
        
        
    print "Done!"
    
finally:

    os.close(tty)
    if cmd == 'append': file.close()
