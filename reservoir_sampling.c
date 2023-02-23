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
#include "postgres.h"
#include <limits.h>
#include "catalog/pg_type.h"
//#include "/home/oracle/datasets/postgres11ps/postgres-pbds/contrib/intarray/_int.h"

PG_MODULE_MAGIC;

typedef struct state_c
{
	ArrayType *reservoir;
        int32 poscnt;
        int32 reservoir_size; 
} state_c;

static ArrayType *MyNew_intArrayType(int num);
//static Datum charToInt(const char *addr);
//static void intToChar(unsigned int hex, char* str)

PG_FUNCTION_INFO_V1(add_ab);
Datum
add_ab(PG_FUNCTION_ARGS)
{

        int32 arg_a = PG_GETARG_INT32(0);
        int32 arg_b = PG_GETARG_INT32(1);

        PG_RETURN_INT32(arg_a + arg_b);
}


PG_FUNCTION_INFO_V1(res_trans_crimes_c);
Datum
res_trans_crimes_c(PG_FUNCTION_ARGS)
{

        //state_c *d1 = malloc(sizeof(state_c));
        //struct state_c st;
    //  st = (state_c *)PG_GETARG_DATUM(0);
    bytea  *addr = (bytea *) PG_GETARG_BYTEA_P(0);

    int64 newsample = PG_GETARG_INT64(1);
    state_c *s = palloc0 (sizeof(state_c)); 
    s->reservoir = MyNew_intArrayType(100);
        //state_c **pp2 = &s;
        //int len = sizeof(struct state_c);
    if(PG_ARGISNULL(0)) {
                elog(ERROR, "lzy");
                state_c *st0 = palloc0 (sizeof(state_c));
                ArrayType *a = MyNew_intArrayType(100);
                //addr = palloc0 (sizeof(bytea));
                state_c **pp = &st0; 
                addr = (bytea *) palloc(sizeof(st0) + sizeof(bytea));
                //memcpy(addr->vl_dat,pp,sizeof(st0));
                //memcpy(VARDATA(addr),pp,sizeof(st0));
                SET_VARSIZE(addr,sizeof(st0)+sizeof(bytea));
            
               st0->poscnt = 1;
               st0->reservoir_size = 3;
               st0->reservoir = a;
                //sprintf(addr->vl_dat, "%p", (void*) st0);
                sprintf(VARDATA(addr), "%p", (void*) st0);
                

    }
        //todo
        //sscanf(addr->vl_dat, "%p", (void**)&s); 
        sscanf(VARDATA(addr), "%p", (void**)&s); 
        //memcpy(pp2,addr->vl_dat,sizeof(*pp2));
        //memcpy(pp2,VARDATA(addr),sizeof(*pp2));
        //memcpy(s,(state_c *)((uintptr_t) *VARDATA(addr)),100);
        //int s = charToInt(ptraddr);
        if(s->poscnt <= s->reservoir_size){
            int32 p = s->poscnt;
                int64 *dr = (int64 *) ARR_DATA_PTR(s->reservoir);
                dr[p-1] = newsample;
            s->poscnt ++;

        }else{
            int32 pos = rand() % s->poscnt ; //0 - postcnt -1
            if(pos < s->reservoir_size){
                        int64 *dr = (int64 *) ARR_DATA_PTR(s->reservoir);
                        dr[pos] = newsample;
            }
            s->poscnt ++;
        }
        //pfree(s);
        PG_RETURN_BYTEA_P(addr);
}

