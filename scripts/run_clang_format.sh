#!/bin/sh
find . -path ./3rd_party -prune -o \( -name '*.h' -or -name '*.cc' \) \
  -exec clang-format -i {} \;
