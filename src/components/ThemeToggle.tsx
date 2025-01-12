import React from 'react'
import styles from './CodeEditor.module.css'
import { Sun, Moon } from 'lucide-react'

interface ThemeToggleProps {
  theme: 'light' | 'dark'
  toggleTheme: () => void
}

const ThemeToggle: React.FC<ThemeToggleProps> = ({ theme, toggleTheme }) => {
  return (
    <button 
      className={styles.themeToggle} 
      onClick={toggleTheme}
      aria-label={`Switch to ${theme === 'light' ? 'dark' : 'light'} mode`}
      data-theme={theme} // Add data-theme attribute to toggle icon color
    >
      {theme === 'light' ? (
        <Moon className={styles.themeIcon} />
      ) : (
        <Sun className={styles.themeIcon} />
      )}
    </button>
  )
}

export default ThemeToggle
