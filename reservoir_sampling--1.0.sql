create function add_lzy(int,int)
        returns int
as 'MODULE_PATHNAME','add_ab'
language C strict;

CREATE FUNCTION res_trans_crimes_c(bytea, bigint)
        RETURNS bytea
        AS 'MODULE_PATHNAME', 'res_trans_crimes_c'
        LANGUAGE C
        IMMUTABLE 
        PARALLEL SAFE;

CREATE FUNCTION finalize_trans_crimes_c(bytea)
        RETURNS ArrayType
        AS 'MODULE_PATHNAME','finalize_trans_crimes_c'
        LANGUAGE C
        IMMUTABLE 
        PARALLEL SAFE;

CREATE AGGREGATE reservoir_sampling_c(bigint)
(
        sfunc = res_trans_crimes_c,
        stype = bytea,
        FINALFUNC = finalize_trans_crimes_c,
        INITCOND=NULL
);

/*
CREATE FUNCTION res_tras_crimes2_c(Datum, int64)
        RETURNS Datum
        AS 'MODULE_PATHNAME', 'res_tras_crimes'
        LANGUAGE C
        IMMUTABLE 
        PARALLEL SAFE;

CREATE FUNCTION finalize_trans_crimes2_c(Datum)
        RETURNS ArrayType
        AS 'MODULE_PATHNAME'
        LANGUAGE C
        IMMUTABLE 
        PARALLEL SAFE;

CREATE AGGREGATE reservoir_sampling2_c(int64)
(
        sfunc = res_tras_crimes2_c,
        stype = state_c,
        FINALFUNC = finalize_trans_crimes2_c,
);*/