#!/bin/bash
#
# This script logs the output of a command to a file and shows it if the
# command fails or a verbose flag is set, grouping it with a title if running
# in GitHub Actions.
#
# Author: Mohammad Rashiqul Alam
#

if [ $# -lt 3 ]; then
    echo "Usage: $0 <title> <logfile> <command>"
    exit 1
fi

title="$1"
shift
logfile="$1"
shift

# Setup unique temp directory for subprocesses
# https://www.mathworks.com/matlabcentral/answers/1728440-running-simulink-on-multiple-matlab-instances-concurrently-on-machine-crashes-in-sldd-dmr-sdi-jen
TMPDIR=$(mktemp -d /tmp/mbd-comp.$$.XXXXXXXXXX)
export TMPDIR

# Avoid multiple MATLAB processes to collide when using preference dir:
# https://www.mathworks.com/matlabcentral/answers/93696-how-do-i-change-the-matlab-preferences-directory-location
MATLAB_PREFDIR=$(mktemp -d /tmp/matlab.$$.XXXXXXXXXX)
export MATLAB_PREFDIR

# Save Crash Logs to TMPDIR
export MATLAB_LOG_DIR=$TMPDIR

# Set Cache Dir
export MCR_CACHE_ROOT=$TMPDIR

trap 'rm -rf "$TMPDIR" "$MATLAB_PREFDIR"' EXIT

echo -n "" > "$logfile"
"$@" &>> "$logfile"
exit_code=$?

if [ $exit_code != 0 ] || [ "$SWC_MBD_VERBOSE_BUILD" ]; then
    echo "::group::$title";
    cat "$logfile";
    cat "$TMPDIR"/matlab_crash* || : # Print CrashLogs if Found
    echo "::endgroup::";
fi

exit $exit_code
