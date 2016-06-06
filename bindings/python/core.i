/* core.i */
%module core
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