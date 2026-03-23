\# 🗜️ YZIP: The File Squisher



> \*\*"If your files are looking a little too thicc, YZIP puts them on a mathematical calorie deficit."\*\*



Most compression tools are boring. YZIP is a custom-built, multi-stage data compression and encryption engine written in raw C++ with a sleek Python Tkinter GUI. It doesn't just zip your files; it squashes spatial redundancy, mathematically optimizes statistical frequencies, packs the bits, and locks it all inside an XOR cryptographic vault. 



Built as a flex for a Design and Analysis of Algorithms (DAA) project. 



\---



\## 🚀 The Architecture (How it Cooks)



YZIP runs on a custom 3-stage pipeline to achieve maximum compression without losing a single byte of data.



\### 1️⃣ Phase 1: Smart Run-Length Encoding (RLE)

Standard Huffman coding chokes on massive blocks of empty space or repeating characters. YZIP intercepts the raw text first and applies a \*\*Smart RLE filter\*\*. 

\* \*\*The Logic:\*\* If a character repeats 3 or more times (e.g., 100 spaces), it gets squashed into a tiny metadata tag using a custom `\~` escape character (`\~100| `). 

\* \*\*Time Complexity:\*\* O(n)

\* \*\*Why it matters:\*\* It absolutely destroys the file bloat found in CSVs and formatted code scripts before the heavy lifting even begins.



\### 2️⃣ Phase 2: Pure Min-Heap Huffman Coding

This is the core engine. YZIP reads the RLE-squashed text and calculates the exact probability of every single character.

\* \*\*The Logic:\*\* Using a perfectly balanced \*\*Min-Heap\*\*, it builds a binary tree where the most frequent characters get incredibly short bit-paths (like `00`), and rare characters get longer ones. 

\* \*\*The Squeeze:\*\* It translates the entire text into raw binary, and then uses C++ bitwise operators (`<<`, `>>`) to physically pack 8 bits into a single `unsigned char` byte. 

\* \*\*Time Complexity:\*\* O(n log n) for building the tree, O(n) for traversing.



\### 3️⃣ Phase 3: Late-Stage XOR Encryption

Compressing data is cool, but securing it is better. 

\* \*\*The Logic:\*\* Instead of encrypting the text (which destroys Huffman's ability to find patterns), YZIP waits until the file is fully compressed into raw binary bytes. Then, it applies an \*\*in-place XOR cipher\*\* using a user-provided password.

\* \*\*The Result:\*\* The final `.yzip` file is absolute mathematical gibberish unless you have the exact key.



\---



\## 💻 Tech Stack

\* \*\*Backend:\*\* `C++17` (STL Priority Queues, Hash Maps, Bitwise Operations, Binary File I/O)

\* \*\*Frontend:\*\* `Python 3` (Tkinter, OS Pathing, Subprocess threading)



\---



\## 🛠️ Installation \& Usage



\### Step 1: Compile the Engine

You need a C++ compiler (like `g++`) to build the core backend. Open your terminal and run:

`g++ zipper.cpp -o zipper.exe`



\### Step 2: Boot up the GUI

Make sure you have Python installed, then launch the frontend:

`python gui.py`



\### Step 3: Squish It

1\. Click \*\*Browse\*\* and select the file you want to compress.

2\. (Optional) Enter a password to lock the file with XOR encryption.

3\. Hit \*\*Compress\*\*. The tool will spit out a heavily condensed `.yzip` file in the same directory.

4\. To reverse the process, just feed the `.yzip` file back into the decompressor with the correct password.

