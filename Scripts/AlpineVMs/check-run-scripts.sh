#!/bin/bash
# run_*.sh syntax check
# Run this script after any edits to verify script syntax

ERRORS=0
DIR=<workspace-root>/novagenesis/Scripts/AlpineVMs

echo "=== Syntax check for all run_ scripts ==="
for script in run_PGCS_on_Source_VM.sh run_PGCS_on_Repo_VM.sh \
               run_NRNCS_on_Source_VM.sh \
               run_Source_on_Source_VM.sh run_Repository_on_Repo_VM.sh; do
  if bash -n "$DIR/$script" 2>&1; then
    echo "  PASS: $script"
  else
    echo "  FAIL: $script"
    ERRORS=$((ERRORS + 1))
  fi
done

echo ""
echo "Errors: $ERRORS"
exit $ERRORS