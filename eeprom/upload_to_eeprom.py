#!/usr/bin/python

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

#PROTOCOL DESCRIPTION
#The upload protocol works with a fixed packet length of 24 bytes. The first packet
#is always the command which can be one of the following:
# - format: format the file system.
# - delete: delete a file on the file system.
# - append: append a file to the file system.
#If the delete of append arguments are selected, a file name will follow the command with
#a null character as a separator and also padded with null characters to 24 bytes. 
#If the append argument was selected, the content of the file will follow as packets of 24 bytes.
#
#Between packets an acknowledgment is expected. For the append comment, the last packet
#padded with spaces as null characters have a special significance in XML. For now, the
#protocol is only meant to transfer clear text that will ignore trailing spaces in files,
#such as CSS, javascript, HTML, SVG, XHTML, etc.
#
#Once an operation has been attempted, an interval of 2 seconds should pass to
#allow the protocol to reset itself.

#IMPORTS
import argparse
import sys
import os
import termios
import time

#Initialize command line parser.
parser = argparse.ArgumentParser(description = 'Uploads a file to an AVR Elements\' EEPROM.')

#Adds arguments to the command line parser.
parser.add_argument('--tty', default='/dev/ttyUSB0', help = 'Path to the tty.')
parser.add_argument('--src', type = file, help = 'Path to the file to be uploaded.')
parser.add_argument('--dest', help = 'Name of the file on the EEPROM. Maximum length is 15 characters')
parser.add_argument('cmd', choices = ['format', 'delete', 'append'], help = 'Command to be executed')


args = parser.parse_args() #Parse the arguments from the command line.
file = args.src #Extracts the file argument.
name = args.dest #extracts the name argument.
cmd = args.cmd #extracts the command argument.
#Opens a file to the tty. We need low-level IO here so we open the file the complicated way.
#Non blockin IO is necessary if we want to implement timing out.
tty = os.open(args.tty, os.O_RDWR|os.O_NONBLOCK)

#Declares an Exception class so we can recognize exception thrown by the script.
class ScriptException(Exception): pass

