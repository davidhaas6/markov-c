#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER 60 // The max length for unknown strings

// A "linked dictionary" that contains each of the following words and how often
// they follow after the key word
typedef struct word_node {
  struct word_node * next;
  char word[BUFFER];
  int num_occurrences;
} word_node;

// A node in a cumulative distribution function array for the word_node list
typedef struct cdf_node {
  struct cdf_node * next;
  char * word;
  int sum_range;
} cdf_node;

// A linked dictionary containing every word and a list of their following words
typedef struct dict_entry {
  struct dict_entry * next;
  char root_word[BUFFER];
  word_node * seq_words_head;
} dict_entry;

int index_word_node(char * word, word_node * head);
int index_dict_entry(char * word, dict_entry * head);
void free_word_list(word_node * head);
void print_dict_keys(dict_entry * head);
void print_word_node_list(word_node * head);
void print_roots_and_nodes(dict_entry * head);
void add_node(word_node * new_node, word_node * head);
char * remove_last_dict_entry(dict_entry * head);
char * replace(char * str, char target_char, char new_char);
char * generate_sentence(char * starting_word, int max_length, dict_entry * words_head);
word_node * create_node(char * word);
word_node * find_word_node(char * word, word_node * head);
word_node * get_word_nodes_list_indx(int index, dict_entry * head);
word_node * get_word_nodes_list_word(char * word, dict_entry * head);


int main() {
  char * file_path = "./text.txt";
  FILE * file_pointer;
  dict_entry * words_head;  // The head for the dictionary of words
  srand(time(NULL));  // Inits the random number generator

  // Opens the source file as file_pointer
  if((file_pointer = fopen(file_path, "r"))== NULL) {
    printf("\nError opening file!");
    return 1;
  }

  // Two temp strings to store input words
  char word[BUFFER], next_word[BUFFER];
  int num_words_read = fscanf(file_pointer, "%s %s", word, next_word);
  dict_entry * current_word = malloc(sizeof(dict_entry));  // The last word in the word dict_entry
  word_node * following_word;
  word_node * seq_head;
  int dict_entry_index;

  // Ensures there's enough space to create a new dict_entry
  if (current_word == NULL) {
    return 1;
  }

  // Lets the program know that current_word is currently the last word in the dict_entry
  current_word->next = NULL;

  // Sets head pointer as the current word
  words_head = current_word;
  while(num_words_read == 2){
    //TODO: Use replace() to remove extraneous characters (clean the input)

    if((dict_entry_index = index_dict_entry(word, words_head)) == -1) { // If word isn't in dict

      strcpy(current_word->root_word , word);
      current_word->seq_words_head = create_node(next_word);

      // Moves onto the next pointer
      // TODO: Currently this creates an extra pointer at the end of the list. fix it.
      current_word->next = malloc(sizeof(dict_entry));
      current_word = current_word->next;
      current_word->next = NULL;
  } else {
    // Gets the head pointer for the followin words linked dict
    seq_head = get_word_nodes_list_indx(dict_entry_index, words_head);
    following_word = find_word_node(next_word, seq_head);

    if (following_word == NULL) { // If this is the first instance of next_word following word
      add_node(create_node(next_word), seq_head);
    } else {
      following_word->num_occurrences++;
    }
  }

  // TODO: Implement a more robust system where you don't have to rewind
  // Moves the cursor where the program reads the file back a word so next_word can be read
  // as word in the next iteration of the loop.
  fseek(file_pointer, -strlen(next_word), SEEK_CUR);

  // Reads the next two words in the txt file
  num_words_read = fscanf(file_pointer, "%s %s", word, next_word);
}
remove_last_dict_entry(words_head);  // The dict always ends with a blank space
// print_dict_keys(words_head);
word_node * h = get_word_nodes_list_indx(10, words_head);
print_word_node_list(h);
generate_sentence("very", 10, words_head);
fclose(file_pointer);
return 0;
}


/*
 * Utilizes Markov Chains to generate a sentence from the dict_entries given a
 * maximum length.
 */
char * generate_sentence(char * starting_word, int max_length, dict_entry * words_head){
  //TODO: FILL
  word_node * current = get_word_nodes_list_word(starting_word, words_head);
  if (current == NULL) {
    printf("\nERROR: STARTING WORD NOT IN DICT");
    return NULL;
  }

  int weights_sum = 0;
  cdf_node * cdf_head;
  cdf_node * cdf_curr = malloc(sizeof(cdf_node));
  if (cdf_curr == NULL) {
    printf("\nERROR ALLOCATING MEMORY FOR cdf_curr");
    return NULL;
  }
  cdf_head = cdf_curr;

  // Creates a cumulative distribution function (CDF) array so a weighted random word
  // can be picked; ergo, each word should be picked with the same percentage that
  // that word follows after the root word. This is done through creating an array
  // of what is essentially intervals ([1,5,6,8,15]), and if the random number falls
  // into a certain interval, the word corresponding with that interval is chosen.
  while (current != NULL) {
    weights_sum += current->num_occurrences;
    cdf_curr->word = current->word;
    cdf_curr->sum_range = weights_sum - 1;  // Sets the CDF range
    if(current->next != NULL) {  // If this isn't the last element in the list
      cdf_curr->next = malloc(sizeof(cdf_node));
      cdf_curr = cdf_curr->next;
    }
    cdf_curr->next = NULL;

    current = current->next;
  }

  cdf_curr = cdf_head;
  printf("\n{");
  while(cdf_curr != NULL) {
    printf("(%i, %s),  ", cdf_curr->sum_range, cdf_curr->word);
    cdf_curr = cdf_curr->next;
  }
  printf("}\n");

  int random = rand() % weights_sum;  // Random number from 0 to weights_sum
  printf("%i", random);
  cdf_curr = cdf_head;
  int prev_val = -1;
  while(cdf_curr != NULL) {
    if(prev_val < random && random <= cdf_curr->sum_range){
      printf("\n%s", cdf_curr->word);
      break;
    }
    prev_val = cdf_curr->sum_range;
    cdf_curr = cdf_curr->next;
  }

  return "xd";
}


