'use client'

import React, { useState, useEffect } from 'react'
import styles from './HistoryMenu.module.css'
import SearchEngine from '../utils/SearchEngine'
import { HistoryEntry } from './CodeEditor'

interface HistoryMenuProps {
  history: HistoryEntry[];
  onSelectEntry: (entry: HistoryEntry) => void;
  isOpen: boolean;
  toggleOpen: () => void;
  showMenuButton: boolean;
  searchEngine: SearchEngine;
}

const HistoryMenu: React.FC<HistoryMenuProps> = ({ 
  history, 
  onSelectEntry, 
  isOpen,
  toggleOpen,
  showMenuButton,
  searchEngine
}) => {
  const [isClosing, setIsClosing] = useState(false);
  const [searchQuery, setSearchQuery] = useState('');
  const [searchResults, setSearchResults] = useState<HistoryEntry[]>([]);

  // Initialize search results with full history when component mounts or history changes
  useEffect(() => {
    setSearchResults(history);
  }, [history]);

  // Updated close menu handler
  const handleCloseMenu = () => {
    setIsClosing(true);
    setSearchQuery(''); // Clear search box
    setSearchResults(history); // Reset search results
    setTimeout(() => {
      setIsClosing(false);
      toggleOpen();
    }, 350);
  };

  // Simplified search handling
  const handleSearch = (query: string) => {
    setSearchQuery(query);
    
    if (query.trim() === '') {
      setSearchResults(history);
      return;
    }

    try {
      const results = searchEngine.search(query);
      const filteredHistory = history.filter(entry => 
        results.some(result => 
          result.input === entry.input && 
          result.output === entry.output
        )
      );
      setSearchResults(filteredHistory);
    } catch (error) {
      console.error('Search error:', error);
      setSearchResults([]);
    }
  };

  return (
    <>
      {/* Toggle Button */}
      {showMenuButton && !isOpen && !isClosing && (
        <button className={styles.menuToggle} onClick={toggleOpen}>
          <img 
            src="/logos/logo_zen.png" 
            alt="Menu Logo" 
            className={styles.logoIcon} 
          />
        </button>
      )}

      {/* History Menu */}
      <div 
        className={`${styles.historyMenu} ${isOpen && !isClosing ? styles.open : ''} ${
          isClosing ? styles.closing : ''
        }`}
      >
        {isOpen && (
          <div className={styles.historyHeader}>
            <h2>History</h2>
            <input
              type="text"
              placeholder="Search history..."
              value={searchQuery}
              onChange={(e) => handleSearch(e.target.value)}
              className={styles.searchInput}
            />
            <img 
              src="/logos/logo_zen.png" 
              alt="Close Menu" 
              className={styles.closeIcon} 
              onClick={handleCloseMenu}
            />
          </div>
        )}

        {/* Search Results */}
        <ul className={styles.historyList}>
          {searchResults.length > 0 ? (
            searchResults.map((entry, index) => (
              <li 
                key={`${entry.input}-${index}`}
                onClick={() => onSelectEntry(entry)} 
                className={styles.historyItem}
              >
                <span className={styles.language}>{entry.language || 'Unknown'}</span>
                <span className={styles.input}>
                  {entry.input.length > 30 ? `${entry.input.slice(0, 30)}...` : entry.input}
                </span>
              </li>
            ))
          ) : (
            <li className={styles.noResults}></li>
          )}
        </ul>
      </div>
    </>
  );
};

export default HistoryMenu;