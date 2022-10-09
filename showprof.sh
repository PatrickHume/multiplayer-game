echo "Generating output profile graph..."
echo "(Warnings have been suppressed.)"
~/go/bin/pprof -web output.pprof 2> /dev/null