#!/usr/bin/env node
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');

const filePath = path.join(__dirname, 'gamejam2024.z64');

let loaderProcess = null;

function onChange() {
  if (loaderProcess) {
    console.log('File changed again. Killing previous process...');
    loaderProcess.kill();
  }

  console.log(`Starting unfloader process for ${filePath}`);

  loaderProcess = spawn('unfloader', [
    filePath,
    '-d', // debugf() messages 
    '-b'// disable ncurses interface
  ], { stdio: 'inherit' });

  loaderProcess.on('close', (code) => {
    if (code !== null) {
      console.log(`unfloader process exited with code ${code}`);
      loaderProcess = null;
    }
  });
}

fs.watchFile(filePath, { interval: 500 }, (curr, prev) => {
  if (
    curr.mtimeMs !== prev.mtimeMs && // changed
    curr.ctimeMs !== 0 // not deleted
  ) {
    console.log(`Detected change in ${filePath}`);
    onChange();
  }
});

console.log(`Watching for changes on ${filePath}...`);
if (fs.existsSync(filePath)) {
  onChange();
}
