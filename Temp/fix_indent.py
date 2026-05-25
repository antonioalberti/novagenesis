path = "/home/gandalf/workspace/novagenesis/PGCS/src/PG.cpp"
with open(path, "r") as f:
    lines = f.readlines()

# Find the problematic section and fix indentation
# Line 562 (0-indexed 561) should have proper indentation
# The issue is that lines 562-582 lost one level of tab indentation

# Find the start of the problematic block
start = None
for i, line in enumerate(lines):
    if 'if (M->GetMessageSize (MessageSize) == OK)' in line and i > 550:
        start = i
        break

print(f"Found GetMessageSize at line {start+1}: {repr(lines[start])}")

# Find the end of the problematic block - look for "delete[] HeaderSizeField"
end = None
for i in range(start, min(start+50, len(lines))):
    if 'delete[] HeaderSizeField' in lines[i]:
        end = i
        break

print(f"Found delete[] HeaderSizeField at line {end+1}: {repr(lines[end])}")

# Show the problematic section
print("\n--- Current lines ---")
for i in range(start, min(end+3, len(lines))):
    print(f"  {i+1}: {repr(lines[i])}")

# The fix: lines 562-582 need one more tab at the beginning
# Line 561 (if GetMessageSize) has 4 tabs - correct
# Line 562 (if MessageSize > 0) has 2 tabs - should have 5 tabs
# Lines 563-582 have mixed indentation - should all have 6 tabs for content, 5 for braces

# Let's fix by replacing the entire block
# First, find the exact line numbers
block_start = start + 1  # line after GetMessageSize (0-indexed)
block_end = end  # line of delete[] HeaderSizeField (0-indexed)

print(f"\nBlock to fix: lines {block_start+1} to {block_end+1}")

# Build the corrected block
corrected = []
corrected.append('\t\t\t\tif (MessageSize > 0)\n')
corrected.append('\t\t\t\t\t{\n')
corrected.append('\t\t\t\t\t\tM->GetNumberofCommandLines (NoCL);\n')
corrected.append('\n')
corrected.append('\t\t\t\t\t\t// Warn if any command line has 0 arguments — such command lines\n')
corrected.append('\t\t\t\t\t\t// cannot be re-parsed by ConvertCommandLineFromCharArray() at the\n')
corrected.append('\t\t\t\t\t\t// receiving end (requires [ ] markers and WhiteSpaceCounter > 3).\n')
corrected.append('\t\t\t\t\t\t// This is a non-fatal warning; the message is still sent.\n')
corrected.append('\t\t\t\t\t\tfor (unsigned int cl = 0; cl < NoCL; cl++) {\n')
corrected.append('\t\t\t\t\t\t\tCommandLine *PCL = 0;\n')
corrected.append('\t\t\t\t\t\t\tif (M->GetCommandLine(cl, PCL) == OK) {\n')
corrected.append('\t\t\t\t\t\t\t\tunsigned int numArgs = 0;\n')
corrected.append('\t\t\t\t\t\t\t\tPCL->GetNumberofArguments(numArgs);\n')
corrected.append('\t\t\t\t\t\t\t\tif (numArgs == 0) {\n')
corrected.append('\t\t\t\t\t\t\t\t\tS << Offset << "(WARNING: Command line " << cl << " (\'"\n')
corrected.append('\t\t\t\t\t\t\t\t\t  << PCL->Name << " " << PCL->Alternative\n')
corrected.append('\t\t\t\t\t\t\t\t\t  << "\') has 0 arguments — will fail to parse at receiver. "\n')
corrected.append('\t\t\t\t\t\t\t\t\t  << "Add ≥1 argument for correct serialization round-trip.)" << endl;\n')
corrected.append('\t\t\t\t\t\t\t\t}\n')
corrected.append('\t\t\t\t\t\t\t}\n')
corrected.append('\t\t\t\t\t\t}\n')
corrected.append('\n')
corrected.append('\t\t\t\t\t\tunsigned char *HeaderSizeField = new unsigned char[8];\n')

# Replace lines block_start through block_end-1 (keep delete[] HeaderSizeField and after)
new_lines = lines[:block_start] + corrected + lines[block_end:]

with open(path, "w") as f:
    f.writelines(new_lines)

print(f"Fixed! File now has {len(new_lines)} lines (was {len(lines)})")

# Verify
with open(path, "r") as f:
    lines = f.readlines()

print("\n--- Fixed section ---")
for i in range(start, min(start+30, len(lines))):
    print(f"  {i+1}: {repr(lines[i])}")
