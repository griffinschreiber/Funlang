# src/Makefile.am

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/include

AM_CFLAGS = -Wall -Werror

OBJDIR = $(topbuilddir)obj

bin_PROGRAMS = funlang
funlang_SOURCES = main.c utils.c front-end/lexer.c
