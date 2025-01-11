# MapReduce Simulation in C++ with Multithreading

This project implements a basic MapReduce framework using multithreading in C++. It simulates the Map, Shuffle, and Reduce phases of the MapReduce algorithm, demonstrating how concurrent computing and synchronization work in the context of parallel data processing.

The code simulates the breakdown of a large task into smaller, independent sub-tasks that can be processed in parallel. The project uses threads to perform the Map and Reduce phases and applies mutexes to ensure synchronization.

## Project Description

In this project, a string of text is processed to count the occurrences of each unique word using a simulated MapReduce framework. The process is broken down into three main phases:

### 1. **Map Phase**
   - The input string is split into chunks that are processed in parallel.
   - Each chunk is processed by a map function, which outputs key-value pairs of words and their occurrences.

### 2. **Shuffle Phase**
   - The key-value pairs from the map phase are grouped by key. This simulates the shuffle phase where the system groups all occurrences of a particular key together.
   
### 3. **Reduce Phase**
   - For each unique key, the occurrences are aggregated to compute the final count of that key in the input string.

The program processes an input string, counts word frequencies, and outputs the final word count for each unique word.

## How It Works

1. **Input**: A string of space-separated words.
2. **Map Phase**: Each chunk of the input string is processed by a map function that outputs key-value pairs of word occurrences.
3. **Shuffle Phase**: The key-value pairs are grouped by key, simulating the shuffle phase in a distributed environment.
4. **Reduce Phase**: The occurrence of each key is counted and displayed.

### Example:

**Input**: `"pizza burger pasta pasta pizza"`

- After Shuffle Phase:  
  - ("pizza", [1, 1])  
  - ("burger", [1])  
  - ("pasta", [1, 1])

- Final Output:  
  - ("pizza", 2)  
  - ("burger", 1)  
  - ("pasta", 2)

## Features

- Implements **Map** and **Reduce** phases of the MapReduce algorithm using multithreading with `pthread` in C++.
- Synchronization between threads using **mutexes** to ensure thread safety during the Map phase.
- The input string is split into chunks, processed by map threads, shuffled, and then reduced concurrently.
- Supports flexible chunking of input strings to simulate parallel processing.

## Setup

### Prerequisites

To compile and run this project, you will need:

- A C++ compiler (e.g., `g++`)
- The `pthread` library (for multithreading support)


### Example Test Case

The program includes a test case with the input string `"123 1 123 1 abc abc abd @#$$ ABC APple apple"`, which demonstrates the functionality of the MapReduce algorithm with word frequency counting.

```bash
Executing Test Cases...
Test Case: Input = "123 1 123 1 abc abc abd @#$$ ABC APple apple", Chunks = 3
Mapping Phase:
(pizza,1)
(burger,1)
(pasta,1)
Shuffling Phase:
pizza: [1,1]
burger: [1]
pasta: [1,1]
Reducing Phase:
Key: pizza Count: 2
Key: burger Count: 1
Key: pasta Count: 2
```

## Code Walkthrough

1. **split_string_into_chunks()**: This function splits the input string into smaller chunks to be processed by different threads in parallel.

2. **mapphase()**: This function processes each chunk in parallel, tokenizes it into words, and stores the word occurrences.

3. **shufflephase()**: This function groups the output from the Map phase by key, preparing it for the Reduce phase.

4. **reducephase()**: This function aggregates the counts for each unique key and outputs the final count.

5. **test_mapreduce()**: A helper function that tests the entire MapReduce process with different test inputs.

## Contributing

Feel free to fork the repository, create branches, and submit pull requests for improvements. Please ensure to test the changes with new test cases.