#The transferring is done within a try block to make sure nothing is left open
#if the transfer fails.
try:   
    #Name cannot be more than 13 characters so we check for this condition.
    if name and len(name) > 15:
        #Name is too long so print an error to stderr and exit.
        print >> sys.stderr,  'Name is too long, a maximum of 15 characters is supported!'
        raise ScriptException() #An Exception is raised so the finally block gets executed.
 
    #Prints a message informing the user on what is about to happen and
    #formats the cmd_data field.
    print 'Preparing to', #Prints the beginning of the message.
    cmd_data = '' #The actual 3 character command that will be sent to the device.
    if cmd == 'format': #If the command is format.
        print  'format file system',
        cmd_data = 'fmt'
    elif cmd == 'delete': #If the command is delete.
        print ' delete ' + name,
        cmd_data = 'del'
    elif cmd == 'append': #If the command is an append.
        #Inform the user how many bytes will be transfered, the destination file and its source.
        print ' append ' + str(os.fstat(file.fileno())[6]) + ' bytes to ' + name + ' from ' + file.name,
        cmd_data = 'dat'
    
    print ' through ' + args.tty #Through which interface.
    
    #Configure the serial port.
    print 'Configuring serial port...',
    params = termios.tcgetattr(tty)
    params[2] = 48 # For 8N1
    params[4] = termios.B9600
    params[5] = termios.B9600
    termios.tcsetattr(tty, termios.TCSADRAIN, params ) #Sets the serial port attributes.
    print '[DONE]'
    
    #There migh still be leftover characters from a previous failed attempt
    #so we flush the file buffer.
    print 'Flushing serial port...',
    while True: #Infinite loop.
        try:
            char = os.read(tty, 1) #Attemps to fetch a char from the tty.
        except OSError: #No char in the buffer.
            break; #The buffer has been flushed.       
        if char == '': #If we have reached the end of the buffer.
            break; #The buffer has been flushed.           
    print '[DONE]'
    
    print 'Sending ' + cmd + ' command...',
    cmd_data += chr(0) #Terminate the command with a null character.
    if name: #If the name argument is set.
        cmd_data += name #Append the name to the command.
    for i in range(24 - len(cmd_data)): cmd_data += chr(0) #Pad with null chars.
    os.write(tty, cmd_data) #Write the data to the tty.
    
    #Define a function to receive acknowledgments from the device.
    def rec_ack():        
        rec = '' #The received code.
        timeout = 0 #The timeout counter.     
        while not len(rec): #While the rec variable is empty.
            if timeout > 100: #If it has been more than 100ms since the packet was sent.
                print '[TIMEOUT]'
                raise ScriptException() #An Exception is raised so the finally block gets executed.
            try:
                rec = os.read(tty, 1) #Attemps to fetch the rec from the tty.
            except OSError: #No data available.
                timeout += 1 #Increase the timeout counter by 1 millisecond.
                time.sleep(0.001) #Sleep for 1 millisecond.
        
        #Protocol Response definitions.
        ACK = '0'
        UNKNOWN_CMD = '1'
        FILE_TOO_BIG = '2'
        IO_ERROR = '3'
        FILE_NOT_FOUND = '4'
        PROTOCOL_ERROR = '5'
        DEBUG = '6' #Debug message, used while debugging the routine within the device
        
        #Send an error message or return depending on what was received.
        if rec == ACK : #If the command succeeded, just return;
            return
        elif rec == UNKNOWN_CMD: #If the command was not understood.
            print '[UKNOWN_CMD]'
        elif rec == FILE_TOO_BIG: #If the device EERPOM is full.
            print '[FILE_TOO_BIG]'
        elif rec == IO_ERROR: #If an IO error occurred.
            print '[IO_ERROR]'
        elif rec == FILE_NOT_FOUND: #If the file was not found.
            print '[FILE_NOT_FOUND]'
        elif rec == PROTOCOL_ERROR: #If the device received something it was not expecting.
            print '[PROTOCOL_ERROR]'
        elif rec == DEBUG: #If the device received a debug reply.
            print '[DEBUG]',
            rec_ack() #Calls itself to get the real reply.
            return;
        else:
            print '[INVALID_RESPONSE: '+ str(ord(rec)) + ']'
                
        raise ScriptException() #An Exception is raised so the finally block gets executed.
        

    rec_ack() #Receive the acknowledgment for the previous command.
    print '[DONE]' #Sending the command succeeded.
    
    if cmd == 'append': #If the command was append.        
        progress = 0 #The progress counter.
        total = os.fstat(file.fileno())[6] #The number of bytes to transfer.
        while True: #Infinite loop.
            print 'Sending file content to ' + name +'...[',
            #Use stdout to allow finer control of the std.
            #Prints the progression.
            sys.stdout.write( str(progress) + ' / ' + str(total) +']')
            sys.stdout.flush() #Flushes the current line to the std.
            if progress >= total: #If we are done sending the file.
                print '[DONE]' #Prints done and a line break.
                break #Ends the loop.
            else:
                #Prints a return to line character so the next line gets printed over.
                sys.stdout.write('\r')
            #Attempts to read 24 characters from the file. 
            to_send = file.read(24)
            #Less than 24 characters mean we have exhausted the file.
            if len(to_send) < 24:
                for i in range(24 - len(to_send)): to_send += ' ' #Pad with spaces, null chars get parsed by the XML parser
            os.write(tty, to_send) #Write the packet to the TTY.
            rec_ack() #Wait for the acknowledgment.
            progress += 24 #Some characters were transfered si increase the counter.   
except ScriptException:
    #Catches ScriptException so stack dumping is avoided. 
    pass
finally:
    #In a finally block so it is ran whatever happens.    
    print 'Cleaning up...',
    os.close(tty) #Closes the open tty.
    if cmd == 'append': file.close() #Closes the file if the command was append.
    print '[DONE]'
