#include <bits/stdc++.h>
using namespace std;

// Add this declaration at the start of the file, after includes and before any classes
void logError(const string& error);

// Add these new error types at the start after the includes
struct LexicalError {
    int line;
    int character;
    string token;
    string message;
};

// Enhanced TrieNode with frequency and language statistics
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    int frequency;
    set<string> languages;
    
    TrieNode() : isEndOfWord(false), frequency(0) {}
};

// Enhanced Trie with language-specific features
class EnhancedTrie {
    TrieNode* root;
    
public:
    EnhancedTrie() { root = new TrieNode(); }
    
    void insert(const string& word, const string& language) {
        TrieNode* current = root;
        for (char c : word) {
            if (!current->children[c])
                current->children[c] = new TrieNode();
            current = current->children[c];
        }
        current->isEndOfWord = true;
        current->frequency++;
        current->languages.insert(language);
    }
    
    pair<bool, set<string>> searchWithInfo(const string& word) {
        TrieNode* current = root;
        for (char c : word) {
            if (!current->children[c])
                return {false, set<string>()};
            current = current->children[c];
        }
        return {current->isEndOfWord, current->languages};
    }
};

// Lexical Analyzer Class
// Lexical Analyzer class - processes source code into tokens, supporting C++, Java, and Python syntax
class LexicalAnalyzer {
private:
    // Core data structures for storing tokens and their properties
    vector<string> tokens;                    // List of all processed tokens
    unordered_map<string, string> tokenTypes; // Mapping between tokens and their types
    set<string> operators;                    // Stores valid operator symbols
    set<string> separators;                   // Stores valid separator characters
    set<string> keywords;                     // Stores reserved keywords for supported languages

    // Collection of any lexical errors found during analysis
    vector<LexicalError> lexicalErrors;

    // Checks and processes different types of comments in the code
    bool isInComment(const string& code, size_t& i, vector<pair<string, string>>& lexicalUnits) {
        // Handle preprocessor directives first
        if (code[i] == '#') {
            string directive;
            size_t start = i;
            // Collect the entire preprocessor directive
            while (i < code.length() && code[i] != '\n') {
                directive += code[i];
                i++;
            }
            // Handle preprocessor directive
            if (directive.substr(0, 8) == "#include") {
                size_t angleStart = directive.find('<');
                size_t angleEnd = directive.find('>');
                if (angleStart != string::npos && angleEnd != string::npos) {
                    // Split into #include and header
                    lexicalUnits.push_back({"#include", "preprocessor"});
                    lexicalUnits.push_back({directive.substr(angleStart, angleEnd - angleStart + 1), "header"});
                } else {
                    lexicalUnits.push_back({directive, "preprocessor"});
                }
                return true;
            }
            // Reset if not a preprocessor directive
            i = start;
        }

        // Handle regular comments
        if (i + 1 >= code.length()) return false;
        
        // C++ style single-line comment
        if (code[i] == '/' && code[i + 1] == '/') {
            string comment = "//";
            i += 2; // Move past comment markers
            while (i < code.length() && code[i] != '\n') {
                comment += code[i++];
            }
            lexicalUnits.push_back({comment, "comment"});
            return true;
        }
        
        // Handle C++/Java multi-line comments
        if (code[i] == '/' && code[i + 1] == '*') {
            string comment = "/*";
            i += 2; // Move past comment markers
            while (i + 1 < code.length()) {
                if (code[i] == '*' && code[i + 1] == '/') {
                    comment += "*/";
                    i += 2;
                    lexicalUnits.push_back({comment, "comment"});
                    return true;
                }
                comment += code[i++];
            }
            // Auto-close any unclosed comments
            lexicalUnits.push_back({comment + "*/", "comment"});
            return true;
        }
        
        return false;
    }

    // Returns true if character is not printable and not whitespace
    bool isInvalidChar(char c) {
        return !isprint(c) && !isspace(c);
    }

