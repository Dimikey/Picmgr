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
// random pantyhose stockings nekomimi
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
    struct database_entry* last_entry; // just in case
};


struct database_entry {
    unsigned int id;
    char* filename;
    char* tags;
    unsigned char rating;
    
    struct database_entry* next_entry;
    struct database_entry* prev_entry;
};

/**
    Adds a new entry at the end of the database (two-way linked list)
 */
void database_add_entry(struct database* db, const char* picfilename) {
    assert(db);
    assert(picfilename);
    
    struct database_entry* entry = malloc(sizeof(struct database_entry));
    entry->id = db->num_entries;
    entry->rating = 0;
    entry->next_entry = NULL; // Two-way linked list terminator
    entry->prev_entry = NULL; // This one too
    
    // Database is empty
    if(db->first_entry == NULL) {
        // Our first entry is the first and the last
        db->first_entry = entry;
        db->last_entry = entry;
    }
    // Database is saturated
    else {
        
    }
    
    
    db->num_entries++;
}


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
                    
                    printf("DT_REGULAR: %s\n", dirent_struct->d_name);
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


void database_delete_entry_by_id(struct database* db, unsigned int id) {
    
    // TODO: deleting by id will corrupt entries id uniquiness (it'll be possible to have multiple entries with same ids in the future). Will need to reassign ids later
    
    // free(entry->filename);
    // free(entry->tags);
    
    db->num_entries--;
}


struct database_entry* database_get_next_entry(struct database* db) {
    return NULL;
}


struct database_entry* database_get_entry_by_id(unsigned int id) {
    return NULL;
}

/**
 Save database contents to file
 */
void database_save_to_file(struct database* db, const char* filename) {
    
}

/**
 Clean up
 */
void database_destroy(struct database* db) {
    
    // some clean up goes here
    
    // Must free all db entries
    
    // and propably some other data
    
    free(db);
}

struct database* db;

int main(int argc, char* argv[]) {
    
    db = database_init();
    assert(db);

    
    
    
    
    // Clean up
    database_destroy(db);

    return 0;
}

