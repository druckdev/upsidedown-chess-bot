#!/bin/bash

# All groups should create a repository under git.tu-berlin.de owned by a group
# called "sose21-ki-projekt-{chess,taiji}-[A-V]" with the name "group-[A-V]".
# This repository should contain an executable file called "start.sh" that
# launches the AI. (See template_start.sh)

URL="git@git.tu-berlin.de"
REPO_OWNER_BASE="sose21-ki-projekt"
REPO_NAME="group"
START_SCRIPT="start.sh"
AI_BRIDGE_DIR="$(dirname $1)/gameserver/pjki21-gs"
AI_BRIDGE_EXEC="ai_bridge.js"

showUsage() {
	printf "Usage: $1 -g <game> -[ph]\n" >&2
}

chr() {
	printf \\$(printf "%03o" "$1")
}

if (( ! $# )); then
	showUsage
	exit 1
fi

ALREADY_PULLED=0

while getopts "g:ph" opt; do
	case $opt in
		g) GAME="$OPTARG";;
		p) ALREADY_PULLED=1;;
		h|*)
			showUsage
			exit 1;
			;;
	esac
done

AI_BRIDGE_DIR=$(realpath "$AI_BRIDGE_DIR-$GAME")

# pushd "$(dirname $AI_BRIDGE)" &>/dev/null
# npm i
# npm start &
# popd &>/dev/null

# Make sure that we have a fresh state
printf "Checking fresh state..."
REPOS_DIR="$GAME-repos"

if (( $ALREADY_PULLED )); then
	[[ -d "$REPOS_DIR" ]] || exit 1
	printf " SKIP\n"
else
	rmdir "$REPOS_DIR" &>/dev/null || true
	mkdir "$REPOS_DIR" || exit 1
	printf " OK\n"
fi

pushd "$REPOS_DIR" &>/dev/null
REPOS_DIR="$PWD"

printf "Cloning players repos..."
if (( ! ALREADY_PULLED )); then
	BASE="$URL:$REPO_OWNER_BASE-$GAME"
	for group in {A..V}; do
		git clone --depth 1 "$BASE-$group/$REPO_NAME-$group" &>/dev/null || continue
	done
	printf " DONE\n"
else
	printf " SKIP\n"
fi

# Start the everybody vs everybody
printf "Starting games...\n"
# Loop from ascii 'A' to acii 'U'
for player0_num in {65..85}; do
	# Convert player0 to letter
	player0="$(chr "$player0_num")"

	player0_script="$REPO_NAME-$player0/$START_SCRIPT"
	[[ -e "$player0_script" ]] || continue
	# Fix permissions
	chmod u+x "$player0_script"

	# Loop from player0 to V (in ascii)
	for player1 in $(seq "$player0_num" 86); do
		# Convert player1 to letter
		player1="$(chr "$player1")"

		[[ $player0 != $player1 ]] || continue
		player1_script="$REPO_NAME-$player1/$START_SCRIPT"
		[[ -e "$player1_script" ]] || continue
		# Fix permissions
		chmod u+x "$player1_script"

		printf "$player0 vs $player1\n"

		printf "Running server...\n"
		node -r esm ../../gameserver/pjki21-gs-chess/src/server.js &
		PROC_ID=$!

		printf "Playing...\n"
		pushd "$AI_BRIDGE_DIR" &>/dev/null
		mkdir -p logs/"group_$player0"
		mkdir -p logs/"group_$player1"
		node -r esm "$AI_BRIDGE_EXEC" localhost:8000 "$REPOS_DIR/$player0_script" w 90.0 100 > logs/"group_$player0"/log_"$player0"_vs_"$player1"_as_white.txt 2> logs/"group_$player0"/error_"$player0"_vs_"$player1"_as_white.txt &
		PROC_BRIDGE1=$!

		node -r esm "$AI_BRIDGE_EXEC" localhost:8000 "$REPOS_DIR/$player1_script" b 90.0 100 > logs/"group_$player1"/log_"$player0"_vs_"$player1"_as_black.txt 2> logs/"group_$player1"/error_"$player0"_vs_"$player1"_as_black.txt &
		PROC_BRIDGE2=$!

		popd &>/dev/null

		# wait while BOTH ai_bridges run
		while kill -0 "$PROC_BRIDGE1" >/dev/null 2>/dev/null && kill -0 "$PROC_BRIDGE2" >/dev/null 2>/dev/null; do
    		sleep 1
		done

		# kill the other one, if only one terminates
		kill $PROC_BRIDGE1 >/dev/null 2>/dev/null
		kill $PROC_BRIDGE2 >/dev/null 2>/dev/null

		printf "Killing server...\n\n"
		kill $PROC_ID
	done
done
