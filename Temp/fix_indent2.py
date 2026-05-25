path = "/home/gandalf/workspace/novagenesis/PGCS/src/PG.cpp"
with open(path, "r") as f:
    lines = f.readlines()

# Find the problematic section
start = None
for i, line in enumerate(lines):
    if 'if (M->GetMessageSize (MessageSize) == OK)' in line and i > 550:
        start = i
        break

print(f"Found GetMessageSize at line {start+1}")

# Find the end - look for "HeaderSizeField" after the warning block
end = None
for i in range(start, min(start+40, len(lines))):
    if 'unsigned char *HeaderSizeField = new unsigned char[8]' in lines[i]:
        end = i
        break

print(f"Found HeaderSizeField at line {end+1}")

# Show current state
print("\n--- Current lines ---")
for i in range(start, min(end+3, len(lines))):
    print(f"  {i+1}: {repr(lines[i])}")

# Replace lines start+1 through end-1 with corrected block
# Keep line start (GetMessageSize) and line end (HeaderSizeField) in place
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
corrected.append('\t\t\t\t\t\t\t\t\t  << "Add >=1 argument for correct serialization round-trip.)" << endl;\n')
corrected.append('\t\t\t\t\t\t\t\t}\n')
corrected.append('\t\t\t\t\t\t\t}\n')
corrected.append('\t\t\t\t\t\t}\n')
corrected.append('\n')

# Replace: keep lines[0:start+1], then corrected, then lines[end:]
new_lines = lines[:start+1] + corrected + lines[end:]

with open(path, "w") as f:
    f.writelines(new_lines)

print(f"\nFixed! File now has {len(new_lines)} lines (was {len(lines)})")

# Verify
with open(path, "r") as f:
    vlines = f.readlines()
print("\n--- Fixed section ---")
for i in range(start, min(start+35, len(vlines))):
    print(f"  {i+1}: {repr(vlines[i])}")
