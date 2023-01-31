/*-------------------------------------------------------------------------
 *
 * binary_search.c
 *	  PostgreSQL type definitions for BINARY_SEARCHs
 *
 * Author:	Boris Glavic
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/binary_search/binary_search.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "reservoir_sampling.h"

#include "c.h"
#include "catalog/pg_collation_d.h"
#include "catalog/pg_type_d.h"
#include "fmgr.h"
#include "lib/stringinfo.h"
#include "nodes/parsenodes.h"
#include "nodes/makefuncs.h"
#include "parser/parse_func.h"
#include "utils/array.h"
#include "utils/arrayaccess.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/selfuncs.h"
#include "utils/typcache.h"
#include "utils/varbit.h"

PG_MODULE_MAGIC;

typedef struct state_c
{
		int [100] reservoir;
        int32 poscnt;
        int32 reservoir_size; 
} state_c;

PG_FUNCTION_INFO_V1(add_ab);
Datum
add_ab(PG_FUNCTION_ARGS)
{

        int32 arg_a = PG_GETARG_INT32(0);
        int32 arg_b = PG_GETARG_INT32(1);

        PG_RETURN_INT32(arg_a + arg_b);
}


PG_FUNCTION_INFO_V1(res_tras_crimes);
Datum
res_tras_crimes(PG_FUNCTION_ARGS)
{

		//state_c *d1 = malloc(sizeof(state_c));

		struct state_c *st = (state_c *)PG_GETARG_POINTER(0);
        int64 newsample = PG_GETARG_INT64(1);
        newsample = newsample + 1;
        //if(!st) {
        int [100]r = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        st->reservoir = r;
        st->poscnt = 1;
        st->reservoir_size = 100;
        //}
        PG_RETURN_DATUM((Datum) st);
}

PG_FUNCTION_INFO_V1(finalize_trans_crimes);
Datum
finalize_trans_crimes(PG_FUNCTION_ARGS)
{

		struct state_c st = (state_c) PG_GETARG_DATUM(0);
		PG_RETURN_ARRAYTYPE_P(st.reservoir);
}