/*
* Returns the index of a value in the key-values of the word dictionary
* Returns -1 if the key is not contained in the dictionary
*/
int index_dict_entry(char * key_word, dict_entry * head) {
  dict_entry * current = head;
  int index = 0;

  while(current != NULL) {
    // If the current word matches key_word, return the index
    if(strcmp(current->root_word, key_word) == 0){
      return index;
    }
    index++;
    current = current->next;
  }
  return -1;
}


/*
* Returns the index of a value in the key-values of the word_node linked dict
* Returns -1 if the key is not contained in the word_node linked dict
*/
int index_word_node(char * key_word, word_node * head) {
  word_node * current = head;
  int index = 0;

  while(current != NULL) {
    // If the current word matches key_word, return the index
    if(strcmp(current->word, key_word) == 0){
      return index;
    }
    index++;
    current = current->next;
  }
  return -1;
}


/*
 * Removes the last entry of the main dict and returns its root_word
 */
char * remove_last_dict_entry(dict_entry * head) {
  char *retval;

  // If there's only one entry
  if(head->next == NULL){
    retval = head->root_word;
    free(head);
    return retval;
  }

  // Gets to the second to last entry
  dict_entry * current = head;
  while(current->next->next!=NULL){
    current = current->next;
  }

  // Frees the last entry and sets the next to NULL
  retval = current->next->root_word;
  free_word_list(current->next->seq_words_head);
  free(current->next);
  current->next = NULL;
  return retval;
}


/*
 * Recursively deletes the word_node linked list
 */
 void free_word_list(word_node * head) {
   if(head == NULL) {  // Ensures that there is a words list
     return;
   }
   else if(head->next != NULL){
     free_word_list(head->next);  // Goes to the end of the list
   }
   free(head);
 }


/*
* Creates a new word_node for a given word
*/
word_node * create_node(char * word){
  word_node * new_node = malloc(sizeof(word_node));
  strcpy(new_node->word, word);
  new_node->num_occurrences = 1;
  new_node->next = NULL;
  return new_node;
}


/*
 * Returns a pointer to the head node of the following words linked list
 * given an index in the dict_entry list
 */
word_node * get_word_nodes_list_indx(int index, dict_entry * head){
  dict_entry * current = head;

  // Iterates to the ith index in the dict
  int i;
  for(i=0; i<index; i++) {
    if(current==NULL) {  // Ensures the index isn't greater than the length of the dict
      printf("\nERROR: INDEX OUT OF BOUNDS\n");
      return NULL;
    }
    current = current->next;
  }
  return current->seq_words_head;
}


/*
 * Returns a pointer to the head node of the following words linked list
 * given a word in the dict_entry list
 */
word_node * get_word_nodes_list_word(char * word, dict_entry * head){
  dict_entry * current = head;

  while(current != NULL){
    if(strcmp(current->root_word, word) == 0){
      return current->seq_words_head;
    }
    current = current->next;
  }
  printf("\nERROR: WORD \'%s\' NOT FOUND", word);
  return NULL;
}

/*
 * Returns a pointer to the word_node corresponding with a certain word
 */
word_node * find_word_node(char * word, word_node * head){
  word_node * current = head;

  while(current != NULL){
    if(strcmp(current->word, word) == 0){
      return current;
    }
    current = current->next;
  }

  return NULL;
}


/*
* Adds a word_node to the end of the linked list
*/
void add_node(word_node * new_node, word_node * head) {
  word_node * current = head;
  while(current->next != NULL){
    current = current->next;
  }
  current->next = new_node;
}


/*
 * Replaces a given character in a string with another character
 */
char * replace(char * str, char target_char, char new_char) {
  // Dynamically allocates space for the new string with the same length
  char * new_str = malloc((strlen(str)+1) * sizeof(char));
  strcpy(new_str, str);

  int i;
  for(i = 0; i < strlen(str); i++) {
    if(str[i] == target_char){
      new_str[i] = new_char;
    }
  }
  free(str);
  return new_str;
}


/*
* Prints the key values of the dictionary
*/
void print_dict_keys(dict_entry * head){
  dict_entry * current = head;
  printf("\n[");
  while (current->next != NULL){
    printf("%s, ", current->root_word);
    current = current->next;
  }
  printf("%s]", current->root_word);
}


/*
* Prints the word_node linked list
*/
void print_word_node_list(word_node * head){
  word_node * current = head;
  printf("\n[");
  while (current->next != NULL){
    printf("(%s, %i), ", current->word, current->num_occurrences);
    current = current->next;
  }
  printf("(%s, %i)]\n", current->word, current->num_occurrences);
}


/*
 * Prints all root_words and their respective nodes
 */
 void print_roots_and_nodes(dict_entry * head) {
   dict_entry * current = head;
   printf("\n{");
   while (current->next != NULL){
     printf("\n  %s:", current->root_word);

     word_node * current_node = current->seq_words_head;
     printf("\n    [");
     while (current_node->next != NULL){
       printf("(%s, %i), ", current_node->word, current_node->num_occurrences);
       current_node = current_node->next;
     }
     printf("(%s, %i)]", current_node->word, current_node->num_occurrences);

     current = current->next;
   }
   printf("\n}");
 }
