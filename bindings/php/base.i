/* base.i 
Since the name core conflicts with some zend definitions, the module is 
name base.
*/
%module base
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

%include "stdint.i"
%include "configuration.h"
%include "../../core/resource.h"
%include "../../core/authority.h"
%include "../../core/message.h"
%include "../../core/processing.h"
%include "../../core/request.h"
%include "../../core/response.h"
%include "../../core/url.h"