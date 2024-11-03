#!/usr/bin/env node
const fs = require('fs');
const path = require('path');
const {spawn} = require('child_process');

const filePath = path.join(__dirname, 'gamejam2024.z64');

let aresProcess = null;

function onChange() {
  if (aresProcess) {
    console.log('File changed again. Killing previous process...');
    aresProcess.kill();
  }

  console.log(`Starting ares process for ${filePath}`);
  aresProcess = spawn('ares', [filePath], {stdio: 'inherit'});

  aresProcess.on('close', (code) => {
    if (code !== null) {
      console.log(`ares process exited with code ${code}`);
      aresProcess = null;
    }
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
