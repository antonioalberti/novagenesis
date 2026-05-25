import re

path = "/home/gandalf/workspace/novagenesis/PGCS/src/PGRunPeriodic01.cpp"
with open(path, "r") as f:
    content = f.read()

# Fix 1: RunPeriodic (line ~125) - add counter argument
content = content.replace(
    '\tRunPeriodic->NewCommandLine ("-run", "--periodic", "0.1", PCL);\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunPeriodic, SCN);',
    '\tRunPeriodic->NewCommandLine ("-run", "--periodic", "0.1", PCL);\n\tPCL->NewArgument (1);\n\tPCL->SetArgumentElement (0, 0, PB->IntToString (PeriodicCounter));\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunPeriodic, SCN);'
)

# Fix 2: RunHello01 (line ~348) - add counter argument
content = content.replace(
    '\t  RunHello01->NewCommandLine ("-run", "--hello", "0.1", PCL);\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunHello01, SCN);',
    '\t  RunHello01->NewCommandLine ("-run", "--hello", "0.1", PCL);\n\t  PCL->NewArgument (1);\n\t  PCL->SetArgumentElement (0, 0, PB->IntToString (HelloCounter));\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunHello01, SCN);'
)

# Fix 3: RunHello02 (line ~381) - add counter argument
content = content.replace(
    '\t  RunHello02->NewCommandLine ("-run", "--hello", "0.2", PCL);\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunHello02, SCN);',
    '\t  RunHello02->NewCommandLine ("-run", "--hello", "0.2", PCL);\n\t  PCL->NewArgument (1);\n\t  PCL->SetArgumentElement (0, 0, PB->IntToString (HelloCounter));\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunHello02, SCN);'
)

# Fix 4: RunHello03 (line ~413) - add counter argument
content = content.replace(
    '\t  RunHello03->NewCommandLine ("-run", "--hello", "0.3", PCL);\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunHello03, SCN);',
    '\t  RunHello03->NewCommandLine ("-run", "--hello", "0.3", PCL);\n\t  PCL->NewArgument (1);\n\t  PCL->SetArgumentElement (0, 0, PB->IntToString (HelloCounter));\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunHello03, SCN);'
)

# Fix 5: RunPublishing (line ~539) - add counter argument
content = content.replace(
    '\t  RunPublishing->NewCommandLine ("-run", "--publishing", "0.1", PCL);\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunPublishing, SCN);',
    '\t  RunPublishing->NewCommandLine ("-run", "--publishing", "0.1", PCL);\n\t  PCL->NewArgument (1);\n\t  PCL->SetArgumentElement (0, 0, PB->IntToString (PublishingCounter));\n\n\t  // Generate the SCN\n\t  PB->GenerateSCNFromMessageBinaryPatterns (RunPublishing, SCN);'
)

# Fix 6: RunStresstest (line ~618) - add counter argument
content = content.replace(
    '      RunStresstest->NewCommandLine("-run", "--stresstest", "0.1", PCL);\n',
    '      RunStresstest->NewCommandLine("-run", "--stresstest", "0.1", PCL);\n      PCL->NewArgument(1);\n      PCL->SetArgumentElement(0, 0, PB->IntToString(StressCounter));\n'
)

with open(path, "w") as f:
    f.write(content)

print("All fixes applied")

# Verify
with open(path, "r") as f:
    content = f.read()

# Check that all NewArgument calls are present
count = content.count('PCL->NewArgument')
print(f"Total NewArgument calls: {count}")

# Check specific patterns
for pattern in ['--periodic', '--hello', '--exposition', '--publishing', '--stresstest']:
    idx = content.find(pattern)
    if idx >= 0:
        # Check if followed by NewArgument within 200 chars
        snippet = content[idx:idx+300]
        has_arg = 'NewArgument' in snippet
        print(f"  {pattern}: NewArgument={'YES' if has_arg else 'NO'}")
