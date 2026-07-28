# ResumeIQ

A resume analyzer built in C++ that matches candidate skills against job roles and provides AI-powered learning suggestions through a graphical interface.

## What it does
- Parses resume text and extracts candidate information
- Validates skills using a Trie data structure
- Corrects typos using Edit Distance algorithm
- Scores resume against a target job role
- Generates personalized learning suggestions via Groq AI
- Displays results in a Dear ImGui desktop GUI

## Tech Stack
C++ | Dear ImGui | GLFW | OpenGL | Groq AI | WinHTTP

## Data Structures
- **Trie** — O(m) skill lookup and validation
- **DAG** — Structured learning path generation
- **Edit Distance (DP)** — Typo-tolerant skill matching

## Setup
1. Clone the repo
2. Create `.env` in project root:

GROQ_API_KEY=your_key_here

3. Get free key at console.groq.com

## Run

Terminal version

./resumeiq.exe

GUI version

./gui.exe