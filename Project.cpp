#include <iostream>
#include <pthread.h>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <fstream>
using namespace std;

const int max_keys = 100;
const int max_key_length = 50;
const int max_chunks = 100;

pthread_mutex_t mutex;

char chunks[max_chunks][max_key_length * 10]; // Chunks of the input string
char map_output[max_keys][max_key_length];    // Map phase output keys
int map_output_count = 0;                     // Counter for map_output
char shuffled_keys[max_keys][max_key_length]; // Array for unique keys
int unique_key_count = 0;                     // Count of unique keys
int shuffled_occurences[max_keys][max_keys];  // Array for occurrences

// Function to split the input string into chunks
void split_string_into_chunks(const char *input, int no_of_chunks)
{
    char *input_copy = strdup(input);
    char *token;
    int word_count = 0;

    // Count the number of words
    token = strtok(input_copy, " ");
    while (token != NULL)
    {
        word_count++;
        token = strtok(NULL, " ");
    }

    free(input_copy);

    int words_per_chunk = word_count / no_of_chunks;
    int remaining_words = word_count % no_of_chunks;

    input_copy = strdup(input);
    int chunk_index = 0;
    int word_in_chunk = 0;

    token = strtok(input_copy, " ");
    while (token != NULL)
    {
        if (word_in_chunk == 0)
            chunks[chunk_index][0] = '\0';

        strcat(chunks[chunk_index], token);
        strcat(chunks[chunk_index], " ");

        word_in_chunk++;
        if (word_in_chunk == words_per_chunk + (chunk_index < remaining_words ? 1 : 0))
        {
            chunk_index++;
            word_in_chunk = 0;
        }

        token = strtok(NULL, " ");
    }

    free(input_copy);

    cout << "Chunks after splitting:" << endl;
    for (int i = 0; i < no_of_chunks; i++)
        cout << "Chunk " << i + 1 << ": " << chunks[i] << endl;
    cout << endl;
}

// Map phase function
void *mapphase(void *arg)
{
    char *chunk = (char *)arg;

    pthread_mutex_lock(&mutex);

    static bool header_displayed = false;
    if (!header_displayed)
    {
        cout << "Mapping Phase:" << endl;
        header_displayed = true;
    }

    char *token = strtok(chunk, " "); // Tokenize the chunk
    while (token != NULL)
    {
        strcpy(map_output[map_output_count], token); // Store each key occurrence independently
        cout << "(" << token << ",1)" << endl;
        map_output_count++; // to track number of keys independently stored
        token = strtok(NULL, " ");
    }

    pthread_mutex_unlock(&mutex);
    return nullptr;
}
// Shuffle phase function
void shufflephase()
{
    cout << "Shuffle Phase:" << endl;

    for (int i = 0; i < map_output_count; i++)
    {
        bool found = false; // check yo see if key is matched
        for (int j = 0; j < unique_key_count; j++)
        {
            if (strcmp(map_output[i], shuffled_keys[j]) == 0) // if key is matched
            {
                for (int k = 0; k < max_keys; k++)
                {
                    if (shuffled_occurences[j][k] == 0) // if empty slot is available
                    {
                        shuffled_occurences[j][k] = 1; // store 1 in front of it   e.g shuffled_occurance[0][1,1,1,1]  where 0 denotes the index of key it can be pizza , burger anything.
                        break;
                    }
                }
                found = true;
                break;
            }
        }
        if (!found)
        {
            strcpy(shuffled_keys[unique_key_count], map_output[i]); // Add unique key
            shuffled_occurences[unique_key_count][0] = 1;           // add First occurrence of this key
            unique_key_count++;                                     // increment unique key counter
        }
    }

    for (int i = 0; i < unique_key_count; i++)
    {
        cout << shuffled_keys[i] << ": [";
        for (int j = 0; j < max_keys; j++)
        {
            if (shuffled_occurences[i][j] != 0)
            {
                cout << "1";
                if (j < max_keys - 1 && shuffled_occurences[i][j + 1] != 0)
                    cout << ", ";
            }
            else
                break;
        }
        cout << "]" << endl;
    }
    cout << endl;
}

// Reduce phase function

