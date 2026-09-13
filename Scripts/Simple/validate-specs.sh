#!/bin/bash
# validate-specs.sh — SPEC consistency validation
# 
# Checks:
# 1. No stale AIOPT2 branch references
# 2. No dangling SPEC-015/016 references
# 3. All Related SPECs exist
# 4. Status values are valid
# 5. No duplicate SPEC numbers
# 6. All SPECs have Branch: AIOPT3
#
# Usage: bash Scripts/Simple/validate-specs.sh
# Returns: exit code 0 if all checks pass, 1 otherwise

set -e

NG_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SPECS_DIR="$NG_ROOT/Specs"
ERRORS=0
PASS=0
FAIL=0

log_pass() { PASS=$((PASS+1)); echo "  ✅ $1"; }
log_fail() { FAIL=$((FAIL+1)); ERRORS=$((ERRORS+1)); echo "  ❌ $1"; }

echo "=========================================="
echo " SPEC Validation Script"
echo " Base: $SPECS_DIR"
echo "=========================================="
echo ""

# ── Check 1: No AIOPT2 references ──
echo "--- Check 1: No stale AIOPT2 references ---"
AIOPT2_FILES=$(grep -rl "AIOPT2" "$SPECS_DIR" 2>/dev/null || true)
if [ -z "$AIOPT2_FILES" ]; then
    log_pass "No AIOPT2 references found."
else
    for f in $AIOPT2_FILES; do
        BASENAME=$(basename "$f")
        LINES=$(grep -n "AIOPT2" "$f" | cut -d: -f1 | tr '\n' ',')
        log_fail "AIOPT2 found in $BASENAME (lines: $LINES)"
    done
fi

# ── Check 2: No dangling SPEC-015/016 references ──
echo ""
echo "--- Check 2: No dangling SPEC-015/016 references ---"
DANGLING=$(grep -rl "SPEC-01[56]" "$SPECS_DIR" 2>/dev/null || true)
if [ -z "$DANGLING" ]; then
    log_pass "No dangling SPEC-015/016 references."
else
    for f in $DANGLING; do
        BASENAME=$(basename "$f")
        log_fail "SPEC-015/016 reference found in $BASENAME"
    done
fi

# ── Check 3: All Related SPECs exist ──
echo ""
echo "--- Check 3: All Related SPEC references exist ---"
# Extract all SPEC-NNN references from Related: lines
RELATED_REFS=$(grep -r "^[*]*Related:" "$SPECS_DIR" 2>/dev/null | grep -oP "SPEC-\d{3}" | sort -u || true)
MISSING_REFS=0
for ref in $RELATED_REFS; do
    # Check if file with this SPEC number exists
    FOUND=$(ls "$SPECS_DIR/$ref"*.md 2>/dev/null | wc -l)
    if [ "$FOUND" -eq 0 ]; then
        log_fail "Related SPEC $ref has no corresponding file."
        MISSING_REFS=$((MISSING_REFS+1))
    fi
done
if [ "$MISSING_REFS" -eq 0 ]; then
    log_pass "All Related SPEC references have corresponding files."
fi

# ── Check 4: Valid status values ──
echo ""
echo "--- Check 4: Valid status values ---"
VALID_STATUSES="Draft|Proposal|In Progress|Implemented|Implemented — Pending Test|Superseded|Abandoned"
INVALID=$(grep -r "^[*]*Status:" "$SPECS_DIR" 2>/dev/null | grep -vP "$VALID_STATUSES" || true)
if [ -z "$INVALID" ]; then
    log_pass "All SPECs have valid status values."
else
    while IFS= read -r line; do
        BASENAME=$(echo "$line" | cut -d: -f1 | xargs basename)
        STATUS=$(echo "$line" | grep -oP "Status:\s*.*" || echo "Status: <missing>")
        log_fail "Invalid status in $BASENAME: $STATUS"
    done <<< "$INVALID"
fi

# ── Check 5: No duplicate SPEC numbers ──
echo ""
echo "--- Check 5: No duplicate SPEC numbers ---"
DUPS=$(ls "$SPECS_DIR"/SPEC-*.md 2>/dev/null | sed 's/.*SPEC-\([0-9]*\).*/\1/' | sort | uniq -d)
if [ -z "$DUPS" ]; then
    log_pass "No duplicate SPEC numbers."
else
    for num in $DUPS; do
        FILES=$(ls "$SPECS_DIR"/SPEC-${num}*.md | xargs -I{} basename {} | tr '\n' ' ')
        log_fail "Duplicate SPEC-${num}: $FILES"
    done
fi

# ── Check 6: Branch field consistency ──
echo ""
echo "--- Check 6: Branch field consistency ---"
BRANCH_ISSUES=$(grep -r "^[*]*Branch:" "$SPECS_DIR" 2>/dev/null | grep -v "AIOPT3" || true)
if [ -z "$BRANCH_ISSUES" ]; then
    log_pass "All SPECs have Branch: AIOPT3 (or no Branch field)."
else
    while IFS= read -r line; do
        BASENAME=$(echo "$line" | cut -d: -f1 | xargs basename)
        BRANCH=$(echo "$line" | grep -oP "Branch:\s*\S*")
        log_fail "Non-AIOPT3 branch in $BASENAME: $BRANCH"
    done <<< "$BRANCH_ISSUES"
fi

# ── Summary ──
echo ""
echo "=========================================="
echo " Results: $PASS passed, $FAIL failed"
echo "=========================================="

if [ "$ERRORS" -gt 0 ]; then
    exit 1
fi
exit 0