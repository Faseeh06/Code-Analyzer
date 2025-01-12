// For storing code history entries
interface HistoryEntry {
  input: string;
  output: string;
  language: string | null;
  timestamp?: number;
}

// Node structure for the Trie data structure with children, word status and frequency tracking
class TrieNode {
  children: Map<string, TrieNode>;
  isEndOfWord: boolean;
  entries: Set<number>; 
  frequency: number;

  constructor() {
    this.children = new Map();
    this.isEndOfWord = false;
    this.entries = new Set();
    this.frequency = 0;S
  }
}

// Trie data structure for efficient prefix-based search functionality
class Trie {
  root: TrieNode;
  
  constructor() {
    this.root = new TrieNode();
  }

  insert(word: string, entryIndex: number): void {
    let current = this.root;
    // Insert all possible prefixes
    for (let i = 0; i < word.length; i++) {
      const prefix = word.substring(0, i + 1);
      this.insertPrefix(prefix, entryIndex);
    }
  }

  private insertPrefix(word: string, entryIndex: number): void {
    let current = this.root;
    for (const char of word) {
      if (!current.children.has(char)) {
        current.children.set(char, new TrieNode());
      }
      current = current.children.get(char)!;
    }
    current.isEndOfWord = true;
    current.entries.add(entryIndex);
    current.frequency++;
  }

  search(prefix: string): Set<number> {
    let current = this.root;
    for (const char of prefix) {
      if (!current.children.has(char)) {
        return new Set();
      }
      current = current.children.get(char)!;
    }
    return this.getAllEntriesBelow(current);
  }

  private getAllEntriesBelow(node: TrieNode): Set<number> {
    let entries = new Set<number>();
    
    const traverse = (current: TrieNode) => {
      for (const entry of current.entries) {
        entries.add(entry);
      }
      for (const child of current.children.values()) {
        traverse(child);
      }
    };
    
    traverse(node);
    return entries;
  }
}

// Min heap implementation for tracking most frequent searches
class MinHeap {
  private heap: [string, number][] = [];

  insert(word: string, frequency: number): void {
    this.heap.push([word, frequency]);
    this.bubbleUp(this.heap.length - 1);
  }

  private bubbleUp(index: number): void {
    while (index > 0) {
      const parentIdx = Math.floor((index - 1) / 2);
      if (this.heap[parentIdx][1] > this.heap[index][1]) {
        [this.heap[parentIdx], this.heap[index]] = [this.heap[index], this.heap[parentIdx]];
        index = parentIdx;
      } else {
        break;
      }
    }
  }

  getMin(): [string, number] | undefined {
    return this.heap[0];
  }
}

// AVL Tree node structure for balanced binary search tree
class AVLNode {
  entry: HistoryEntry;
  left: AVLNode | null;
  right: AVLNode | null;
  height: number;

  constructor(entry: HistoryEntry) {
    this.entry = entry;
    this.left = null;
    this.right = null;
    this.height = 1;
  }
}

// Self-balancing AVL Tree implementation for ordered storage
class AVLTree {
  root: AVLNode | null;

  constructor() {
    this.root = null;
  }

  private getHeight(node: AVLNode | null): number {
    return node ? node.height : 0;
  }

  private getBalance(node: AVLNode): number {
    return this.getHeight(node.left) - this.getHeight(node.right);
  }

  private rightRotate(y: AVLNode): AVLNode {
    const x = y.left!;
    const T2 = x.right;

    x.right = y;
    y.left = T2;

    y.height = Math.max(this.getHeight(y.left), this.getHeight(y.right)) + 1;
    x.height = Math.max(this.getHeight(x.left), this.getHeight(x.right)) + 1;

    return x;
  }

  private leftRotate(x: AVLNode): AVLNode {
    const y = x.right!;
    const T2 = y.left;

    y.left = x;
    x.right = T2;

    x.height = Math.max(this.getHeight(x.left), this.getHeight(x.right)) + 1;
    y.height = Math.max(this.getHeight(y.left), this.getHeight(y.right)) + 1;

    return y;
  }

  insert(entry: HistoryEntry): void {
    this.root = this._insert(this.root, entry);
  }

