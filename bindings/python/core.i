/* core.i */
%module(directors="1", allprotected="1") core
%{    
    #include "configuration.h"
    #include "../../core/resource.h"
    #include "../../core/authority.h"
    #include "../../core/message.h"
    #include "../../core/processing.h"
    #include "../../core/request.h"
    #include "../../core/response.h"
    #include "../../core/url.h"
%}

// Report C++ errors in Python.
%feature("director:except") {
    if( $error != NULL ) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch( &ptype, &pvalue, &ptraceback );
        PyErr_Restore( ptype, pvalue, ptraceback );
        PyErr_Print();
        Py_Exit(1);
    }
} 

// generate directors for all classes that have virtual methods
%feature("director") Resource;    
%feature("director") Message;  
%feature("nodirector") Url;  

%include "stdint.i"
%include "configuration.h"

// Not sure why but these two get mixed up.
%rename(process_request) process(const Request* request, Response* response);
%rename(process_response) process(const Response* response);

%include "../../core/resource.h"
%include "../../core/authority.h"
%include "../../core/message.h"
%include "../../core/processing.h"
%include "../../core/request.h"
%include "../../core/response.h"
%include "../../core/url.h"