    // Records a lexical error with position and description
    void addLexicalError(int line, int character, const string& token, const string& message) {
        lexicalErrors.push_back({line, character, token, message});
    }

public:
    // Initialize analyzer with supported language elements
    LexicalAnalyzer() {
        // Set up basic operators
        operators = {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||"};
        
        // Set up code separators
        separators = {";", ",", "(", ")", "{", "}", "[", "]", ".", ":"};
        
        // Set up language keywords
        keywords = {
            // C++ keywords
            "cout", "cin", "include", "namespace", "using", "class", "public", "private",
            "protected", "template", "typename", "const", "virtual", "friend",
            
            // Java keywords
            "System", "out", "println", "public", "static", "void", "extends", "implements",
            "interface", "abstract", "final", "synchronized",
            
            // Python keywords
            "def", "class", "if", "elif", "else", "while", "for", "try", "except",
            "finally", "with", "as", "lambda", "yield", "None", "True", "False"
        };
    }

    // Main analysis function - breaks code into tokens
    vector<pair<string, string>> analyzeLexically(const string& code) {
        vector<pair<string, string>> lexicalUnits;
        string currentToken;
        int currentLine = 1;
        int currentChar = 1;
        lexicalErrors.clear(); // Clear previous errors
        
        // Process each character in the code
        for (size_t i = 0; i < code.length(); i++) {
            char c = code[i];

            // Update position tracking
            if (c == '\n') {
                currentLine++;
                currentChar = 1;
            } else {
                currentChar++;
            }

            // Check for invalid characters
            if (isInvalidChar(c)) {
                addLexicalError(currentLine, currentChar, string(1, c), 
                              "Invalid character detected");
                continue;
            }

            // Check for invalid identifier naming
            if (!currentToken.empty() && currentToken[0] >= '0' && currentToken[0] <= '9') {
                if (isalpha(c)) {
                    addLexicalError(currentLine, currentChar - currentToken.length(), 
                                  currentToken + c, 
                                  "Variable name cannot start with a number");
                }
            }

            // Handle comments
            if (c == '/' || c == '#') {
                if (isInComment(code, i, lexicalUnits)) {
                    if (!currentToken.empty()) {
                        classifyAndAddToken(currentToken, lexicalUnits);
                        currentToken.clear();
                    }
                    continue;
                }
            }
            
            // Process whitespace and newlines
            if (isspace(c)) {
                if (!currentToken.empty()) {
                    classifyAndAddToken(currentToken, lexicalUnits);
                    currentToken.clear();
                }
                if (c == '\n') {
                    lexicalUnits.push_back({"NEWLINE", "separator"});
                }
                continue;
            }
            
            // Handle string literals
            if (c == '"' || c == '\'') {
                if (!currentToken.empty()) {
                    classifyAndAddToken(currentToken, lexicalUnits);
                    currentToken.clear();
                }
                string stringLiteral = extractStringLiteral(code, i, c);
                lexicalUnits.push_back({stringLiteral, "string_literal"});
                i += stringLiteral.length() + 1;
                continue;
            }
            
            // Process operators (single and double character)
            string possibleOp = string(1, c);
            if (i + 1 < code.length()) {
                string twoCharOp = possibleOp + code[i + 1];
                if (operators.find(twoCharOp) != operators.end()) {
                    if (!currentToken.empty()) {
                        classifyAndAddToken(currentToken, lexicalUnits);
                        currentToken.clear();
                    }
                    lexicalUnits.push_back({twoCharOp, "operator"});
                    i++;
                    continue;
                }
            }
            if (operators.find(possibleOp) != operators.end()) {
                if (!currentToken.empty()) {
                    classifyAndAddToken(currentToken, lexicalUnits);
                    currentToken.clear();
                }
                lexicalUnits.push_back({possibleOp, "operator"});
                continue;
            }
            
            // Process separators
            if (separators.find(string(1, c)) != separators.end()) {
                if (!currentToken.empty()) {
                    classifyAndAddToken(currentToken, lexicalUnits);
                    currentToken.clear();
                }
                lexicalUnits.push_back({string(1, c), "separator"});
                continue;
            }
            
            // Add character to current token
            currentToken += c;
        }
        
        // Process final token if exists
        if (!currentToken.empty()) {
            classifyAndAddToken(currentToken, lexicalUnits);
        }
        
        return lexicalUnits;
    }
//Done till here.......................................................................................................................................................................................................................................................................................................................................................................................................................................................................
    
    // Returns list of lexical errors found during analysis
    const vector<LexicalError>& getLexicalErrors() const {
        return lexicalErrors;
    }

private:
    // Classifies token type and adds to lexical units
    void classifyAndAddToken(const string& token, vector<pair<string, string>>& lexicalUnits) {
        // Check for invalid identifiers
        if (isdigit(token[0]) && any_of(token.begin(), token.end(), ::isalpha)) {
            lexicalUnits.push_back({token, "invalid_identifier"});
            addLexicalError(currentLine, currentChar - token.length(), 
                          token, "Invalid identifier: Cannot start with a number");
            return;
        }

        // Determine token type and add to units
        if (keywords.find(token) != keywords.end()) {
            lexicalUnits.push_back({token, "keyword"});
        }
        else if (isNumber(token)) {
            lexicalUnits.push_back({token, "numeric_literal"});
        }
        else if (isValidIdentifier(token)) {
            lexicalUnits.push_back({token, "identifier"});
        }
        else {
            lexicalUnits.push_back({token, "unknown"});
        }
    }
    
    // Extracts string literal content between quotes
    string extractStringLiteral(const string& code, size_t start, char quoteType) {
        string literal;
        size_t i = start + 1;
        while (i < code.length() && code[i] != quoteType) {
            if (code[i] == '\\' && i + 1 < code.length()) {
                literal += code[i + 1];
                i += 2;
            } else {
                literal += code[i];
                i++;
            }
        }
        return literal;
    }
    
    // Checks if token is a valid number
    bool isNumber(const string& token) {
        if (token.empty()) return false;
        char* end = nullptr;
        strtod(token.c_str(), &end);
        return end != token.c_str() && *end == '\0';
    }
    
    // Validates identifier naming rules
    bool isValidIdentifier(const string& token) {
        // Must start with letter or underscore
        if (token.empty() || (!isalpha(token[0]) && token[0] != '_')) {
            return false;
        }
        
        // Rest must be alphanumeric or underscore
        return all_of(token.begin() + 1, token.end(), 
                     [](char c) { return isalnum(c) || c == '_'; });
    }

    // Position tracking variables
    int currentLine = 1;
    int currentChar = 1;
};

// Language Detection System
class LanguageDetector {
    EnhancedTrie keywordTrie;
    unordered_map<string, int> languageFeatures;
    
public:
    LanguageDetector() {    
        initializeKeywords();
    }
    
