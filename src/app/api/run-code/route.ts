import { NextResponse } from 'next/server';
import { execFile } from 'child_process';
import { promisify } from 'util';
import fs from 'fs/promises';
import path from 'path';

const execFileAsync = promisify(execFile);

export async function POST(request: Request) {
  try {
    const { code } = (await request.json()) as { code: string };

    // Ensure the backend folder exists
    const backendFolder = path.join(process.cwd(), 'backend');
    await fs.mkdir(backendFolder, { recursive: true });
    console.log('Backend folder path:', backendFolder);

    // Write the input to lexicalinput.txt
    const inputPath = path.join(backendFolder, 'lexicalinput.txt');
    await fs.writeFile(inputPath, code, 'utf-8');
    console.log(`Input file written to: ${inputPath}`);

    // Execute main.py
    const mainPythonPath = path.join(backendFolder, 'main.py');
    console.log(`Executing main Python script: ${mainPythonPath}`);

    const mainResult = await execFileAsync('python', [mainPythonPath, inputPath], {
      cwd: backendFolder,
      timeout: 60000, // 60 seconds timeout
    });

    if (mainResult.stderr && (mainResult.stderr.includes('ERROR') || mainResult.stderr.includes('CRITICAL'))) {
      console.error('Main Python script error:', mainResult.stderr);
      return NextResponse.json(
        { error: 'Error executing main Python script', details: mainResult.stderr },
        { status: 500 }
      );
    }

    console.log('Main Python script output:', mainResult.stdout);

    // Save main output to a file for autocomplete to read
    const mainOutputPath = path.join(backendFolder, 'lexicalinput.txt');
    await fs.writeFile(mainOutputPath, mainResult.stdout, 'utf-8');
    console.log(`Main output written to: ${mainOutputPath}`);

    // Execute autocomplete.py
    const autocompletePythonPath = path.join(backendFolder, 'autocomplete.py');
    console.log(`Executing autocomplete Python script: ${autocompletePythonPath}`);

    try {
      const autocompleteResult = await execFileAsync('python', [autocompletePythonPath, mainOutputPath], {
        cwd: backendFolder,
        timeout: 60000, // 60 seconds timeout
        env: {
          ...process.env,
          PYTHONPATH: backendFolder, // Set PYTHONPATH for backend folder
        },
      });

      console.log('Raw autocomplete output:', autocompleteResult.stdout);
      console.log('Raw autocomplete errors:', autocompleteResult.stderr);

      if (
        autocompleteResult.stderr &&
        !autocompleteResult.stderr.includes('WARNING') &&
        (autocompleteResult.stderr.includes('ERROR') || autocompleteResult.stderr.includes('CRITICAL'))
      ) {
        console.error('Autocomplete script error:', autocompleteResult.stderr);
        return NextResponse.json({
          output: mainResult.stdout,
          autocompleteOutput: `Error in autocomplete: ${autocompleteResult.stderr}`,
        });
      }

      // Clean up temporary file
      await fs.unlink(mainOutputPath).catch(console.error);

      return NextResponse.json({
        output: mainResult.stdout || 'No output',
        autocompleteOutput: autocompleteResult.stdout || 'No suggestions available',
      });
    } catch (autocompleteError) {
      console.error('Autocomplete execution error:', autocompleteError);
      return NextResponse.json({
        output: mainResult.stdout || 'No output',
        autocompleteOutput: 'Autocomplete processing failed',
      });
    }
  } catch (error: unknown) {
    console.error('API route error:', error);

    if (error instanceof Error) {
      const execError = error as Error & { killed: boolean; signal: string };
      if ('killed' in execError && execError.killed && execError.signal === 'SIGTERM') {
        return NextResponse.json(
          { error: 'Script execution timed out', details: 'The Python script took too long to respond.' },
          { status: 504 }
        );
      }
      return NextResponse.json(
        { error: 'Internal server error', details: error.message },
        { status: 500 }
      );
    }

    return NextResponse.json(
      { error: 'Unknown error occurred', details: 'An unexpected error occurred' },
      { status: 500 }
    );
  }
}
