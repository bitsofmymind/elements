/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.7
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */

#ifndef PHP_BASE_H
#define PHP_BASE_H

extern zend_module_entry base_module_entry;
#define phpext_base_ptr &base_module_entry

#ifdef PHP_WIN32
# define PHP_BASE_API __declspec(dllexport)
#else
# define PHP_BASE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(base);
PHP_MSHUTDOWN_FUNCTION(base);
PHP_RINIT_FUNCTION(base);
PHP_RSHUTDOWN_FUNCTION(base);
PHP_MINFO_FUNCTION(base);

ZEND_NAMED_FUNCTION(_wrap_new_Resource);
ZEND_NAMED_FUNCTION(_wrap_Resource_add_child);
ZEND_NAMED_FUNCTION(_wrap_Resource_remove_child);
ZEND_NAMED_FUNCTION(_wrap_Resource_get_number_of_children);
ZEND_NAMED_FUNCTION(_wrap_Resource_get_name);
ZEND_NAMED_FUNCTION(_wrap_Resource_get_parent);
ZEND_NAMED_FUNCTION(_wrap_Authority_redirect_url_set);
ZEND_NAMED_FUNCTION(_wrap_Authority_redirect_url_get);
ZEND_NAMED_FUNCTION(_wrap_new_Authority);
ZEND_NAMED_FUNCTION(_wrap_new_Message);
ZEND_NAMED_FUNCTION(_wrap_Message_get_age);
ZEND_NAMED_FUNCTION(_wrap_Message_get_to_url);
ZEND_NAMED_FUNCTION(_wrap_Message_get_from_url);
ZEND_NAMED_FUNCTION(_wrap_Message_get_type);
ZEND_NAMED_FUNCTION(_wrap_Message_get_dispatching_type);
ZEND_NAMED_FUNCTION(_wrap_Message_set_dispatching_type);
ZEND_NAMED_FUNCTION(_wrap_Message_c_print);
ZEND_NAMED_FUNCTION(_wrap_Message_serialize);
ZEND_NAMED_FUNCTION(_wrap_Message_parse);
ZEND_NAMED_FUNCTION(_wrap_Message_set_body);
ZEND_NAMED_FUNCTION(_wrap_Message_get_body);
ZEND_NAMED_FUNCTION(_wrap_Message_unset_body);
ZEND_NAMED_FUNCTION(_wrap_Message_to_destination);
ZEND_NAMED_FUNCTION(_wrap_Message_next);
ZEND_NAMED_FUNCTION(_wrap_Message_previous);
ZEND_NAMED_FUNCTION(_wrap_Message_current);
ZEND_NAMED_FUNCTION(_wrap_new_Processing);
ZEND_NAMED_FUNCTION(_wrap_Processing_step);
ZEND_NAMED_FUNCTION(_wrap_Processing_start);
ZEND_NAMED_FUNCTION(_wrap_new_Request);
ZEND_NAMED_FUNCTION(_wrap_Request_c_print);
ZEND_NAMED_FUNCTION(_wrap_Request_serialize);
ZEND_NAMED_FUNCTION(_wrap_Request_find_arg);
ZEND_NAMED_FUNCTION(_wrap_Request_is_method);
ZEND_NAMED_FUNCTION(_wrap_Response_location_set);
ZEND_NAMED_FUNCTION(_wrap_Response_location_get);
ZEND_NAMED_FUNCTION(_wrap_new_Response);
ZEND_NAMED_FUNCTION(_wrap_Response_set_to_url);
ZEND_NAMED_FUNCTION(_wrap_Response_set_from_url);
ZEND_NAMED_FUNCTION(_wrap_Response_get_status_code);
ZEND_NAMED_FUNCTION(_wrap_Response_set_status_code);
ZEND_NAMED_FUNCTION(_wrap_Response_get_request);
ZEND_NAMED_FUNCTION(_wrap_Response_set_request);
ZEND_NAMED_FUNCTION(_wrap_Response_c_print);
ZEND_NAMED_FUNCTION(_wrap_Response_serialize);
ZEND_NAMED_FUNCTION(_wrap_new_URL);
ZEND_NAMED_FUNCTION(_wrap_URL_get_protocol);
ZEND_NAMED_FUNCTION(_wrap_URL_get_authority);
ZEND_NAMED_FUNCTION(_wrap_URL_get_port);
ZEND_NAMED_FUNCTION(_wrap_URL_get_fragment);
ZEND_NAMED_FUNCTION(_wrap_URL_get_resources);
ZEND_NAMED_FUNCTION(_wrap_URL_get_arguments);
ZEND_NAMED_FUNCTION(_wrap_URL_append_resource);
ZEND_NAMED_FUNCTION(_wrap_URL_insert_resource);
ZEND_NAMED_FUNCTION(_wrap_URL_parse);
ZEND_NAMED_FUNCTION(_wrap_URL_serialize);
ZEND_NAMED_FUNCTION(_wrap_URL_c_print);
ZEND_NAMED_FUNCTION(_wrap_URL_is_absolute);
#endif /* PHP_BASE_H */