    void initializeKeywords() {
        // C++ specific keywords and patterns
        vector<string> cppKeywords = {
            "cout", "cin", "include", "namespace", "using", "template", "typename",
            "virtual", "friend", "operator", "nullptr", "constexpr", "noexcept"
        };
        
        // Java specific keywords and patterns
        vector<string> javaKeywords = {
            "System.out.println", "public", "private", "protected", "interface",
            "implements", "extends", "super", "synchronized", "volatile", "transient"
        };
        
        // Python specific keywords and patterns
        vector<string> pythonKeywords = {
            "def", "lambda", "yield", "with", "as", "True", "False", "None",
            "self", "nonlocal", "global", "async", "await"
        };
        
        for (const auto& keyword : cppKeywords)
            keywordTrie.insert(keyword, "C++");
        for (const auto& keyword : javaKeywords)
            keywordTrie.insert(keyword, "Java");
        for (const auto& keyword : pythonKeywords)
            keywordTrie.insert(keyword, "Python");
    }


    string detectLanguage(const vector<pair<string, string>>& lexicalUnits) {
        unordered_map<string, int> scores = {{"C++", 0}, {"Java", 0}, {"Python", 0}};
        
        // Analyze each lexical unit
        for (const auto& unit : lexicalUnits) {
            const string& token = unit.first;
            const string& type = unit.second;
            
            // Check keywords
            auto [found, languages] = keywordTrie.searchWithInfo(token);
            if (found) {
                for (const string& lang : languages)
                    scores[lang] += 2;
            }
            
            // Language-specific patterns
            analyzeSyntaxPatterns(token, type, scores);
        }
        
        // Analyze overall code structure
        analyzeCodeStructure(lexicalUnits, scores);
        
        // Find language with highest score
        string detectedLang = "Unknown";
        int maxScore = 0;
        for (const auto& [lang, score] : scores) {
            if (score > maxScore) {
                maxScore = score;
                detectedLang = lang;
            }
        }
        
        return detectedLang;
    }

private:
    void analyzeSyntaxPatterns(const string& token, const string& type, 
                              unordered_map<string, int>& scores) {
        // C++ patterns
        if (token.find("::") != string::npos) scores["C++"] += 3;
        if (token.find("cout") != string::npos) scores["C++"] += 3;
        if (token.find("#include") != string::npos) scores["C++"] += 5;
        
        // Java patterns
        if (token.find("System.out.println") != string::npos) scores["Java"] += 5;
        if (token.find("public class") != string::npos) scores["Java"] += 4;
        if (token.find("extends") != string::npos) scores["Java"] += 2;
        
        // Python patterns
        if (token.find("def ") != string::npos) scores["Python"] += 4;
        if (token.find("import ") != string::npos) scores["Python"] += 2;
        if (token == "self") scores["Python"] += 3;
    }
    
