/*
 * File: status.c
 * Description: Implementation of the 'status' command for GOAT.
 *              Scans the working directory and index to determine file states.
 *              Displays categorized lists of untracked, modified, and staged files.
 * Date: 13/11/2025
 * Author: Aliago
 */

#define _POSIX_C_SOURCE 200809L

#include "commands/status/status.h"
#include "utils/fs.h"
#include "ui/messages.h"
#include "core/command.h"
#include "core/hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define GOAT_DIR ".goat"
#define INDEX_FILE ".goat/index"

// Forward declarations of handlers
void set_short_format(cmd_opts_t *opts);
void set_verbose(cmd_opts_t *opts);

// Option definition for status command
static const option_entry_t status_options[] = {
    {"-s", "short format", set_short_format},
    {"-v", "complete format", set_verbose}
};

// Command options structure
static const command_options_t status_cmd_opts = {
    .command_name = "status",
    .options = status_options,
    .num_options = sizeof(status_options)/sizeof(status_options[0])
};

// ============ File List Management ============

file_entry_t *create_file_entry(const char *path, file_status_t status)
{
    if (!path) return NULL;
    
    file_entry_t *entry = malloc(sizeof(file_entry_t));
    if (!entry) return NULL;
    
    entry->path = strdup(path);
    if (!entry->path) {
        free(entry);
        return NULL;
    }
    
    entry->status = status;
    entry->next = NULL;
    return entry;
}

void free_file_list(file_entry_t *head)
{
    while (head) {
        file_entry_t *tmp = head;
        head = head->next;
        free(tmp->path);
        free(tmp);
    }
}

void add_file_entry(file_entry_t **head, const char *path, file_status_t status)
{
    if (!head || !path) return;
    
    file_entry_t *new_entry = create_file_entry(path, status);
    if (!new_entry) return;
    
    // Insert at head (simple)
    new_entry->next = *head;
    *head = new_entry;
}

// ============ Index Reading (Simple) ============

static char *normalize_path(const char *path)
{
    // Remove leading "./" if present
    if (path[0] == '.' && path[1] == '/') {
        return strdup(path + 2);
    }
    return strdup(path);
}

static char *get_file_hash_from_index(const char *path)
{
    FILE *f = fopen(INDEX_FILE, "r");
    if (!f) return NULL;
    
    char *normalized = normalize_path(path);
    if (!normalized) {
        fclose(f);
        return NULL;
    }
    
    char line[4096];
    char *result = NULL;
    
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        
        // Format: "path hash timestamp"
        char indexed_path[4096];
        char hash[256];
        if (sscanf(line, "%s %s", indexed_path, hash) == 2) {
            if (strcmp(indexed_path, normalized) == 0) {
                result = strdup(hash);
                break;
            }
        }
    }
    
    fclose(f);
    free(normalized);
    return result;
}

static bool is_in_index(const char *path)
{
    char *hash = get_file_hash_from_index(path);
    if (hash) {
        free(hash);
        return true;
    }
    return false;
}

// Simple hash computation using project's hash function
static char *compute_file_hash(const char *path)
{
    // Normalize path before hashing (remove leading ./)
    char *normalized = normalize_path(path);
    if (!normalized) return NULL;
    
    // Allocate buffer for hex hash (SHA-256 = 64 hex chars + null terminator)
    char *hex_hash = malloc(65);
    if (!hex_hash) {
        free(normalized);
        return NULL;
    }
    
    // Use your project's hash_file_to_hex function
    int result = hash_file_to_hex(normalized, hex_hash);
    
    free(normalized);
    
    if (result != 0) {
        free(hex_hash);
        return NULL;
    }
    
    return hex_hash;
}

static bool is_file_modified(const char *path)
{
    char *index_hash = get_file_hash_from_index(path);
    if (!index_hash) return false;
    
    char *current_hash = compute_file_hash(path);
    if (!current_hash) {
        free(index_hash);
        return false;
    }
    
    bool modified = (strcmp(index_hash, current_hash) != 0);
    
    free(index_hash);
    free(current_hash);
    return modified;
}

// ============ Directory Scanning ============

static bool should_ignore(const char *name)
{
    return strcmp(name, ".") == 0 || 
           strcmp(name, "..") == 0 ||
           strcmp(name, GOAT_DIR) == 0;
}

