'use client'

import React, { useState, useEffect } from 'react'
import styles from './CodeEditor.module.css'
import ThemeToggle from './ThemeToggle'
import LanguageDisplay from './LanguageDisplay'
import HistoryMenu from './HistoryMenu'
import { LinkedList } from '../utils/LinkedList'
import { saveToLocalStorage, loadFromLocalStorage } from '../utils/LocalStorage'
import SearchEngine from '../utils/SearchEngine'

export interface HistoryEntry {
  input: string;
  output: string;
  autocompleteOutput: string;
  language: string | null;
}

const CodeEditor: React.FC = () => {
  const [input, setInput] = useState('')
  const [output, setOutput] = useState('')
  const [autocompleteOutput, setAutocompleteOutput] = useState('')
  const [theme, setTheme] = useState<'light' | 'dark'>('dark')
  const [isLoading, setIsLoading] = useState(false)
  const [detectedLanguage, setDetectedLanguage] = useState<string | null>(null)
  const [history, setHistory] = useState<LinkedList<HistoryEntry>>(() => {
    const savedHistory = loadFromLocalStorage('codeEditorHistory');
    return savedHistory ? LinkedList.fromArray(savedHistory) : new LinkedList<HistoryEntry>();
  });
  const [isHistoryOpen, setIsHistoryOpen] = useState(false)
  const [searchEngine] = useState(() => {
    const savedHistory = loadFromLocalStorage('codeEditorHistory');
    const engine = new SearchEngine();
    if (savedHistory) {
      savedHistory.forEach((entry: HistoryEntry) => {
        engine.addEntry(entry);
      });
    }
    return engine;
  });

  const detectLanguage = (firstLine: string) => {
    const lowercaseLine = firstLine.toLowerCase()
    if (lowercaseLine.includes('c++')) return 'C++'
    if (lowercaseLine.includes('python')) return 'Python'
    if (lowercaseLine.includes('java')) return 'Java'
    return null
  }

  const handleRun = async () => {
    setIsLoading(true)
    setOutput('')
    setAutocompleteOutput('')
    
    try {
      const response = await fetch('/api/run-code', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ code: input }),
      })

      const data = await response.json()

      if (!response.ok) {
        throw new Error(data.error || 'An unknown error occurred')
      }

      // Set main output
      setOutput(data.output || 'No output')
      
      // Set autocomplete output
      setAutocompleteOutput(data.autocompleteOutput || 'No autocomplete suggestions')

      // Detect language from main output
      const language = detectLanguage(data.output.split('\n')[0])
      setDetectedLanguage(language)

      // Add to history
      setHistory(prevHistory => {
        const newHistory = new LinkedList<HistoryEntry>();
        prevHistory.toArray().forEach(entry => newHistory.append(entry));
        const newEntry = { 
          input, 
          output: data.output, 
          autocompleteOutput: data.autocompleteOutput,
          language 
        };
        newHistory.append(newEntry);
        searchEngine.addEntry(newEntry);
        return newHistory;
      });
    } catch (error) {
      console.error('Error running code:', error)
      setOutput(`Error: ${error instanceof Error ? error.message : 'An unknown error occurred'}`)
      setAutocompleteOutput('Autocomplete processing failed')
    } finally {
      setIsLoading(false)
    }
  }

  const toggleTheme = () => {
    setTheme(prevTheme => prevTheme === 'light' ? 'dark' : 'light')
  }

  const loadHistoryEntry = (entry: HistoryEntry) => {
    setInput(entry.input)
    setOutput(entry.output)
    setAutocompleteOutput(entry.autocompleteOutput)
    setDetectedLanguage(entry.language)
    setIsHistoryOpen(false)
  }

  useEffect(() => {
    const savedHistory = loadFromLocalStorage('codeEditorHistory');
    if (savedHistory) {
      const newHistory = new LinkedList<HistoryEntry>();
      savedHistory.forEach((entry: HistoryEntry) => {
        newHistory.append(entry);
      });
      setHistory(newHistory);
    }
  }, []);

  useEffect(() => {
    saveToLocalStorage('codeEditorHistory', history.toArray());
  }, [history]);

  useEffect(() => {
    if (detectedLanguage) {
      document.body.setAttribute('data-language', detectedLanguage.toLowerCase())
    } else {
      document.body.removeAttribute('data-language')
    }
  }, [detectedLanguage])

  return (
    <div className={`${styles.container} ${styles[theme]}`}>
      <div className={styles.header}>
        <div className={styles.placeholder}></div>
        <LanguageDisplay language={detectedLanguage} />
        <button className={styles.runButton} onClick={handleRun} disabled={isLoading}>
          {isLoading ? 'Running...' : 'Run'}
        </button>
      </div>
      <div className={styles.editorContainer}>
        <div className={styles.editorBox}>
          <textarea
            value={input}
            onChange={(e) => setInput(e.target.value)}
            className={styles.editor}
            placeholder="Enter your code here..."
            spellCheck="false"
            style={{ position: 'absolute', top: 0, left: 0 }}
          />
        </div>
        <div className={styles.outputContainer}>
          <div className={styles.outputBox}>
            
            <pre>
              <code>{output}</code>
            </pre>
          </div>
          <div className={styles.outputBox}>
            
            <pre>
              <code>{autocompleteOutput}</code>
            </pre>
          </div>
        </div>
      </div>
      <ThemeToggle theme={theme} toggleTheme={toggleTheme} />
      <HistoryMenu
        history={history.toArray()}
        onSelectEntry={loadHistoryEntry}
        isOpen={isHistoryOpen}
        toggleOpen={() => setIsHistoryOpen(!isHistoryOpen)}
        showMenuButton={true}
        searchEngine={searchEngine}
      />
    </div>
  )
}

export default CodeEditor

