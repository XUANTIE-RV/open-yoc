/*
 * $Id: cvi_json_object_private.h,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

/**
 * @file
 * @brief Do not use, cvi_json-c internal, may be changed or removed at any time.
 */
#ifndef _json_object_private_h_
#define _json_object_private_h_

#ifdef __cplusplus
extern "C" {
#endif

struct json_object;
#include "json_inttypes.h"
#include "json_types.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

/* cvi_json object int type, support extension*/
typedef enum cvi_json_object_int_type
{
	cvi_json_object_int_type_int64,
	cvi_json_object_int_type_uint64
} cvi_json_object_int_type;

struct cvi_json_object
{
	enum cvi_json_type o_type;
	uint32_t _ref_count;
	cvi_json_object_to_cvi_json_string_fn *_to_cvi_json_string;
	struct printbuf *_pb;
	cvi_json_object_delete_fn *_user_delete;
	void *_userdata;
	// Actually longer, always malloc'd as some more-specific type.
	// The rest of a struct cvi_json_object_${o_type} follows
};

struct cvi_json_object_object
{
	struct cvi_json_object base;
	struct lh_table *c_object;
};
struct cvi_json_object_array
{
	struct cvi_json_object base;
	struct array_list *c_array;
};

struct cvi_json_object_boolean
{
	struct cvi_json_object base;
	cvi_json_bool c_boolean;
};
struct cvi_json_object_double
{
	struct cvi_json_object base;
	double c_double;
};
struct cvi_json_object_int
{
	struct cvi_json_object base;
	enum cvi_json_object_int_type cint_type;
	union
	{
		int64_t c_int64;
		uint64_t c_uint64;
	} cint;
};
struct cvi_json_object_string
{
	struct cvi_json_object base;
	ssize_t len; // Signed b/c negative lengths indicate data is a pointer
	// Consider adding an "alloc" field here, if cvi_json_object_set_string calls
	// to expand the length of a string are common operations to perform.
	union
	{
		char idata[1]; // Immediate data.  Actually longer
		char *pdata;   // Only when len < 0
	} c_string;
};

void _cvi_json_c_set_last_err(const char *err_fmt, ...);

extern const char *cvi_json_hex_chars;

#ifdef __cplusplus
}
#endif

#endif
