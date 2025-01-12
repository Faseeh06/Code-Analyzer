import sys
import os
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
            logging.error("Gemini API Key is not configured! Ensure it's set in the .env file.")
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
    except FileNotFoundError:
        logging.error(f"Error: Input file {filename} does not exist!")
        return None
    except Exception as e:
        logging.error(f"Error reading file {filename}: {e}")
        traceback.print_exc()
        return None

def get_gemini_output(user_input, retries=3, delay=5):
    """Send input to Gemini API and get the response."""
    for attempt in range(retries):
        try:
            model = genai.GenerativeModel(
                model_name="gemini-2.0-flash-exp",
                system_instruction=os.getenv("Output"),
            )
            response = model.generate_content(user_input)
            return response.text.strip()
        except Exception as e:
            logging.error(f"Gemini API call attempt {attempt + 1} failed: {e}")
            if attempt < retries - 1:
                time.sleep(delay)
            else:
                return f"Gemini API call failed after {retries} attempts: {e}"

def main(argv):
    """Main function to process the input file and interact with Gemini API."""
    try:
        if len(argv) < 2:
            logging.error("Error: No input file path provided (expected lexicalinput.txt)")
            return

        input_file = argv[1]
        logging.info(f"Processing input file: {input_file}")

        # Read lexical input from the file
        lexical_input = read_file(input_file)
        if lexical_input is None:
            logging.error("Cannot proceed without lexical input")
            return

        # Ensure Gemini API is configured
        if not configure_gemini_api():
            logging.error("Gemini API configuration failed. Exiting.")
            return

        # Prepare the prompt for Gemini API
        gemini_prompt = f"""


{lexical_input}
Only give me the actual output of this code(Suppose there are no error in the code):
"""
        logging.info("Sending prompt to Gemini API...")
        gemini_output = get_gemini_output(gemini_prompt)

        print("\nGemini API Output:")
        print(gemini_output)

    except KeyboardInterrupt:
        logging.info("Script interrupted by user.")
    except Exception as e:
        logging.error(f"Unexpected error in main execution: {e}")
        traceback.print_exc()

if __name__ == "__main__":
    app.run(main)
