/* pal.i */
%module pal
%{
    #include "configuration.h"
    #include "../../pal/pal.h"
%}

%include "stdint.i"
%include "configuration.h"
%include "../../pal/pal.h"