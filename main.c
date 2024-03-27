#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 4096

enum Status {
    EMPTY,  // for when there is no input (zero means non-initialized, 
            // i.e. we've not read any input for this list index from the todo file)
            // important that this is the default state!
    OPEN,
    WORKING,
    COMPLETE
};

struct todo_item {
    enum Status status;
    char* description;
};

struct todo_item* read_todo_file(FILE* fp);
void print_todo_items(struct todo_item* items);
void save_todo_items(char* filename, struct todo_item* items);

char* instructions = "\nUSAGE:\ntodo -- show open tasks\ntodo add (task description) -- add task (no quotes neccessary)\ntodo set (task number) (open/working/complete)\ntodo delete (task number)\n";

// todo file format:
/*
status, description
status: open, working, complete
description: String with a short description
*/

// Actions:
/*
add task
set (task number) (open/working/complete)
delete (task number) (have are you sure prompt?)

NOTE: task number has one indexing (starts with 1)
*/
int main(int argc, char** argv) {
    FILE* todo_file;
    char filename[256];
    strcat(strcpy(filename, getenv("HOME")), "/.todo.csv");

    todo_file = fopen(filename, "r");

    if (todo_file == NULL) {
        printf("Error opening file: %s\n", filename);
        return 1;
    }

    struct todo_item* items = read_todo_file(todo_file);


    if (argc >= 3 && strcmp(argv[1], "add") == 0) {

        // Concatenate the input arguments!
        int total_length = 0;
        for (int i = 2; i < argc; i++) {
            total_length += strlen(argv[i]);
        }

        // Add space for the separator characters
        total_length += (argc - 2) - 1; // 1 space between each string, except the last one.

        char *result = malloc(1024 * sizeof(char));
        int index = 0;

        // Concatenate strings with spaces in between
        for (int i = 2; i < argc; i++) {
            strcpy(&result[index], argv[i]);
            index += strlen(argv[i]);
            if (i < argc - 1) { // If not the last string
                result[index++] = ' ';
            }
        }
        // no error checking for index not exceeding 1024!

        result[index] = '\0';


        int i = 0;
        while (items[i].status != EMPTY) i++;
        items[i].status = OPEN;
        items[i].description = result;
    }
    if (argc == 4 && strcmp(argv[1], "set") == 0) { 
        // add support for modifying multiple tasks in one command in the future?
        int i;
        if (sscanf(argv[2], "%d", &i) != 1) {
            printf("Could not parse task number!\n");
        }
        else if (i < 1 || i > MAX_ITEMS || items[i-1].status == EMPTY) {
            printf("Cannot modify task that does not exist");
        } 
        else if (strcmp(argv[3], "open") == 0) items[i-1].status = OPEN;
        else if (strcmp(argv[3], "working") == 0) items[i-1].status = WORKING;
        else if (strcmp(argv[3], "complete") == 0) items[i-1].status = COMPLETE;
        else printf("Status %s not recognized.\n", argv[3]);
    }
    if (argc == 3 && strcmp(argv[1], "delete") == 0) {
        int i;
        if (sscanf(argv[2], "%d", &i) != 1) {
            printf("Could not parse task number!\n");
        }
        else if (i < 1 || i > MAX_ITEMS || items[i-1].status == EMPTY) {
            printf("Cannot delete task that does not exist!\n");
        }
        else {
            printf("Do you want to delete task \"%s\" [y/n]? ", items[i-1].description);
            if ('y' == getc(stdin)) {
                items[i-1].status = EMPTY;
            }
        }
    }

    // always print out tasks at in the final state
    print_todo_items(items);
    // printf(instructions);

    fclose(todo_file);
    save_todo_items(filename, items); // overwrites the file!

    for (int i = 0; i < MAX_ITEMS; i++) {
        free(items[i].description);
    }
    free(items);

    return 0;
}

struct todo_item* read_todo_file(FILE* fp) {
    struct todo_item* items = calloc(MAX_ITEMS, sizeof(struct todo_item));
    char description[1024];
    int i = 0;
    while (fscanf(fp, "%d, \"%[^\"]\"", &(items[i].status), description) == 2) {
        items[i].description = malloc(1024 * sizeof(char));
        strcpy(items[i].description, description);
        i++;
    }

    return items;
}

void print_todo_items(struct todo_item* items) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (items[i].status != EMPTY) {
            char *status_string;
            switch (items[i].status) {
                case OPEN:
                    status_string = "OPEN";
                    break;
                case WORKING:
                    status_string = "WORKING";
                    break;
                case COMPLETE:
                    status_string = "COMPLETE";
                    break;
                default:
                    printf("Shit is wrong with the status!\n");
                    return;
            }

            printf("%4d.   %-10s %s\n", i+1, status_string, items[i].description);
        }
    }
}

void save_todo_items(char* filename, struct todo_item* items) {
    FILE* fp = fopen(filename, "w");
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (items[i].status != EMPTY) {
            fprintf(fp, "%d, \"%s\"\n", items[i].status, items[i].description);
        }
    }
    fclose(fp);
    return;
}

