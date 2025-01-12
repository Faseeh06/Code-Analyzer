// Sample code snippet used for demonstration
const codeExample = `
for(int i=0; i<10; i++) {
    cout << "Hello";
}
`;

/**
 * Class demonstrating the tokenization process used in the code editor
 * Shows how code is broken down into searchable tokens and indexed
 */
class TokenizingExample {
    /**
     * Converts input text into searchable tokens
     * @param text - The input code/text to tokenize
     * @returns Array of individual tokens extracted from the text
     */
    private tokenize(text: string): string[] {
        // Convert all text to lowercase for consistent matching
        // Split on any non-word character (spaces, punctuation, etc.)
        // Filter out any empty tokens
        const tokens = text.toLowerCase()
            .split(/\W+/)
            .filter(word => word.length > 0);
        
        console.log("Tokenization process:");
        return tokens;
    }

    /**
     * Demonstrates the complete tokenization and indexing process
     * Shows:
     * 1. How code is broken into tokens
     * 2. How tokens are stored with their positions
     * 3. How the index mapping works
     */
    demonstrateTokenizing() {
        // Display the original code for reference
        console.log("Original code:", codeExample);

        // Convert code into tokens
        const tokens = this.tokenize(codeExample);
        console.log("Tokens created:", tokens);
        // Shows how 'for(int i=0; i<10; i)' becomes ['for', 'int', 'i', '0', 'i', '10', 'i']

        // Create an index map to track token positions
        // Key: token string
        // Value: array of positions where token appears
        const indexMapping = new Map<string, number[]>();
        
        // Build the index mapping
        tokens.forEach((token, index) => {
            // Initialize array for new tokens
            if (!indexMapping.has(token)) {
                indexMapping.set(token, []);
            }
            // Add current position to token's position list
            indexMapping.get(token)!.push(index);
        });

        // Display the final index mapping
        console.log("\nToken storage in index mapping:");
        indexMapping.forEach((positions, token) => {
            console.log(`Token "${token}" appears at positions:`, positions);
        });
        /* Example output explains:
         * - How 'i' appears multiple times at different positions
         * - How each unique token is stored with its positions
         * - How this enables efficient token searching
         */
    }
}
