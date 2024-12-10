#!/usr/bin/env node
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');
const readline = require('readline');

const filePath = path.join(__dirname, 'gamejam2024.z64');

let aresProcess = null;

function startAresProcess() {
  if (aresProcess) {
    console.log('File changed again. Killing previous process...');
    aresProcess.kill();
  }

  console.log(`Starting ares process for ${filePath}`);
  aresProcess = spawn('ares', [filePath]);

  // Set up readline for formatting ares output
  const rl = readline.createInterface({
    input: aresProcess.stdout,
    output: process.stdout,
    terminal: false,
  });

  let buffer = []; // Store lines to display

  rl.on('line', (line) => {
    if (process.env.VSYNC) {
      if (line.startsWith('----')) {
        // Clear the screen and render the buffer
        console.clear();
        console.log(buffer.join('\n'));
        buffer = []; // Clear the buffer after rendering
      } else {
        buffer.push(line); // Add line to the buffer
      }
    } else {
      console.log(line);
    }
  });

  rl.on('close', () => {
    if (buffer.length > 0) {
      console.log(buffer.join('\n')); // Print remaining content
    }
  });

  aresProcess.on('close', (code) => {
    if (code !== null) {
      console.log(`ares process exited with code ${code}`);
      aresProcess = null;
    }
  });

  aresProcess.stderr.on('data', (data) => {
    console.error(`ares stderr: ${data}`);
  });
}

// Watch for file changes
fs.watchFile(filePath, { interval: 500 }, (curr, prev) => {
  if (
    curr.mtimeMs !== prev.mtimeMs && // changed
    curr.ctimeMs !== 0 // not deleted
  ) {
    console.log(`Detected change in ${filePath}`);
    startAresProcess();
  }
});

console.log(`Watching for changes on ${filePath}...`);
if (fs.existsSync(filePath)) {
  startAresProcess();
}