  private _insert(node: AVLNode | null, entry: HistoryEntry): AVLNode {
    if (!node) return new AVLNode(entry);

    if (entry.input < node.entry.input) {
      node.left = this._insert(node.left, entry);
    } else if (entry.input > node.entry.input) {
      node.right = this._insert(node.right, entry);
    } else {
      return node;
    }

    node.height = 1 + Math.max(this.getHeight(node.left), this.getHeight(node.right));

    const balance = this.getBalance(node);

    if (balance > 1 && entry.input < node.left!.entry.input) {
      return this.rightRotate(node);
    }

    if (balance < -1 && entry.input > node.right!.entry.input) {
      return this.leftRotate(node);
    }

    if (balance > 1 && entry.input > node.left!.entry.input) {
      node.left = this.leftRotate(node.left!);
      return this.rightRotate(node);
    }

    if (balance < -1 && entry.input < node.right!.entry.input) {
      node.right = this.rightRotate(node.right!);
      return this.leftRotate(node);
    }

    return node;
  }

  inOrderTraversal(): HistoryEntry[] {
    const result: HistoryEntry[] = [];
    this._inOrderTraversal(this.root, result);
    return result;
  }

  private _inOrderTraversal(node: AVLNode | null, result: HistoryEntry[]): void {
    if (node) {
      this._inOrderTraversal(node.left, result);
      result.push(node.entry);
      this._inOrderTraversal(node.right, result);
    }
  }
}

// Main search engine class combining multiple data structures for efficient search
class SearchEngine {
  private invertedIndex: Map<string, Set<number>>;  // For word lookup
  private entries: HistoryEntry[];                  // Main storage
  private trie: Trie;                              // For prefix searching
  private avlTree: AVLTree;                        // For ordered access
  private minHeap: MinHeap;                        // For frequent searches
  private searchFrequency: Map<string, number>;    // For tracking search counts

  constructor() {
    this.invertedIndex = new Map();
    this.entries = [];
    this.trie = new Trie();
    this.avlTree = new AVLTree();
    this.minHeap = new MinHeap();
    this.searchFrequency = new Map();
  }

  // Adds a new entry to the search history and updates all data structures
  addEntry(entry: HistoryEntry): void {
    const index = this.entries.length;
    entry.timestamp = Date.now();
    this.entries.push(entry);
    this.avlTree.insert(entry);

    // Tokenize both input and output separately for better search
    const inputWords = this.tokenize(entry.input);
    const outputWords = this.tokenize(entry.output);
    const allWords = new Set([...inputWords, ...outputWords]);
    
    for (const word of allWords) {
      if (!this.invertedIndex.has(word)) {
        this.invertedIndex.set(word, new Set());
      }
      this.invertedIndex.get(word)!.add(index);
      this.trie.insert(word, index);
    }
  }

  // Performs search across entries using multiple data structures for efficiency
  search(query: string): HistoryEntry[] {
    if (!query || query.trim() === '') {
      return this.getAllEntries();
    }

    const searchWords = this.tokenize(query);
    if (searchWords.length === 0) {
      return this.getAllEntries();
    }

    // Get matching entries using Trie
    let matchingIndices = new Set<number>();
    let isFirstWord = true;

    for (const word of searchWords) {
      const currentMatches = this.trie.search(word);
      
      if (isFirstWord) {
        matchingIndices = currentMatches;
        isFirstWord = false;
      } else {
        // Intersection with previous matches
        matchingIndices = new Set(
          [...matchingIndices].filter(x => currentMatches.has(x))
        );
      }

      if (matchingIndices.size === 0) break;
    }

    // Convert indices to entries
    const results = Array.from(matchingIndices)
      .map(index => this.entries[index])
      .filter(entry => entry !== undefined);

    // Sort by timestamp (most recent first)
    return results.sort((a, b) => 
      ((b.timestamp || 0) - (a.timestamp || 0))
    );
  }

  // Updates the frequency count for search terms
  private updateSearchFrequency(query: string): void {
    const count = (this.searchFrequency.get(query) || 0) + 1;
    this.searchFrequency.set(query, count);
    this.minHeap.insert(query, count);
  }

  // Breaks text into searchable tokens
  private tokenize(text: string): string[] {
    if (!text) return [];
    return text.toLowerCase()
      .split(/\W+/)
      .filter(word => word.length > 0);
  }

  // Returns all entries in order
  getAllEntries(): HistoryEntry[] {
    return this.avlTree.inOrderTraversal();
  }

  // Returns the k most frequent searches
  getTopSearches(k: number): [string, number][] {
    const temp = [...this.searchFrequency.entries()];
    temp.sort((a, b) => b[1] - a[1]);
    return temp.slice(0, k);
  }
}

export default SearchEngine;

