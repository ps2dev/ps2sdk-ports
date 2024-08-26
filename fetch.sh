#!/bin/bash
set -e
REPO_REF="$1"
REPO_URL="$2"
FETCH_ALL="$3"
REPO_FOLDER=$(basename "$REPO_URL")
REPO_FOLDER=build/${REPO_FOLDER%.*}
echo "Fetching '$REPO_FOLDER' version '$REPO_REF'..."
if [ ! -d "$REPO_FOLDER" ]; then
    git init "$REPO_FOLDER"
    git -C "$REPO_FOLDER" remote add origin "$REPO_URL"
else
    git -C "$REPO_FOLDER" remote set-url origin "$REPO_URL"
fi
if [[ "$FETCH_ALL" == true ]]; then
    git -C "$REPO_FOLDER" fetch --unshallow || true
    git -C "$REPO_FOLDER" fetch origin "$REPO_REF" --tags
else
    git -C "$REPO_FOLDER" fetch origin "$REPO_REF" --depth=1
fi
git -C "$REPO_FOLDER" checkout -f FETCH_HEAD
