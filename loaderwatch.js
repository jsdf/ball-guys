#!/usr/bin/env node
const fs = require('fs');
const path = require('path');
const {spawn} = require('child_process');

const filePath = path.join(__dirname, 'gamejam2024.z64');

function onChange() {
  console.log(`Uploading ${filePath}...`);
  // Run the command with the changed file path
  const command = 'unfloader';
  const args = ['-r', filePath, '-t', '5', '-b'];

  const process = spawn(command, args, {stdio: 'inherit'});

  process.on('close', (code) => {
    console.log(`Command finished with exit code ${code}`);
  });
}

fs.watchFile(filePath, {interval: 500}, (curr, prev) => {
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