PG_FUNCTION_INFO_V1(finalize_trans_crimes_c);
Datum
finalize_trans_crimes_c(PG_FUNCTION_ARGS)
{               

                ArrayType *result;
                Datum *elems;
                int i;
                int num;
                int64 *dr;

                state_c *st = palloc0 (sizeof(state_c));
                bytea  *addr = (bytea *) PG_GETARG_BYTEA_P(0);
                //st->reservoir = MyNew_intArrayType(100);
                //state_c **pp2 = &st; 
                //memcpy(pp2,addr->vl_dat,sizeof(*pp2));
                //memcpy(pp2,VARDATA(addr),sizeof(*pp2));
                //int len = sizeof(struct state_c);
                //memcpy(st, (state_c *)((uintptr_t) *(addr->vl_dat)), 100);
                //sscanf(addr->vl_dat, "%p", (void**)&st); 
                sscanf(VARDATA(addr), "%p", (void**)&st);
                num = st->reservoir_size;
                dr = (int64 *) ARR_DATA_PTR(st->reservoir); 
                
                elems = (Datum *)palloc(num * sizeof(Datum));
                
                for (i = 0; i < num; i++) {
                        elems[i] = dr[i]; 
                }

                result = construct_array(elems, num , INT8OID, 8, true, 'd');
                PG_RETURN_ARRAYTYPE_P(result);
                //PG_RETURN_ARRAYTYPE_P(st->reservoir); 
}
static
ArrayType *
MyNew_intArrayType(int num)
{
        ArrayType  *r;
        int nbytes;

        /* if no elements, return a zero-dimensional array */
        if (num <= 0)
        {
                Assert(num == 0);
                r = construct_empty_array(INT8OID);
                return r;
        }

        nbytes = ARR_OVERHEAD_NONULLS(1) + sizeof(int) * num;

        r = (ArrayType *) palloc0(nbytes);

        SET_VARSIZE(r, nbytes);
        ARR_NDIM(r) = 1;
        r->dataoffset = 0;                      /* marker for no null bitmap */
        ARR_ELEMTYPE(r) = INT8OID;
        ARR_DIMS(r)[0] = num;
        ARR_LBOUND(r)[0] = 1;

        return r;
}
/*static Datum
charToInt(const char *str)
{
    int result = 0;
    int length = strlen(str);
    int i;
    for (i = 0; i < length; i++) {
        char c = str[i];
        int value;
        if (c >= '0' && c <= '9') {
            value = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            value = 10 + c - 'a';
        } else if (c >= 'A' && c <= 'F') {
            value = 10 + c - 'A';
        } else {
            return 0;
        }
        result = result * 16 + value;
    }
    return result;
}

static
void intToChar(unsigned int hex, char* str) {
    sprintf(str, "%x", hex);
}


PG_FUNCTION_INFO_V1(res_tras_crimes2);
Datum
res_tras_crimes2_c(PG_FUNCTION_ARGS)
{

		//state_c *d1 = malloc(sizeof(state_c));
		//struct state_c st;
	//	st = (state_c *)PG_GETARG_DATUM(0);
        state_c *st = (state_c *)PG_GETARG_DATUM(0);
        int64 newsample = PG_GETARG_INT64(1);

        memset(&st, 0, sizeof(struct state_c));
        if (memcmp(&st, &st, sizeof(struct state_c)) == 0) {
        	int64 r[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        	int64 *a = r;
        	st.poscnt = 1;
        	st.reservoir_size = 100;
        	st.reservoir = a;
        }
        if (st == NULL) {
                int64 r[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                int64 *a = r;
                st->poscnt = 1;
                st->reservoir_size = 100;
                st->reservoir = a;
        }
        if(st->poscnt <= st->reservoir_size){
        	int32 p = st->poscnt;
        	*(st->reservoir+p-1) = newsample;
        	st->poscnt ++;

        }else{
        	int32 pos = rand() % st->poscnt ; //0 - postcnt -1
        	if(pos < st->reservoir_size){
        		*(st->reservoir+pos) = newsample;
        	}
        	st->poscnt ++;
        }
        PG_RETURN_DATUM((Datum) st);
}

PG_FUNCTION_INFO_V1(finalize_trans_crimes2);
Datum
finalize_trans_crimes2_c(PG_FUNCTION_ARGS)
{

	state_c *st = (state_c *) PG_GETARG_DATUM(0);
	PG_RETURN_ARRAYTYPE_P(st->reservoir);
}*/
