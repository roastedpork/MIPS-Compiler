#!/bin/bash
make bin/c_compiler

cat testscript/tmp1.c |  ./bin/c_compiler > testscript/tmp1_print.out

cat testscript/tmp1_print.out
