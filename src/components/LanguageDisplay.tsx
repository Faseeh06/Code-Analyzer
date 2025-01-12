    import React from 'react'
    import Image from 'next/image'
    import styles from './LanguageDisplay.module.css'

    interface LanguageDisplayProps {
    language: string | null
    }

    const LanguageDisplay: React.FC<LanguageDisplayProps> = ({ language }) => {
    const getLanguageLogo = (lang: string) => {
        switch (lang.toLowerCase()) {
        case 'c++':
            return '/logos/c++_logo.png'
        case 'python':
            return '/logos/python_logo.png'
        case 'java':
            return '/logos/java.png'
        default:
            return null
        }
    }

    const logoPath = language ? getLanguageLogo(language) : null

    return (
        <div className={styles.languageDisplay}>
        {language && (
            <>
            {logoPath && (
                <Image
                src={logoPath}
                alt={`${language} logo`}
                width={48}
                height={48}
                layout="intrinsic"
                className={styles.languageLogo}
              />
              
            )}
            <span className={styles.languageName}>{language}</span>
            </>
        )}
        </div>
    )
    }

    export default LanguageDisplay

