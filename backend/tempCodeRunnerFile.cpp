vector<string> checkPythonIndentation(const string& code) {
    vector<string> errors;
    const int baseIndent = 4;
    int expectedIndent = 0;
    vector<int> indentLevels;  // Change from stack to vector for easier searching
    indentLevels.push_back(0); // Start with base level indentation
    
    istringstream iss(code);
    string line;
    int lineNumber = 0;

    while (getline(iss, line)) {
        lineNumber++;
        
        // Skip empty lines or lines with only whitespace
        if (line.find_first_not_of(" \t") == string::npos) {
            continue;
        }

        // Count leading spaces
        int currentIndent = 0;
        for (char c : line) {
            if (c == ' ') currentIndent++;
            else if (c == '\t') currentIndent += 4; // Convert tab to spaces
            else break;
        }

        // Get the content of the line (strip leading whitespace)
        string content = line.substr(line.find_first_not_of(" \t"));
        if (content.empty()) continue;

        // Handle dedent
        while (!indentLevels.empty() && currentIndent < indentLevels.back()) {
            indentLevels.pop_back();
        }

        // Check if current indentation matches any valid level
        if (!indentLevels.empty() && currentIndent != indentLevels.back()) {
            if (currentIndent > indentLevels.back()) {
                // If indenting, must be exactly one level deeper
                if ((currentIndent - indentLevels.back()) != baseIndent) {
                    errors.push_back("Incorrect indentation at line " + to_string(lineNumber) + 
                                   " (expected " + to_string(indentLevels.back() + baseIndent) + 
                                   " spaces, found " + to_string(currentIndent) + " spaces)");
                }
            } else {
                // When dedenting, must match a previous indentation level
                if (find(indentLevels.begin(), indentLevels.end(), currentIndent) == indentLevels.end()) {
                    errors.push_back("Incorrect dedent at line " + to_string(lineNumber));
                }
            }
        }

        // Handle indent after colon
        if (!content.empty() && content.back() == ':') {
            indentLevels.push_back(currentIndent + baseIndent);
        } else if (currentIndent > (indentLevels.empty() ? 0 : indentLevels.back())) {
            indentLevels.push_back(currentIndent);
        }
    }

    return errors;
}