    void analyzeCodeStructure(const vector<pair<string, string>>& units,
                            unordered_map<string, int>& scores) {
        bool hasSemicolons = false;
        bool hasSignificantWhitespace = false;
        bool hasBraces = false;
        
        for (const auto& unit : units) {
            if (unit.first == ";") hasSemicolons = true;
            if (unit.first == "{" || unit.first == "}") hasBraces = true;
            if (unit.second == "NEWLINE" && 
                units.begin() != units.end() &&
                unit != units.back() &&
                next(find(units.begin(), units.end(), unit))->second == "identifier") {
                hasSignificantWhitespace = true;
            }
        }
        
        if (hasSemicolons && hasBraces) {
            scores["C++"] += 3;
            scores["Java"] += 3;
        }
        if (hasSignificantWhitespace) {
            scores["Python"] += 5;
        }
    }
};

// Add these new error structs after existing classes
struct BracketError {
    int line;
    int character;
    char bracket;
};

struct QuoteError {
    int line;
    int character;
};

struct SemicolonError {
    int line;
    string lineContent;
};

// Custom Stack implementation for bracket matching
class node {
public:
    char data;
    node* next;
};

class CustomStack {
    node *head, *tail;
public:
    CustomStack() : head(nullptr), tail(nullptr) {}
    
