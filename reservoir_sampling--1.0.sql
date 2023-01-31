create function add_lzy(int,int)
        returns int
as 'MODULE_PATHNAME','add_ab'
language C strict;

CREATE FUNCTION res_tras_crimes(state_c, int64)
        RETURNS state_c
        AS 'MODULE_PATHNAME', 'res_tras_crimes'
        LANGUAGE C
        IMMUTABLE 
        PARALLEL SAFE;

CREATE FUNCTION finalize_trans_crimes(state_c)
        RETURNS ArrayType
        AS 'MODULE_PATHNAME'
        LANGUAGE C
        IMMUTABLE 
        PARALLEL SAFE;

CREATE AGGREGATE reservoir_sampling(int64)
(
        sfunc = res_tras_crimes,
        stype = state_c,
        FINALFUNC = finalize_trans_crimes,
);