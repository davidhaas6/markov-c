#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 25

// A "linked dict_entryionary" that contains each of the following words and how often
// they follow after the key word
typedef struct word_node {
  struct word_node * next;
  char word[BUFFER];
  int num_occurrences;
} word_node;

// A linked dict_entryionary containing every word and a list of their following words
typedef struct dict_entry {
  struct dict_entry * next;
  char root_word[BUFFER];
  word_node * seq_words_head;
} dict_entry;

int index_dict_entry(char * word, dict_entry * head);
int index_word_node(char * word, word_node * head);
void print_dict_keys(dict_entry * head);
void print_word_node_list(word_node * head);
void add_node(word_node * new_node, word_node * head);
char * remove_last_dict_entry(dict_entry * head);
word_node * get_following_word_nodes_list(int index, dict_entry * head);
word_node * get_following_word_node(char * word, word_node * head);
word_node * create_node(char * word);


int main() {
  char * file_path = "./text.txt";
  FILE * file_pointer;
  dict_entry * words_head; // The head for the dict_entryionary of words

  // Opens the source file as file_pointer
  if((file_pointer = fopen(file_path, "r"))== NULL) {
    printf("Error opening file!");
    return 1;
  }

  char word[BUFFER], next_word[BUFFER]; // Two temp strings to store input words
  int num_words_read = fscanf(file_pointer, "%s %s", word, next_word);
  dict_entry * current_word = malloc(sizeof(dict_entry)); // The last word in the word dict_entry
  word_node * following_word;
  word_node * seq_head;
  int dict_entry_index;

  // Ensures there's enough space to create a new dict_entry
  if (current_word == NULL) {
    return 1;
  }

  // Lets the program know that current_word is currently the last word in the dict_entry
  current_word->next = NULL;

  // Sets head pointer as the curernt word
  words_head = current_word;

  while(num_words_read == 2){
    printf("\n\n-----------\n%s %s", word, next_word);
    if((dict_entry_index = index_dict_entry(word, words_head)) == -1) { // If word isn't in dict
      strcpy(current_word->root_word , word);
      current_word->seq_words_head = create_node(next_word);
      printf("\n**** %s", current_word->seq_words_head->word);
      printf("\nseq_words_head->word lives at %p.", (void*)&current_word->seq_words_head->word);

      // Moves onto the next pointer
      // TODO: Currently this creates an extra pointer at the end of the list. fix it.
      current_word->next = malloc(sizeof(dict_entry));
      current_word = current_word->next;
      current_word->next = NULL;
  } else {
    // Gets the head pointer for the followin words linked dict
    seq_head = get_following_word_nodes_list(dict_entry_index, words_head);
    following_word = get_following_word_node(next_word, seq_head);

    if (following_word == NULL) { // If this is the first instance of next_word following word
      add_node(create_node(next_word), seq_head);
    } else {
      following_word->num_occurrences++;
    }
    print_word_node_list(seq_head);
  }

  // TODO: Implement a more robust system where you don't have to rewind
  // Moves the cursor where the program reads the file back a word so next_word can be read
  // as word in the next iteration of the loop.
  fseek(file_pointer, -strlen(next_word), SEEK_CUR);

  // Reads the next two words in the txt file
  num_words_read = fscanf(file_pointer, "%s %s", word, next_word);
}
remove_last_dict_entry(words_head);
print_dict_keys(words_head);
fclose(file_pointer);
return 0;
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

char * remove_last_dict_entry(dict_entry * head) {
  char *retval;

  if(head->next == NULL){
    retval = head->root_word;
    free(head);
    return retval;
  }

  dict_entry * current = head;
  while(current->next->next!=NULL){
    current = current->next;
  }

  retval = current->next->root_word;
  free(current->next);
  current->next = NULL;
  return retval;
}

/*
* Creates a new word_node
*/
word_node * create_node(char * word){
  word_node * new_node = malloc(sizeof(word_node));
  strcpy(new_node->word, word);
  new_node->num_occurrences = 1;
  new_node->next = NULL;
  return new_node;
}

/*
* Returns a pointer to the head node of the following words linked dict_entry
*/
word_node * get_following_word_nodes_list(int index, dict_entry * head){
  dict_entry * current = head;
  int i;
  for(i=0; i<index; i++) {
    if(current==NULL){
      printf("\nERROR: INDEX OUT OF BOUNDS\n");
      return NULL;
    }
    current = current->next;
  }
  return current->seq_words_head;
}

word_node * get_following_word_node(char * word, word_node * head){
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
  printf("(%s, %i)]", current->word, current->num_occurrences);
}