static int scan_directory(const char *dir_path, file_entry_t **file_list)
{
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("opendir");
        return -1;
    }
    
    struct dirent *entry;
    char path_buf[4096];
    
    while ((entry = readdir(dir)) != NULL) {
        if (should_ignore(entry->d_name)) continue;
        
        // Build full path
        int n = snprintf(path_buf, sizeof(path_buf), "%s/%s", 
                        dir_path, entry->d_name);
        if (n >= (int)sizeof(path_buf)) {
            fprintf(stderr, "Warning: path too long, skipping: %s/%s\n",
                    dir_path, entry->d_name);
            continue;
        }
        
        struct stat st;
        if (stat(path_buf, &st) == -1) continue;
        
        if (S_ISDIR(st.st_mode)) {
            // Recursively scan subdirectories
            scan_directory(path_buf, file_list);
        } else if (S_ISREG(st.st_mode)) {
            // Determine file status
            file_status_t status;
            
            if (is_in_index(path_buf)) {
                // File is tracked - check if modified
                if (is_file_modified(path_buf)) {
                    status = FILE_MODIFIED;
                } else {
                    status = FILE_UNMODIFIED;
                }
            } else {
                status = FILE_UNTRACKED;
            }
            
            // Show untracked and modified files
            if (status == FILE_UNTRACKED || status == FILE_MODIFIED) {
                add_file_entry(file_list, path_buf, status);
            }
        }
    }
    
    closedir(dir);
    return 0;
}

// ============ Status Display ============

static void print_file_list(file_entry_t *head, const char *category_name, 
                           const char *color, bool short_format)
{
    if (!head) return;
    
    if (!short_format) {
        printf("%s%s:%s\n", color, category_name, COLOR_RESET);
    }
    
    for (file_entry_t *curr = head; curr; curr = curr->next) {
        if (short_format) {
            char status_code[3] = "??";
            if (curr->status == FILE_MODIFIED) strcpy(status_code, " M");
            else if (curr->status == FILE_STAGED) strcpy(status_code, "A ");
            
            printf("%s%s %s%s\n", color, status_code, curr->path, COLOR_RESET);
        } else {
            printf("    %s%s%s\n", color, curr->path, COLOR_RESET);
        }
    }
    
    if (!short_format) {
        printf("\n");
    }
}

static void display_status(file_entry_t *files, cmd_opts_t *opts)
{
    file_entry_t *staged = NULL;
    file_entry_t *modified = NULL;
    file_entry_t *untracked = NULL;
    
    bool short_format = opts->cmd_specific.status.short_format;
    
    // Categorize files
    for (file_entry_t *curr = files; curr; curr = curr->next) {
        switch (curr->status) {
            case FILE_STAGED:
                add_file_entry(&staged, curr->path, FILE_STAGED);
                break;
            case FILE_MODIFIED:
                add_file_entry(&modified, curr->path, FILE_MODIFIED);
                break;
            case FILE_UNTRACKED:
                add_file_entry(&untracked, curr->path, FILE_UNTRACKED);
                break;
            default:
                break;
        }
    }
    
    // Display categorized lists
    if (!short_format) {
        printf("On branch master\n\n");
    }
    
    bool has_changes = (staged || modified || untracked);
    
    print_file_list(staged, "Changes to be committed", 
                   COLOR_GREEN, short_format);
    print_file_list(modified, "Changes not staged for commit", 
                   COLOR_YELLOW, short_format);
    print_file_list(untracked, "Untracked files", 
                   COLOR_RED, short_format);
    
    // Free categorized lists
    free_file_list(staged);
    free_file_list(modified);
    free_file_list(untracked);
    
    if (!short_format && !has_changes) {
        printf("nothing to commit, working tree clean\n");
    }
}

// ============ Option Parsing ============

int parse_status_options(int argc, char **argv, cmd_opts_t *opts)
{
    if (!opts) return -1;
    
    // Initialize status-specific options
    opts->cmd_specific.status.short_format = false;
    opts->cmd_specific.status.branch = false;
    opts->cmd_specific.status.help = false;
    
    // Use generic option parser
    return parse_options(argc, argv, &status_cmd_opts, opts);
}

// ============ Main Command ============

int cmd_status(cmd_opts_t *opts)
{
    if (!opts) return -1;
    
    // Check if we're in a GOAT repository
    if (!fs_dir_exists(GOAT_DIR)) {
        fprintf(stderr, COLOR_RED "Error: Not a GOAT repository\n" COLOR_RESET);
        fprintf(stderr, COLOR_YELLOW "Hint: Run 'goat init' to initialize a repository\n" COLOR_RESET);
        return 1;
    }
    
    file_entry_t *files = NULL;
    
    // Scan current directory
    if (scan_directory(".", &files) < 0) {
        free_file_list(files);
        return 2;
    }
    
    // Display status
    display_status(files, opts);
    
    // Cleanup
    free_file_list(files);
    
    return 0;
}
