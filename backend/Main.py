import sys
import os
import subprocess
import traceback
import time
from absl import app
from absl import logging
import google.generativeai as genai
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()

def configure_gemini_api():
    """Configure Gemini API with error handling."""
    try:
        GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
        if not GEMINI_API_KEY:
            logging.error("Gemini API Key is not configured!")
            return False
        genai.configure(api_key=GEMINI_API_KEY)
        return True
    except Exception as e:
        logging.error(f"Gemini API Configuration Error: {e}")
        return False

def read_file(filename):
    """Read file with comprehensive error handling."""
    try:
        with open(filename, "r", encoding="utf-8") as file:
            return file.read()
    except Exception as e:
        logging.error(f"Error reading file {filename}: {e}")
        return None


def run_cpp_program(cpp_filename):
    """Compile and run a C++ program, returning its output."""
    try:
        exe_file = os.path.splitext(cpp_filename)[0] + ".exe"
        compile_cmd = ["g++", cpp_filename, "-o", exe_file]
        compile_process = subprocess.run(compile_cmd, capture_output=True, text=True)

        if compile_process.returncode != 0:
            logging.error(f"Compilation error in {cpp_filename}:")
            logging.error(compile_process.stderr)
            return f"Compilation failed for {cpp_filename}"

        run_process = subprocess.run([exe_file], capture_output=True, text=True)
        if run_process.returncode != 0:
            logging.error(f"Runtime error in {cpp_filename}:")
            logging.error(run_process.stderr)
            return f"Runtime error in {cpp_filename}"

        return run_process.stdout.strip()

    except Exception as e:
        logging.error(f"Error executing {cpp_filename}: {e}")
        traceback.print_exc()
        return None


def get_gemini_output(user_input, retries=3, delay=5):
    """Send input to Gemini API and get the response."""
    for attempt in range(retries):
        try:
            model = genai.GenerativeModel(model_name='gemini-2.0-flash-exp',
    system_instruction = os.getenv("ZEN")
)
            response = model.generate_content(user_input)
            return response.text
        except Exception as e:
            logging.error(f"Gemini API call attempt {attempt + 1} failed: {e}")
            if attempt < retries - 1:
                time.sleep(delay)
            else:
                return f"Gemini API call failed after {retries} attempts: {e}"


def main(argv):
    """Main function to orchestrate the workflow."""
    try:
        if len(argv) < 2:
            logging.error("Error: No input file path provided (expected lexicalinput.txt)")
            return

        input_file = argv[1]
        if not configure_gemini_api():
            logging.error("Gemini API configuration failed")
            return

        # Read lexical input
        lexical_input = read_file(input_file)
        if lexical_input is None:
            logging.error("Cannot proceed without lexical input")
            return

        # Run merged.cpp output
        logging.info("Running merged.cpp...")
        cpp_output = run_cpp_program("merged.cpp")
        print(cpp_output)

        # Prepare Gemini input
        gemini_input = f"""
Code Analysis Request:
---------------------
Source Code:
{lexical_input}

Analysis Results:
{cpp_output}

The code is: "Code of the file", Output is: "Stderr or stdout from console".
Explain me the errors only.(especially the syntax ones.)
"""

        # Pass data to Gemini API and print response
        logging.info("Requesting analysis from Gemini API...")
        gemini_analysis = get_gemini_output(gemini_input)

        print("\nAI Analysis:")
        print("-----------")
        print(gemini_analysis)

    except KeyboardInterrupt:
        logging.info("Script interrupted by user")
    except Exception as e:
        logging.error(f"Unexpected error in main execution: {e}")
        traceback.print_exc()


if __name__ == "__main__":
    app.run(main)
