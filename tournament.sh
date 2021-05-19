#!/bin/sh

cleanup() {
	echo "trap SIG"
	echo "$BLACK, $WHITE, $TEMP"
	[[ $BLACK ]] || kill "$BLACK"
	[[ $WHITE ]] || kill "$WHITE"
	[[ $TEMP ]]  || command rm -rf "$TEMP"
}

trap cleanup SIGTERM
trap cleanup SIGINT

TEMP="$(mktemp -d)"
mkfifo "$TEMP/chess-"{1,2}

cd "$(realpath "$(dirname "$0")")"

mkdir -p build
cd build
cmake ..
make

printf "\n"

( stdbuf -o L ./bot 1 <"$TEMP/chess-2" | tee -a "$TEMP/chess-1" ) &
BLACK=$!
( stdbuf -o L ./bot 0 <"$TEMP/chess-1" | tee -a "$TEMP/chess-2" ) &
WHITE=$!

wait

cleanup
