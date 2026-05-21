#!/bin/bash
# ================================================================================
# monitor_logs.sh
# ================================================================================
# Monitors all log files from a test run in real-time with color-coded labels.
# Usage: bash monitor_logs.sh <log_directory>
# Example: bash monitor_logs.sh ../../IO/logs/run_20260503_093000
# ================================================================================

if [ -z "$1" ]; then
    echo "Usage: $0 <log_directory>"
    echo "Example: $0 ../../IO/logs/run_20260503_093000"
    exit 1
fi

LOGDIR="$1"

if [ ! -d "$LOGDIR" ]; then
    echo "Error: Directory not found: $LOGDIR"
    exit 1
fi

echo "============================================================"
echo "Monitoring logs in: $LOGDIR"
echo "============================================================"
echo "Press Ctrl+C to stop monitoring."
echo ""

# Color codes for labels
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Use tail -f with multiple files; tail adds ==> filename <== headers
# We enhance this with color-coded process names
tail -f "$LOGDIR"/*.log 2>/dev/null | while IFS= read -r line; do
    if [[ "$line" == "==>"*".log <==" ]]; then
        # Extract filename from tail's header
        filename=$(echo "$line" | sed 's/==> //;s/ <==//')
        basename=$(basename "$filename" .log)
        case "$basename" in
            PGCS)        label="${RED}[PGCS]${NC}" ;;
            NRNCS)       label="${GREEN}[NRNCS]${NC}" ;;
            Repository1) label="${YELLOW}[REPO]${NC}" ;;
            Source1)     label="${CYAN}[SOURCE]${NC}" ;;
            *)           label="${BLUE}[${basename}]${NC}" ;;
        esac
        echo ""
        echo -e "==> $label <=="
    else
        echo "$line"
    fi
done