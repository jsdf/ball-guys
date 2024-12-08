const readline = require('readline');

// Set up interface for reading stdin
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

let buffer = []; // Store lines to display

rl.on('line', (line) => {
    if (line.startsWith('----')) {
        // Clear the screen and render the buffer
        console.clear();
        console.log(buffer.join('\n'));
        buffer = []; // Clear the buffer after rendering
    } else {
        buffer.push(line); // Add line to the buffer
    }
});

// Gracefully handle program exit
rl.on('close', () => {
    if (buffer.length > 0) {
        console.log(buffer.join('\n')); // Print remaining content
    }
    process.exit(0);
});
