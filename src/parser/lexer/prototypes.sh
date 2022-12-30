#!/bin/sh
sed -n '/^[a-zA-Z0-9]\+[[:space:]]*[a-zA-Z0-9]*[[:space:]]*[a-zA-Z0-9*_]\+(.\+)$/p' "$1" | sed 's/$/;/'
