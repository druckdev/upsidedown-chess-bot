#!/bin/sh

cd "$(dirname "$0")" || exit

# $1 player is w or b
# $2 totalTime as double
# $3 maxMoves as int

./build/bot $1 $2 $3
