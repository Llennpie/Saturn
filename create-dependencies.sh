#!/bin/bash

# separate with colon, prefix with REDIR to redirect download destination to a separate directory
DIRS="dynos"
EXTRA_FILES="fonts/forkawesome-webfont.ttf"
EXTRA_FILES_WINDOWS="lib/discordsdk/discord_game_sdk.dll"
EXTRA_FILES_LINUX="lib/discordsdk/libdiscord_game_sdk.so:res/saturn-linuxicon.png"
REDIRECTS="lib/discordsdk;."

REPO_OWNER="Llennpie"
REPO_NAME="Saturn"
REPO_BRANCH="legacy"

DEPENDENCY_FILE="dependencies.txt"

# generate os dependency list
create_dependency_list() {
    IFS=':' read -ra files <<< "$1"
    for file in "${files[@]}"; do
        echo $file
        echo "$2 https://raw.githubusercontent.com/$REPO_OWNER/$REPO_NAME/$REPO_BRANCH/$file" >> $DEPENDENCY_FILE
    done
}

# generate redirect list
create_redirect_list() {
    IFS=':' read -ra redirs <<< "$1"
    for redir in "${redirs[@]}"; do
        echo "> $redir" >> $DEPENDENCY_FILE
    done
}

# searchese through the input directories
recursive_search() {
    IFS=':' read -ra dirs <<< "$1"
    output=""
    for dir in "${dirs[@]}"; do
        files="$(find "$dir" -type f -printf "%p:")"
        [ -n "$files" ] && output+="$files"
    done
    if [ -n "$output" ]; then
        output="${output%:}"
    fi
    echo $output
}

# clear the original file
rm $DEPENDENCY_FILE
touch $DEPENDENCY_FILE

# append comments
echo "# Links to files that Saturn updater downloads during installation" >> $DEPENDENCY_FILE
echo "# Prefix lines with # for comments" >> $DEPENDENCY_FILE
echo "# Prefix lines with $ for Windows only files" >> $DEPENDENCY_FILE
echo "# Prefix lines with % for Linux only files" >> $DEPENDENCY_FILE
echo "# Prefix lines with @ for files for both operating systems" >> $DEPENDENCY_FILE
echo "# Prefix lines with > to redirect downloaded file to a separate directory" >> $DEPENDENCY_FILE
echo "# Use ./generate-dependencies.sh to create this dependency list" >> $DEPENDENCY_FILE
echo "" >> $DEPENDENCY_FILE

# generate the dependency list
create_dependency_list $EXTRA_FILES_WINDOWS "$"
create_dependency_list $EXTRA_FILES_LINUX "%"
create_dependency_list $EXTRA_FILES "@"
create_dependency_list "$(recursive_search $DIRS)" "@"

create_redirect_list $REDIRECTS
