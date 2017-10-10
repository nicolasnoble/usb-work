#!/bin/sh

fromdos $1
clang-format-3.5 -style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 180, IndentCaseLabels: false}" $1 > $1.2
mv $1.2 $1
