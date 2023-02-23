# contrib/reservoir_sampling/Makefile

MODULE_big = reservoir_sampling
OBJS = reservoir_sampling.o $(WIN32RES)
EXTENSION = reservoir_sampling
DATA = reservoir_sampling--1.0.sql
PGFILEDESC = "reservoir_sampling - binary search for integer arrays"
PG_CFLAGS += -ggdb -O0

REGRESS = reservoir_sampling

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/reservoir_sampling
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
