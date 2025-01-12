1. A Lexical analysis to detect the Language
2. A Basic code syntax checker(Implementing for C++ as of Now)
3. Gemini Integration




DSA Concepts used :
Let me analyze the Data Structures and Algorithms (DSA) concepts used in both code files:

Data Structures Used:

a) Basic Data Structures:

Vectors (vector<>) - For storing various tokens and errors
Sets (unordered_set<>) - For storing and looking up keywords and operators
Strings (string) - For text processing
Arrays - Basic arrays used for storing characters and operators

b) Custom Data Structures:

Stack (Custom Implementation):

Implemented using a linked list structure
Used for bracket matching/balancing
Has basic operations: push(), pop(), top(), isEmpty()



c) Linked List (as part of Stack):

Used in the stack implementation
Nodes containing char data and next pointer
Maintains head and tail pointers

d) Structs:

BracketError - For storing bracket error information
QuoteError - For storing quote error information


Algorithms and Techniques:

a) String Processing:

Character-by-character parsing
Token identification
Lexical analysis

b) Pattern Matching:

Bracket matching algorithm
Quote matching algorithm
Keyword detection

c) Searching:

Hash-based searching (through unordered_sets)
Linear searching in arrays and vectors

d) Language Detection Algorithm:

Score-based language detection
Keyword frequency analysis
Pattern matching for language-specific operators


Key Algorithm Concepts:

a) Stack Operations:

Push and Pop operations
Top element access
Stack-based bracket matching

b) Iteration Techniques:

Linear traversal of strings
File reading character by character
Vector and array traversal

c) Error Handling:

File I/O error checking
Edge case handling in stack operations
Error position tracking


Time Complexity Analysis:


Stack Operations: O(1) for push, O(n) for pop and top (due to linked list implementation)
Keyword Lookup: O(1) average case (using unordered_set)
String Processing: O(n) where n is the length of input
Language Detection: O(k) where k is number of detected keywords


Space Complexity:


O(n) for storing the input text
O(k) for storing keywords and operators
O(m) for storing errors where m is number of errors
O(p) for stack space where p is number of nested brackets

These DSA concepts work together to create:

A lexical analyzer that detects programming language features
A bracket and quote matching system
A language detection system based on keyword frequency