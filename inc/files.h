#ifndef __FILES_H_
#define __FILES_H_

#include <dirent.h>

#define OUTPUT_PATH	"./tmp"

enum {
	E_TYPE_FILE,
	E_TYPE_DIR,
	E_TYPE_LINK,
};

struct s_tree_elt{
	char name[PATH_MAX];
	int depth;
};


int is_dir(const char *path);
int is_file(const char *filename);

int get_current_dir(char *filename, char *current_dir);
int get_depth(char *filename);
int get_extension(char *extension, char *string);
int get_short_filename(char *filename, char *short_filename);
int get_short_filename_no_ext(char *filename, char *short_filename);
int remove_extension(char *ext_filename, char *filename);

int get_html_url(char *filename, char *html_filename);

int is_markdown_file(char *filename);
int is_html_file(char *filename);
int is_image_file(char *filename);

int create_dir(const char *dirname);
int cp_file(const char *source, const char *dest);
int cp_dir(const char *source, const char *dest);

int process_tree(struct s_tree_elt *tree);
int process_files(struct s_tree_elt *tree);

#endif
