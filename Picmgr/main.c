//
//  main.c
//  Picmgr
//
//  A CLI manager for your local storage of image board tagged pictures
//  (yande.re, konachan.com and probably some others are acceptable)
//
//  Created by Dimikey on 04.02.2023.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0

void error(const char* msg) {
    printf("ERROR: %s\n", msg);
    exit(-1);
}


/* Database entry
// filename -- tags -- id???

// Possible commands:
// select pantyhose
// random
// random pantyhose
// random pantyhose sweater
// random pantyhose stockings nekomimi thighhighs
// first pantyhose
// first pantyhose uniform
// lastadded
// gettags //???
// tagstat // show most popular tags among the pics
// update
// save
// exit
// quit
//
 (Case 1: select all pics tagged with "pantyhose")
 Input:
 select pantyhose
 Output:
 Found 2 pictures, "yande.re 112233 pantyhose sweater.jpg", "konachan.com - 445566 nekomimi blush.jpg"
 
 (Case 2: get random picture from db)
 Input:
 random
 Output:
 Random picture, "yande.re 778899 uniform tail.jpg"
 
 (Case 3: request a non-existent pic)
 Input:
 select wowsuchtag wowsuchnonexistent
 Output:
 Nothing found by your request!
 
*/
// Still have in mind implementing some SDL2 based GUI to display the pics
// and probably with some interesting stuff
//
// And also: AI based tagging??? wow take it easy, implement the basic stuff first

struct database {
    unsigned int num_entries;
    unsigned int current_entry_id;
    //struct database_entry* array_entries;
    
    struct database_entry* first_entry;
    struct database_entry* last_entry;
    
    struct database_entry* current_entry; // Current entry being manipulated
};


struct database_entry {
    unsigned int id;
    char* filename;
    char* tags;
    unsigned char rating;
    
    struct database_entry* next_entry;
    struct database_entry* prev_entry;
};


struct database* database_init(void);
void database_add_entry(struct database* db, const char* picfilename);
struct database_entry* database_get_next_entry(struct database* db);
struct database_entry* database_get_prev_entry(struct database* db);
struct database_entry* database_get_entry_by_id(unsigned int id);
int database_save_to_file(struct database* db, const char* filename);
int database_destroy(struct database* db);


/**
    Init a database object from folder of pictures OR open existing database
 */
struct database* database_init() {
    
    // TODO: add arg with filename
    // Check if there's already a database file exists
    // TODO: do check
    // If no database file found, then scan directory
    
    struct database* db = malloc(sizeof(struct database));
    assert(db);
    db->num_entries = 0;
    db->current_entry_id = 0;
    db->first_entry = NULL;
    db->last_entry = NULL;
    db->current_entry = NULL;
    
    DIR* picdir = opendir("/Users/dimikey/Documents/Projects/pictures");
    assert(picdir);
    struct dirent* dirent_struct;
    
    do {
        dirent_struct = readdir(picdir);
        
        if(dirent_struct != NULL) {
            
            switch(dirent_struct->d_type) {
                case DT_REG:
                    // Regular file
                    // Check whether it's a picture or not
                    // then add to db
                    database_add_entry(db, dirent_struct->d_name);
                    
                    //printf("DT_REGULAR: %s\n", dirent_struct->d_name);
                    break;
                case DT_DIR:
                    printf("DT_DIR: %s\n", dirent_struct->d_name);
                    break;
                case DT_UNKNOWN:
                    printf("DT_UNKNOWN: %s\n", dirent_struct->d_name);
                    break;
                default:
                    printf("unhandled type: %s\n", dirent_struct->d_name);
                    break;
            }
               
            
            
        }
    }
    while( dirent_struct != NULL );
    
    closedir(picdir);
    
    return db;
}


/**
    Adds a new entry at the end of the database (two-way linked list)
 */
