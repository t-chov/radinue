#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 1 || ! -d "$1/Contents/PlugIns" ]]; then
    echo "Usage: $0 /path/to/Radinue.app" >&2
    exit 1
fi

app_bundle=$1
frameworks_directory="$app_bundle/Contents/Frameworks"
plugins_directory="$app_bundle/Contents/PlugIns"

while IFS= read -r -d '' plugin; do
    while IFS= read -r dependency; do
        relative_path=${dependency#@rpath/}
        if [[ ! -e "$frameworks_directory/$relative_path" ]]; then
            echo "Cannot resolve $dependency required by $plugin" >&2
            exit 1
        fi

        install_name_tool \
            -change "$dependency" "@executable_path/../Frameworks/$relative_path" \
            "$plugin"
    done < <(otool -L "$plugin" | awk '$1 ~ /^@rpath\// { print $1 }')
done < <(find "$plugins_directory" -type f -name '*.dylib' -print0)

unresolved_dependencies=$(
    find "$plugins_directory" -type f -name '*.dylib' -exec otool -L {} \; |
        awk '$1 ~ /^@rpath\// { print $1 }'
)
if [[ -n "$unresolved_dependencies" ]]; then
    echo "Qt plugins still contain unresolved @rpath dependencies:" >&2
    echo "$unresolved_dependencies" >&2
    exit 1
fi
