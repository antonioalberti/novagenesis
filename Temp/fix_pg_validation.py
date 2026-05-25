import sys

path = "/home/gandalf/workspace/novagenesis/PGCS/src/PG.cpp"
with open(path, "r") as f:
    lines = f.readlines()

# Line 565 (0-indexed: 564) is the target
idx = 564

# Verify we're at the right place
assert "GetNumberofCommandLines" in lines[idx], f"Expected GetNumberofCommandLines at line {idx+1}, got: {lines[idx].strip()}"
assert "HeaderSizeField" in lines[idx+2], f"Expected HeaderSizeField at line {idx+3}, got: {lines[idx+2].strip()}"

# Get the indentation from the surrounding lines
indent = "\t\t\t\t\t  "  # 6 tabs + 6 spaces, matching the surrounding code

validation_code = indent + "// Validate: every command line must have >=1 argument for correct\n"
validation_code += indent + "// serialization round-trip through raw socket.\n"
validation_code += indent + "// ConvertCommandLineFromCharArray() requires [ ] markers and\n"
validation_code += indent + "// WhiteSpaceCounter > 3, which are only present when NoA > 0.\n"
validation_code += indent + "for (unsigned int cl = 0; cl < NoCL; cl++) {\n"
validation_code += indent + "    CommandLine *PCL = 0;\n"
validation_code += indent + "    if (M->GetCommandLine(cl, PCL) == OK) {\n"
validation_code += indent + "        unsigned int numArgs = 0;\n"
validation_code += indent + "        PCL->GetNumberofArguments(numArgs);\n"
validation_code += indent + "        if (numArgs == 0) {\n"
validation_code += indent + "            S << Offset << \"(ERROR: Command line \" << cl << \" ('\"\n"
validation_code += indent + "              << PCL->Name << \" \" << PCL->Alternative\n"
validation_code += indent + "              << \"') has 0 arguments. All inter-host command lines \"\n"
validation_code += indent + "              << \"must have >=1 argument for correct serialization \"\n"
validation_code += indent + "              << \"round-trip through raw socket. Add at least one \"\n"
validation_code += indent + "              << \"argument (e.g., a counter) to fix this.)\" << endl;\n"
validation_code += indent + "            Status = ERROR;\n"
validation_code += indent + "            break;\n"
validation_code += indent + "        }\n"
validation_code += indent + "    }\n"
validation_code += indent + "}\n"
validation_code += indent + "if (Status == ERROR) {\n"
validation_code += indent + "    S << Offset << \"(ABORTING: message not sent due to 0-argument command line)\" << endl;\n"
validation_code += indent + "    return Status;\n"
validation_code += indent + "}\n"
validation_code += "\n"

# Insert after line 565 (before the empty line 566 and HeaderSizeField)
lines.insert(idx + 1, validation_code)

with open(path, "w") as f:
    f.writelines(lines)

print(f"Inserted validation code after line {idx+1}")
print(f"File now has {len(lines)} lines")
