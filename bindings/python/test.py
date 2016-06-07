# test.py - Script for testing elements python bindings.
# Copyright (C) 2016 Antoine Mercier-Linteau
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Note: it's possible to debug the c++ called from python using 
# gdb /usr/bin/python test.py

from elements import utils

########################### Test data structures #########################

# Note: data structures contained in utils/utils.cpp have not been defined
# and are only meant to be used internally by the framework. Scripting
# languages already provide powerful enough data structure objects.

print "*** testing MemFile ... "

print " > file reading ... ",
file = utils.MemFile("123456789")
if(file.read(3)[1] != b'123'):
    print "(failed)"
else:
    file.set_cursor(2)
    if(file.read(3)[1] != b'345'):
        print "(failed)"
    else:
        print "(done)"

print " > file writing ... ",
file.set_cursor(0)
file.write("aa", 2)
file.set_cursor(0)
if(file.read(3)[1] != b'aa3'):
    print "(failed)"
else:
    print "(done)"
     
print "*** (done)"

########################### Test request parsing #########################

from elements import core

def parse_message(message, data, expected):
    result = message.parse(data, len(data))
    if(result != expected):
        print "(failed [ returned", result, "])"
    else:
        print "(done)"

print "*** testing Request ..."

print " > valid request parsing ... ",
parse_message(
    core.Request(),
    "GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456",
    core.Message.PARSING_COMPLETE
)    

print "   > invalid request parsing ... ",
parse_message(
    core.Request(),
    "GET .?v=4&#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456",
    core.Message.HEADER_MALFORMED
)    

print "*** (done)"
    
########################### Test response parsing #########################

print "*** testing Response ... "

print "   > valid response parsing ... ",
parse_message(
    core.Response(),
    "HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\n123456",
    core.Message.PARSING_COMPLETE
)

print "   > invalid response parsing ... ",
parse_message(
    core.Response(),
    "HTTP/1.1 SERVER ERROR\r\n\r\n",
    core.Message.HEADER_MALFORMED
)

print "*** (done)"