    void push(char value) {
        node* newNode = new node();
        newNode->data = value;
        newNode->next = nullptr;
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    
    char pop() {
        if (head == nullptr) return 'f';
        if (head == tail) {
            char value = head->data;
            delete head;
            head = nullptr;
            tail = nullptr;
            return value;
        }
        node* temp = head;
        while (temp->next != tail) {
            temp = temp->next;
        }
        char value = tail->data;
        delete tail;
        tail = temp;
        tail->next = nullptr;
        return value;
    }
    
    char top() {
        return (head == nullptr) ? 'f' : tail->data;
    }
    
    bool isEmpty() {
        return head == nullptr;
    }
};

// Add these new functions before main()
vector<QuoteError> checkQuotes(const string& text) {
    vector<QuoteError> errors;
    bool inQuote = false;
    int currentLine = 1;
    int currentChar = 1;
    int quoteStartLine = 0;
    int quoteStartChar = 0;

    for (char c : text) {
        if (c == '\n') {
            currentLine++;
            currentChar = 1;
        } else {
            currentChar++;
        }

        if (c == '"' || c == '\'') {
            if (inQuote) {
                inQuote = false;
            } else {
                inQuote = true;
                quoteStartLine = currentLine;
                quoteStartChar = currentChar;
            }
        }
    }

    if (inQuote) {
        errors.push_back({quoteStartLine, quoteStartChar});
    }
    
    return errors;
}

vector<BracketError> checkBrackets(const string& text, const string& language) {
    vector<BracketError> errors;
    CustomStack bracketStack;
    int currentLine = 1;
    int currentChar = 1;

    for (char c : text) {
        if (c == '\n') {
            currentLine++;
            currentChar = 1;
        } else {
            currentChar++;
        }

        if (c == '(' || c == '{' || c == '[') {
            bracketStack.push(c);
        } else if (c == ')' || c == '}' || c == ']') {
            if (bracketStack.isEmpty()) {
                errors.push_back({currentLine, currentChar, c});
            } else {
                char top = bracketStack.pop();
                if ((c == ')' && top != '(') || (c == '}' && top != '{') || (c == ']' && top != '[')) {
                    errors.push_back({currentLine, currentChar, c});
                }
            }
        }
    }

    while (!bracketStack.isEmpty()) {
        char unclosedBracket = bracketStack.pop();
        errors.push_back({-1, -1, unclosedBracket});
    }
    
    return errors;
}

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

vector<SemicolonError> checkSemicolon(const string& line, int lineNumber, const string& language) {
    vector<SemicolonError> errors;
    
    if (language == "C++") {
        // Check for C++ I/O statements
        if (line.find("cout") != string::npos || line.find("cin") != string::npos) {
            if (line.find(';') == string::npos) {
                errors.push_back({lineNumber, line});
            }
        }
    } 
    else if (language == "Java") {
        // Check for Java I/O statements
        if (line.find("System.out.println") != string::npos || 
            line.find("System.out.print") != string::npos ||
            line.find("Scanner") != string::npos) {
            if (line.find(';') == string::npos) {
                errors.push_back({lineNumber, line});
            }
        }
    }
    
    return errors;
}

// Add these helper functions
void printLexicalAnalysisResults(const vector<pair<string, string>>& lexicalUnits) {
    cout << "\nLexical Analysis Results:\n";
    cout << "========================\n";
    for (const auto& unit : lexicalUnits) {
        cout << "Token: " << setw(20) << left << unit.first 
             << " Type: " << unit.second << endl;
    }
}

void printTokenStatistics(const vector<pair<string, string>>& lexicalUnits) {
    cout << "\nToken Statistics:\n";
    cout << "================\n";
    map<string, int> tokenTypes;
    for (const auto& unit : lexicalUnits) {
        tokenTypes[unit.second]++;
    }
    
    // Print statistics with comments included
    for (const auto& [type, count] : tokenTypes) {
        cout << type << ": " << count;
        if (type == "comment") {
            cout << " (Removed during analysis)";
        }
        cout << endl;
    }
}

// Add queue for error tracking
queue<string> errorQueue;
unordered_map<char, char> matchingBrackets = {{')', '('}, {'}', '{'}, {']', '['}};

// Add error logging function
void logError(const string& error) {
    errorQueue.push(error);
}

// Modify the main function to include the additional checks
int main() {
    // Initialize analyzers
    LexicalAnalyzer lexAnalyzer;
    LanguageDetector langDetector;
    
    // Read input file
    ifstream fin("lexicalinput.txt");
    if (!fin.is_open()) {
        cout << "Error opening file\n";
        return 1;
    }
    
    stringstream buffer;
    buffer << fin.rdbuf();
    string code = buffer.str();
    fin.close();
    
    auto lexicalUnits = lexAnalyzer.analyzeLexically(code);
    string detectedLanguage = langDetector.detectLanguage(lexicalUnits);
    
    // 1. First show lexical analysis details
    cout << "Language Detected: " << detectedLanguage << "\n\n";
    
    // Show tokens and their types
    cout << "Token Analysis:\n";
    cout << "---------------\n";
    for (const auto& unit : lexicalUnits) {
        cout << "Token: " << setw(20) << left << unit.first 
             << " Type: " << unit.second << endl;
    }

    // Show token statistics
    cout << "\nToken Statistics:\n";
    cout << "----------------\n";
    int keywords = 0, identifiers = 0, operators = 0, literals = 0;
    for (const auto& unit : lexicalUnits) {
        if (unit.second == "keyword") keywords++;
        else if (unit.second == "identifier") identifiers++;
        else if (unit.second == "operator") operators++;
        else if (unit.second == "string_literal" || unit.second == "numeric_literal") literals++;
    }
    cout << "Keywords: " << keywords << "\n";
    cout << "Identifiers: " << identifiers << "\n";
    cout << "Operators: " << operators << "\n";
    cout << "Literals: " << literals << "\n";

    // 2. Then show any errors found
    cout << "\n=== ERROR ANALYSIS ===\n";
    
    // Track if any errors were found
    bool hasErrors = false;
    
    // Check lexical errors
    const auto& lexicalErrors = lexAnalyzer.getLexicalErrors();
    if (!lexicalErrors.empty()) {
        hasErrors = true;
        cout << "\nLexical Errors Found:\n";
        cout << "-------------------\n";
        for (const auto& error : lexicalErrors) {
            cout << "Line " << error.line << ", Char " << error.character 
                 << ": " << error.message << " '" << error.token << "'\n";
        }
    }

    // Check syntax errors
    vector<QuoteError> quoteErrors = checkQuotes(code);
    vector<BracketError> bracketErrors = checkBrackets(code, detectedLanguage);
    vector<string> indentationErrors;
    vector<SemicolonError> semicolonErrors;

    // Perform language-specific checks
    if (detectedLanguage == "Python") {
        indentationErrors = checkPythonIndentation(code);
    } else {
        istringstream iss(code);
        string line;
        int lineNumber = 0;
        while (getline(iss, line)) {
            lineNumber++;
            auto errors = checkSemicolon(line, lineNumber, detectedLanguage);
            semicolonErrors.insert(semicolonErrors.end(), errors.begin(), errors.end());
        }
    }

    // Show syntax errors if any
    if (!quoteErrors.empty() || !bracketErrors.empty() || 
        !indentationErrors.empty() || !semicolonErrors.empty()) {
        hasErrors = true;
        cout << "\nSyntax Errors Found:\n";
        cout << "-----------------\n";
        
        // Show quote errors
        if (!quoteErrors.empty()) {
            cout << "Quote Balancing:\n";
            for (const auto& error : quoteErrors) {
                cout << "- Unclosed quote at line " << error.line 
                     << ", char " << error.character << "\n";
            }
        }

        // Show bracket errors
        if (!bracketErrors.empty()) {
            cout << "\nBracket Balancing:\n";
            for (const auto& error : bracketErrors) {
                if (error.line == -1) {
                    cout << "- Unclosed opening bracket '" << error.bracket << "'\n";
                } else {
                    cout << "- Unmatched bracket '" << error.bracket 
                         << "' at line " << error.line << "\n";
                }
            }
        }

        // Show language-specific errors
        if (detectedLanguage == "Python" && !indentationErrors.empty()) {
            cout << "\nIndentation Errors:\n";
            for (const auto& error : indentationErrors) {
                cout << "- " << error << "\n";
            }
        }

        if (!semicolonErrors.empty()) {
            cout << "\nMissing Semicolons:\n";
            for (const auto& error : semicolonErrors) {
                cout << "- Line " << error.line << ": " << error.lineContent << "\n";
            }
        }
    }

    // If no errors found, show balanced message
    if (!hasErrors) {
        cout << "\nAll checks passed successfully!\n";
        cout << "- Lexical analysis: No errors\n";
        cout << "- Syntax analysis: All balanced\n";
        cout << "- Language-specific checks: Passed\n";
    }

    return 0;
}
