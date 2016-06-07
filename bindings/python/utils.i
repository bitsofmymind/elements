/* utils.i */
%module(directors="1", allprotected="1") utils
%{
    #include "configuration.h"
    #include "../../utils/file.h"
    #include "../../utils/memfile.h"
    #include "../../utils/template.h"
    //#include "../../utils/utils.h"
    #include "../../utils/utils.h"
%}

%include "stdint.i"
%include "configuration.h"

%include "cstring.i"
%cstring_output_withsize(char* buffer, size_t* length);
/* Since there is no obvious (safe) mapping for those functions, they are excluded.
 * Use Filed::read() instead.  */
%ignore File::extract(char* buffer);
%ignore File::read(char* buffer, size_t length);

// Generate directors for classes that inherit from File.
%feature("director") File;  

%include "../../utils/file.h"

/* Ignore these two constructors because they do not copy the data to a new 
 * buffer and will cause in-place writing to script strings.*/
%ignore MemFile::MemFile(char* data, bool is_const = false );
%ignore MemFile::MemFile(char* data, size_t length, bool is_const);

%include "../../utils/memfile.h"
%include "../../utils/template.h"
%include "../../utils/utils.h"