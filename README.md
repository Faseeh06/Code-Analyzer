# Zenalyze: Advanced Code Analysis and Autocomplete Tool

Zenalyze is an innovative code analysis and autocomplete tool designed to enhance the coding experience for developers of all skill levels. It combines powerful lexical analysis, language detection, and AI-driven code suggestions to provide a seamless and intelligent coding environment.

![image](https://github.com/user-attachments/assets/ab72fe6f-a177-40b7-b805-669804c2091c)


## Table of Contents
1. [Features](#features)
2. [Technologies Used](#technologies-used)
3. [Setup and Installation](#setup-and-installation)
4. [Running the Application](#running-the-application)
5. [Usage Guide](#usage-guide)
6. [Contributing](#contributing)
7. [License](#license)

## Features

- Real-time lexical analysis and syntax highlighting
- Automatic language detection for C++, Java, and Python
- AI-powered code autocomplete suggestions
- Intelligent error detection and explanation
- Dark/Light theme toggle
- Code history management with efficient search functionality
- Responsive design for various screen sizes

  ![image](https://github.com/user-attachments/assets/e2fd7be6-3da8-447b-8e49-af5f359ea421)


## Technologies Used

- **Frontend:**
  - React.js with TypeScript (TSX)
  - Next.js for server-side rendering and routing
  - Tailwind CSS for styling
  - shadcn/ui for UI components

- **Backend:**
  - C++ for core lexical analysis and language detection
  - Python for AI integration and advanced code analysis
  - Google's Gemini AI API for code suggestions and error explanations

- **Build Tools:**
  - Webpack (integrated with Next.js)
  - Babel for JavaScript transpilation

- **Version Control:**
  - Git



## Setup and Installation

1. Clone the repository
   
2. Install Node.js dependencies
   
3. Set up Python environment
  
4. Compile the C++ backend
   
5. Set up environment variables:
Create a `.env` file in the root directory and add your Gemini API key

GEMINI_API_KEY=your_api_key_here


## Running the Application

1 Start the Next.js development server:
        **npm run dev**



3. Open your browser and navigate to `http://localhost:3000`

## Usage Guide

1. **Code Editor:** Type or paste your code into the main editor area.
2. **Language Detection:** The detected language will be displayed automatically.
3. **Run Analysis:** Click the "Run" button to perform lexical analysis and get AI suggestions.
4. **View Output:** Check the output area for analysis results and error explanations.
5. **Autocomplete:** As you type, autocomplete suggestions will appear.
6. **History:** Access your coding history using the history menu icon.
7. **Theme Toggle:** Switch between light and dark themes using the toggle in the bottom right corner.

## Contributing

We welcome contributions to Zenalyze! Please follow these steps to contribute:

1. Fork the repository
2. Create a new branch: `git checkout -b feature-branch-name`
3. Make your changes and commit them: `git commit -m 'Add some feature'`
4. Push to the branch: `git push origin feature-branch-name`
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