void database_add_entry(struct database* db, const char* picfilename) {
    
    assert(db);
    assert(picfilename);
    
    struct database_entry* e = malloc(sizeof(struct database_entry));
    e->id = db->num_entries;
    e->rating = 0;
    e->next_entry = NULL; // Two-way linked list terminator
    e->prev_entry = NULL; // This one too
    
    /*
     Entry ordering stuff
     */
    
    // Database is empty
    if(db->first_entry == NULL || db->last_entry == NULL) {
        // Our first entry is the first and the last
        db->first_entry = e;
        db->last_entry = e;
    }
    // Database is saturated
    else {
        db->last_entry->next_entry = e;
        e->prev_entry = db->last_entry;
        db->last_entry = e;
    }
    
    /*
     String handling stuff
     */
    
    // Copy filename
    unsigned long filename_len = strlen(picfilename);
    e->filename = malloc(filename_len + 1);
    strcpy(e->filename, picfilename);
    
    // Extract tags
    const char* tags = "test tags pantyhose thighhighs"; // dummy tags
    e->tags = malloc(strlen(tags) + 1);
    strcpy(e->tags, tags);
    
    // If it's the first entry, set it to current
    if(db->current_entry == NULL)
        db->current_entry = e;
    
    db->num_entries++;
    
    printf("Added new entry with filename: %s\nTags:%s\n\n", e->filename, e->tags);
}

void database_delete_entry(struct database* db, struct database_entry* e) {
    
    assert(db);
    assert(e);
    
    
    /*
     Handling ties between entries (two-linked list pointers)
     */
    
    // Deleting between 2 existing entries
    if(e->next_entry != NULL && e->prev_entry != NULL)
    {
        e->next_entry->prev_entry = e->prev_entry;
        e->prev_entry->next_entry = e->next_entry;
    }
    // Deleting at the beginning
    else if(e->next_entry != NULL)
        e->next_entry->prev_entry = e->prev_entry; // should be NULL in practice
    // Deleting at the end of the list
    else if(e->prev_entry != NULL)
        e->prev_entry->next_entry = e->next_entry; // should be NULL in practice
    else {
        printf("database_delete_entry() tie management gone wrong... Or attempting to delete the last remaining entry\n");
        return;
    }
        
    // Check whether we're deleting current entry in db
    // Reset to first in case it is
    if(e == db->current_entry)
        db->current_entry = db->first_entry;
    
    free(e->filename);
    free(e->tags);
    free(e);
}


/**
 Scans entire database and deletes first entry, which has id equal to requested id.
 
 Returns: 1 on success, NULL on error
 (entry was not found, database is already empty, and etc.)
 */
void database_delete_entry_by_id(struct database* db, unsigned int id) {
    
    // TODO: deleting by id will corrupt entries id uniquiness (it'll be possible to have multiple entries with same ids in the future). Will need to reassign ids later
    
    
    
    db->num_entries--;
}


/**
 Returns current entry and changes pointer to the next one
 
 Returns: entry object or NULL if no objects left
 */
struct database_entry* database_get_next_entry(struct database* db) {
    if(db->current_entry == NULL)
        return NULL;
    
    struct database_entry* cur = db->current_entry;
    db->current_entry = db->current_entry->next_entry;
    
    return cur;
}


struct database_entry* database_get_prev_entry(struct database* db) {
    printf("database_get_prev_entry() is not implemented yet.\n");
    return NULL;
}


struct database_entry* database_get_entry_by_id(unsigned int id) {
    printf("database_get_entry_by_id() is not implemented yet.\n");
    return NULL;
}

/**
 Save database contents to file
 */
int database_save_to_file(struct database* db, const char* filename) {
    printf("database_save_to_file() is not implemented yet.\n");
    return 0;
}

/**
 Clean up
 */
int database_destroy(struct database* db) {
    
    // some clean up goes here
    
    // Must free all db entries
    
    // and propably some other data
    
    free(db);
    
    return 0;
}

struct database* db;

int main(int argc, char* argv[]) {
    
    db = database_init();
    assert(db);

    database_get_next_entry(db);
    
    
    
    // Clean up
    database_destroy(db);

    return 0;
}

