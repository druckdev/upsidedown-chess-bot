#!/usr/bin/env bash

FORMAT_CMD="clang-format-11 --style=file -i"

cd "$(realpath "$(dirname "$0")")"

if [[ $1 == --all || $1 == -a ]]; then
	find . \
		-name '.git' -prune -o \
		-name 'build' -prune -o \
		\( -name "*.c" -o -name "*.h" \) -a \
		-exec $FORMAT_CMD '{}' ';'
else
	git diff --name-only --diff-filter=AMT HEAD |
		grep -E "\.[ch]$" |
		xargs -I '{}' $FORMAT_CMD '{}'
fi
