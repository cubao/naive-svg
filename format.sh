#!/bin/bash

# Checking if this project has a .clang-format style file at the root dir.
if ! [[ -e $PWD/.clang-format ]]; then
    exit 0
fi

files=$(git ls-files | grep -E '\.h$|\.cpp$|\.hpp$|\\.c$' | tr '\n' ' ')
for file in $files; do
    echo $file
    clang-format -i -style=file -fallback-style=none $PWD/$file
done
