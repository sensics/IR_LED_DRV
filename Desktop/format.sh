#!/bin/sh
(
cd $(dirname "$0")
clang-format -i -style=file *.h *.cpp
)
