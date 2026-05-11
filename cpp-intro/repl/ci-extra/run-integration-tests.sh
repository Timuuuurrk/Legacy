#!/usr/bin/env bash
set -euo pipefail

PRESET_NAME=$1
python3 -m pytest integration-tests/tests --exe "build/${PRESET_NAME}/app/repl-app"
