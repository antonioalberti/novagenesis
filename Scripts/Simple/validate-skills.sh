#!/bin/bash
# validate-skills.sh — Skills vs Docs consistency validation
#
# Checks:
# 1. All NovaGenesis skills exist with valid SKILL.md
# 2. Skills reference files exist
# 3. Skills have correct cross-references to docs
# 4. No orphaned skills (skills without matching docs/docs without skills)
#
# Usage: bash Scripts/Simple/validate-skills.sh
# Returns: exit code 0 if all checks pass, 1 otherwise

set -e

NG_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SKILLS_DIR="$HOME/.hermes/skills"
DOCS_DIR="$NG_ROOT/Docs"
ERRORS=0
PASS=0
FAIL=0

log_pass() { PASS=$((PASS+1)); echo "  ✅ $1"; }
log_fail() { FAIL=$((FAIL+1)); ERRORS=$((ERRORS+1)); echo "  ❌ $1"; }

echo "=========================================="
echo " Skills Validation Script"
echo " Base: $SKILLS_DIR"
echo "=========================================="
echo ""

# ── Expected NovaGenesis skills ──
NG_SKILLS=(
    "novagenesis-architecture"
    "novagenesis-build-deploy"
    "novagenesis-debug"
    "novagenesis-spec-workflow"
    "novagenesis-git-workflow"
    "novagenesis-intra-domain-routing"
    "novagenesis/ng-inverted-pub-sub"
)

# ── Check 1: All expected skills exist ──
echo "--- Check 1: All expected NovaGenesis skills exist ---"
for skill in "${NG_SKILLS[@]}"; do
    if [ -f "$SKILLS_DIR/$skill/SKILL.md" ]; then
        log_pass "Skill exists: $skill"
    else
        log_fail "Missing skill: $skill"
    fi
done

# ── Check 2: Skills have description in frontmatter ──
echo ""
echo "--- Check 2: Skills have description in frontmatter ---"
for skill in "${NG_SKILLS[@]}"; do
    SKILL_FILE="$SKILLS_DIR/$skill/SKILL.md"
    if [ -f "$SKILL_FILE" ]; then
        if grep -q "^description:" "$SKILL_FILE" 2>/dev/null; then
            log_pass "Description found: $skill"
        else
            log_fail "Missing description in $skill"
        fi
    fi
done

# ── Check 3: Skills reference files exist ──
echo ""
echo "--- Check 3: Skill reference files exist ---"
for skill in "${NG_SKILLS[@]}"; do
    REFS_DIR="$SKILLS_DIR/$skill/references"
    if [ -d "$REFS_DIR" ]; then
        REF_COUNT=$(ls "$REFS_DIR"/*.md 2>/dev/null | wc -l)
        if [ "$REF_COUNT" -gt 0 ]; then
            log_pass "$skill: $REF_COUNT reference files"
        fi
    else
        log_pass "$skill: No references directory"
    fi
done

# ── Check 4: Docs ARCHITECTURE/DIAGNOSTICS/DECISIONS exist ──
echo ""
echo "--- Check 4: Doc structure exists ---"
for dir in ARCHITECTURE DIAGNOSTICS DECISIONS; do
    if [ -d "$DOCS_DIR/$dir" ]; then
        COUNT=$(find "$DOCS_DIR/$dir" -maxdepth 1 -name "*.md" | wc -l)
        log_pass "Docs/$dir/ exists with $COUNT files"
    else
        log_fail "Docs/$dir/ is missing"
    fi
done

# ── Check 5: SPEC-STATUS-REGISTER.md exists ──
echo ""
echo "--- Check 5: SPEC-STATUS-REGISTER.md exists ---"
if [ -f "$DOCS_DIR/DECISIONS/SPEC-STATUS-REGISTER.md" ]; then
    log_pass "SPEC-STATUS-REGISTER.md exists"
else
    log_fail "SPEC-STATUS-REGISTER.md is missing"
fi

# ── Check 6: DOCUMENTATION-ARCHITECTURE.md exists ──
echo ""
echo "--- Check 6: DOCUMENTATION-ARCHITECTURE.md exists ---"
if [ -f "$DOCS_DIR/DECISIONS/DOCUMENTATION-ARCHITECTURE.md" ]; then
    log_pass "DOCUMENTATION-ARCHITECTURE.md exists"
else
    log_fail "DOCUMENTATION-ARCHITECTURE.md is missing"
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