void *reducephase(void *arg)
{
    int index = *(int *)arg;

    int count = 0;
    int i = 0;

    pthread_mutex_lock(&mutex);

    while (shuffled_occurences[index][i] != 0) // run loop for that key till 0 is found in occurance array
    {
        count++;
        i++;
    }

    cout << "Key: " << shuffled_keys[index] << " Count: " << count << endl;
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

// Test case function
void test_mapreduce(string input, int no_of_chunks)
{
    if (input.empty())
    {
        cout << "Test Case: Input is an empty string. No computation performed.\n";
        cout << "------------------------------------------------------------" << endl;
        return;
    }

    map_output_count = 0;
    unique_key_count = 0;
    memset(chunks, 0, sizeof(chunks));
    memset(map_output, 0, sizeof(map_output));
    memset(shuffled_keys, 0, sizeof(shuffled_keys));
    memset(shuffled_occurences, 0, sizeof(shuffled_occurences));

    cout << "Test Case: Input = \"" << input << "\", Chunks = " << no_of_chunks << endl;

    for (size_t i = 0; i < input.length(); i++)
    {
        if (input[i] >= 'A' && input[i] <= 'Z')
        {
            input[i] = input[i] + 32;
        }
    }

    split_string_into_chunks(input.c_str(), no_of_chunks);

    pthread_t map_threads[no_of_chunks];
    for (int i = 0; i < no_of_chunks; i++)
        pthread_create(&map_threads[i], NULL, mapphase, (void *)chunks[i]);

    for (int i = 0; i < no_of_chunks; i++)
        pthread_join(map_threads[i], NULL);

    cout << endl;

    shufflephase();

    pthread_t reduce_threads[unique_key_count];
    int indices[unique_key_count];
    for (int i = 0; i < unique_key_count; i++)
    {
        indices[i] = i;
        pthread_create(&reduce_threads[i], NULL, reducephase, (void *)&indices[i]);
    }

    for (int i = 0; i < unique_key_count; i++)
        pthread_join(reduce_threads[i], NULL);

    cout << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
}

int main()
{
    system("clear");
    pthread_mutex_init(&mutex, NULL);

    // Original main function (unchanged)
    // string input;
    // // Commented out the file reading code

    // string line;
    // ifstream inputFile("input.txt");
    // if (!inputFile)
    // {
    //     cerr << "Error: Unable to open file 'input.txt'." << endl;
    //     return 1;
    // }

    // while (getline(inputFile, line))
    //     input += line + " ";
    // inputFile.close();

    // Ask for user input
    // cout << "Enter the input string: ";
    // getline(cin, input);

    // int no_of_chunks;
    // cout << "Enter the number of chunks: ";
    // cin >> no_of_chunks;
    // cout << endl;

    // // Split the input into chunks
    // split_string_into_chunks(input.c_str(), no_of_chunks);

    // // Map phase
    // pthread_t map_threads[no_of_chunks];
    // for (int i = 0; i < no_of_chunks; i++)
    //     pthread_create(&map_threads[i], NULL, mapphase, (void *)chunks[i]);

    // for (int i = 0; i < no_of_chunks; i++)
    //     pthread_join(map_threads[i], NULL);

    // cout << endl;
    // // Shuffle phase
    // shufflephase();

    // // Reduce phase
    // pthread_t reduce_threads[unique_key_count];
    // int indices[unique_key_count];
    // for (int i = 0; i < unique_key_count; i++)
    // {
    //     indices[i] = i;
    //     pthread_create(&reduce_threads[i], NULL, reducephase, (void *)&indices[i]);
    // }

    // for (int i = 0; i < unique_key_count; i++)
    //     pthread_join(reduce_threads[i], NULL);

    // Test cases(added after the main workflow)

    cout
        << "\nExecuting Test Cases...\n";
    // test_mapreduce("apple banana orange apple banana orange", 2);
    // test_mapreduce("apple apple apple apple apple", 3);
    // test_mapreduce("", 1);
    // test_mapreduce("apple banana orange grape pineapple", 5);
    // test_mapreduce("Apple apple APPLE aPPle", 2);
    // test_mapreduce("apple banana orange apple mango banana mango", 3);
    test_mapreduce("123 1 123 1 abc abc abd @#$$ ABC APple apple", 3);

    pthread_mutex_destroy(&mutex);
    return 0;
}
