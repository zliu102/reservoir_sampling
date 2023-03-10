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
static bool initialized = false;
static bytea *cmp;
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
    
    bytea  *addr = (bytea *) PG_GETARG_BYTEA_P(0);
    elog(INFO, "addr is %p",addr);
    int64 newsample = PG_GETARG_INT64(1);
    state_c *s = palloc (sizeof(state_c));
    if(!initialized){
        cmp=addr;
    }
    if (cmp == addr) {
        elog(INFO, "cmp == addr");
        elog(INFO, "cmp is %p",cmp);
    } else {
        elog(INFO, "cmp != addr");
        elog(INFO, "cmp is %p",cmp);
    }
    if (cmp == addr){
            state_c *st0 = (state_c *) palloc (sizeof(state_c));
            elog(INFO, "st0 is %p",st0);   
            addr = (bytea *) palloc(sizeof(st0) + sizeof(bytea));
            SET_VARSIZE(addr,sizeof(st0)+sizeof(bytea));      
            st0->poscnt = 1;
            st0->reservoir_size = 3;
            ArrayType *a = MyNew_intArrayType(3);
            st0->reservoir = a;
            memcpy(VARDATA(addr), &st0, sizeof(void *));
            initialized = true;

    }
        
        void **new_ptr = (void **) VARDATA(addr);
        s= (state_c *) (*new_ptr);
        elog(INFO, "s is %p",s);
        elog(INFO, "s poscnt is %d,reservoir_size is %d",s->poscnt,s->reservoir_size);
        if(s->poscnt <= s->reservoir_size){
            elog(INFO, "case 1");
            int32 p = s->poscnt;
            int64 *dr = (int64 *) ARR_DATA_PTR(s->reservoir);
            dr[p-1] = newsample;
            elog(INFO, "newsample is %ld",newsample);
            s->poscnt ++;
        }else{
            elog(INFO, "case 2");
            int32 pos = rand() % s->poscnt ;
            elog(INFO, "pos is %d",pos);//0 - postcnt -1
            elog(INFO, "newsample is %ld",newsample); 
            if(pos < s->reservoir_size){
                int64 *dr = (int64 *) ARR_DATA_PTR(s->reservoir);
                dr[pos] = newsample;
            }
            s->poscnt ++;
        }
        elog(INFO, "----------------"); 
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

                state_c *st = palloc (sizeof(state_c));
                bytea  *addr = (bytea *) PG_GETARG_BYTEA_P(0);
                void **new_ptr = (void **) VARDATA(addr);
                st= (state_c *) (*new_ptr);
                elog(INFO, "st is %p",st);
                elog(INFO, "st poscnt is %d,reservoir_size is %d",st->poscnt,st->reservoir_size);
                num = st->reservoir_size;
                dr = (int64 *) ARR_DATA_PTR(st->reservoir); 
                
                elems = (Datum *)palloc(num * sizeof(Datum));
                
                for (i = 0; i < num; i++) {
                    elems[i] = Int64GetDatum(dr[i]); 
                    elog(INFO, "dr[%d] is %ld",i,dr[i]);
                    elog(INFO, "elems[%d] is %ld",i,elems[i]);
                }
                //int nbytes = ARR_OVERHEAD_NONULLS(1) + sizeof(int) * num;
                //result = (ArrayType *) palloc0(nbytes);
                
                result = construct_array((Datum *)elems, num , INT8OID, sizeof(int64), true, 'i');
                /*if (ARR_NDIM(result) != 1 ){
                     elog(INFO, "yes1");
                 }
                if (ARR_HASNULL(result)) {
                    elog(INFO, "yes2");
                }
                if(ARR_ELEMTYPE(result) != INT8OID){
                    elog(INFO, "yes3");
                }
                if (result && ARR_ELEMTYPE(result) == INT8OID) {
                    elog(INFO, "yes4");
                }*/
               
                //pfree(addr);
                initialized = false;
                elog(INFO, "before retrun initialized is %d",initialized);
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
