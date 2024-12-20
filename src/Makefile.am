# src/Makefile.am

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_CFLAGS = -Wall -Werror

OBJDIR = $(top_builddir)obj

bin_PROGRAMS = $(top_builddir)/bin/funlang
__top_builddir__bin_funlang_SOURCES = main.c utils.c front-end/lexer.c
