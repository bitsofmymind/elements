/* core.i */
%module(directors="1", allprotected="1") core
%{    
    #include "configuration.h"
    #include "../../pal/pal.h"
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
%include "../../pal/pal.h"

// Not sure why but these two get mixed up.
%rename(process_request) process(const Request* request, Response* response);
%rename(process_response) process(const Response* response);

%include "../../core/resource.h"
%include "../../core/authority.h"

/* 
 * Ignore this version of the serialize method and define one that is compatible
 * with SWIG.
 */
%ignore Message::serialize(char* buffer, bool write) const;
%include "cstring.i"
%cstring_output_allocate_size(char **buffer, int *length, free(*$1));
%extend Message
{
    void serialize(char** buffer, int *length)
    {
        *length = $self->serialize(*buffer, false);
        *buffer = (char *)malloc(*length);
        $self->serialize(*buffer, true);
    }
}
%include "../../core/message.h"

%include "../../core/processing.h"

%include "../../core/request.h"
%include "../../core/response.h"
// Remove this method, it is not useful in python an confuses SWIG.
%ignore URL::append_resource(const char* name, int8_t length);
%include "../../core/url.h"