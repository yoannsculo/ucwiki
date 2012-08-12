#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "files.h"

#include "render_html.h" // TODO : move
#include "markup.h"

int is_dir(const char *path)
{
	struct stat sb;

	if (path == NULL)
		return 0;

	if (stat(path, &sb) == -1)
		return 0;

	if ((sb.st_mode & S_IFMT) == S_IFDIR)
		return 1;
	else
		return 0;
}

int is_file(const char *filename)
{
	struct stat sb;

	if (filename == NULL)
		return 0;

	if (stat(filename, &sb) == -1)
		return 0;

	if ((sb.st_mode & S_IFMT) == S_IFREG)
		return 1;
	else
		return 0;
}

/* like "pwd" but with a pilename as input */
int get_current_dir(char *filename, char *current_dir)
{
	char *pstr = NULL;
	int size;

	if (filename == NULL || current_dir == NULL)
		return -1;

	pstr = strrchr(filename, '/');
	
	if (pstr != NULL) {
		size = pstr - filename;
		strncpy(current_dir, filename, size);
		current_dir[size] = '\0';
		return 0;
	} else {
		return -1;
	}
}


/* FIXME !! */
int get_depth(char *filename)
{
	char local_filename[PATH_MAX];
	int depth = 0;
	char *pstr = NULL;

	// filename needs to be writtable for strtok
	strcpy(local_filename, filename);
	if (filename == NULL)
		return -1;

	pstr = strtok(local_filename, "/");
	while (pstr != NULL) {
		pstr = strtok(NULL, "/");
		depth++;
	}


	return depth-2;
}

int get_extension(char *extension, char *string)
{
	char *pstr = NULL;

	pstr = strrchr(string, '.');
	if (pstr == NULL || pstr == string) {
		strcpy(extension, "");
		return -1;
	}

	strcpy(extension, pstr+1);

	return 0;
}


/* get current filename without absolute path */
int get_short_filename(char *filename, char *short_filename)
{
	char *pchr = NULL;

	if (filename == NULL || short_filename == NULL)
		return -1;

	if (strlen(filename) == 0)
		return -1;

	pchr = strrchr(filename, '/');
	
	if (pchr != NULL && strlen(pchr) <= 1) {
		strcpy(short_filename, "");
		return -1;
	} else {
		strcpy(short_filename, pchr+1);
		return 0;
	}
}

int get_short_filename_no_ext(char *filename, char *short_filename)
{
	int ret = 0;

	if ((ret = get_short_filename(filename, short_filename)) < 0)
		return ret;

	if ((ret = remove_extension(short_filename, short_filename)) < 0)
		return ret;

	return ret;
}

int remove_extension(char *ext_filename, char *filename)
{
	char *pstr = NULL;
	int ext_pos = 0;

	if (ext_filename == NULL || filename == NULL)
		return -1;

	pstr = strrchr(ext_filename, '.');
	if (pstr == NULL || pstr == ext_filename) {
		strcpy(filename, "");
		return -1;
	}
	
	ext_pos =  pstr - ext_filename;
	if (ext_pos <= 0) {
		strcpy(filename, "");
		return -1;
	}

	strcpy(filename, ext_filename);
	filename[ext_pos] = '\0';

	return 0;
}


int get_html_url(char *filename, char *output_dir, char *html_filename)
{
	char current_dir[PATH_MAX];
	char short_filename[255];

	if (filename == NULL || html_filename == NULL || output_dir == NULL)
		return -1;

	strcpy(html_filename, output_dir);
	
	get_current_dir(filename, current_dir);
	get_short_filename(filename, short_filename);
	remove_extension(short_filename, short_filename);

	sprintf(html_filename, "%s/%s/%s.html", output_dir, current_dir, short_filename);
	return 0;
}

int is_markdown_file(char *filename)
{
	char extension[10];
	
	if (get_extension(extension, filename) != 0)
		return 0;
	
	if (!strcmp(extension, "mkd") ||
	    !strcmp(extension, "markdown")) {
		return 1;
	} else {
		return 0;
	}

	// TODO : check content ?
}

int is_html_file(char *filename)
{
	char extension[10];
	
	if (get_extension(extension, filename) != 0)
		return 0;
	
	if (!strcmp(extension, "htm") ||
	    !strcmp(extension, "html")) {
		return 1;
	} else {
		return 0;
	}
}

int is_image_file(char *filename)
{
	char extension[10];

	if (get_extension(extension, filename) != 0)
		return 0;
	
	if (!strcmp(extension, "jpg") || !strcmp(extension, "jpeg") ||
	    !strcmp(extension, "png") || !strcmp(extension, "gif")) {
		return 1;
	} else {
		return 0;
	}
	return 0;
}

// recursive creation
int create_dir(const char *dirname)
{
	int ret;
	char local_dirname[PATH_MAX];
	char sub_dirname[PATH_MAX];
	char *pstr = NULL;
	int first_loop = 1;

	if (dirname == NULL)
		return -1;

	if (is_dir(dirname))
		return 0;
	
	// dirname needs to be writtable for strtok
	strcpy(local_dirname, dirname);
	
	pstr = strtok(local_dirname, "/");
	while (pstr != NULL) {
		if (strcmp(pstr, ".") == 0) {
			strcpy(sub_dirname, pstr);
		} else {
			sprintf(sub_dirname, "%s/%s", sub_dirname, pstr);
		}

		if (*dirname == '/' && first_loop) {
			sprintf(sub_dirname, "/%s", pstr);
			first_loop = 0;
		}
		pstr = strtok(NULL, "/");
	
		if (is_dir(sub_dirname))
			continue;
		
		ret = mkdir(sub_dirname, S_IRWXU);
		if (ret != 0) {
			printf("Unable to create directory %s : %s\n", local_dirname, strerror(errno));
			return ret;
		}
	}

	return 0;
}

int cp_file(const char *source, const char *dest)
{
	FILE *fp_source;
	FILE *fp_dest;
	char short_filename[PATH_MAX];
	char dest_filename[PATH_MAX];
	int nb_bytes;
	char buffer[512];

	if (source == NULL || dest == NULL)
		return -1;

	fp_source = fopen(source, "rb");
	if (fp_source == NULL) {
		printf("Could not open file %s : %s\n", source, strerror(errno));
		return -1;
	}
	
	if (is_dir(dest)) {
		get_short_filename((char *)source, short_filename);
		sprintf(dest_filename, "%s/%s", dest, short_filename);
		fp_dest = fopen(dest_filename, "w+");
	} else{
		fp_dest = fopen(dest, "wb");
	}

	if (fp_dest == NULL) {
		printf("Could not open file %s : %s\n", dest, strerror(errno));
		fclose(fp_source);
		return -1;
	}

	while ((nb_bytes = fread(buffer, 1, 512, fp_source)) != 0)
		fwrite(buffer, 1, nb_bytes, fp_dest);

	fclose(fp_source);
	fclose(fp_dest);
	
	return 0;
}

int cp_dir(const char *source, const char *dest)
{
	DIR *dir;
	struct dirent entry;
	struct dirent *result;
	struct stat entryInfo;
	char path_name[PATH_MAX + 1];
	char path_new[PATH_MAX + 1];

	int ret = 0;

	if (source == NULL || dest == NULL)
		return -1;

	if (!is_dir(source))
		return -1;
	// TODO : gérer le dest déjà existant
	if (create_dir(dest) != 0)
		return -1;


	if ((dir = opendir(source)) == NULL) {
		printf("Couldn't open %s : %s\n", source, strerror(errno));
		return -1;
	}

	while ((readdir_r(dir, &entry, &result) == 0) && result != 0) {
		if (strcmp(entry.d_name, ".")  == 0 ||
		    strcmp(entry.d_name, "..") == 0) {
			continue;
		}
	
		strncpy(path_name, source, PATH_MAX);
		strncat(path_name, "/", PATH_MAX);
		strncat(path_name, entry.d_name, PATH_MAX);
		sprintf(path_new, "%s/%s", dest, entry.d_name);

		if (lstat(path_name, &entryInfo) == 0) {
			if (S_ISDIR (entryInfo.st_mode)) {
				if (create_dir(path_new) != 0)
					printf("erreur de creation de dir\n");
				if ((ret = cp_dir(path_name, path_new)) < 0)
					goto err_dir;
			} else {
				if ((ret = cp_file(path_name, path_new)) < 0)
					goto err_file;
			}
		}
	}

	return 0;
err_dir:
	printf("Couldn't copy dir\n");
	closedir(dir);
	return ret;
err_file:
	printf("Couldn't copy file\n");
	closedir(dir);
	return ret;
}


void process_elt(char *path, struct s_tree_elt *ptr_tree)
{
	strcpy(ptr_tree->name, path);
	ptr_tree->depth = get_depth(path);
}

struct s_tree_elt *parse_dir(char *path, struct s_tree_elt *ptr_tree)
{
	DIR *dir;
	struct dirent entry;
	struct dirent *result;
	struct stat entryInfo;
	char path_name[PATH_MAX + 1];

	if (path == NULL)
		goto err;

	if ((dir = opendir(path)) == NULL) {
		fprintf(stderr,"Couldn't open %s : %s\n", path, strerror(errno));
		goto err;
	}

	while ((readdir_r(dir, &entry, &result) == 0) && result != 0) {
		if (strcmp(entry.d_name, ".")  == 0 ||
		    strcmp(entry.d_name, "..") == 0) {
			continue;
		}
		
		strncpy(path_name, path, PATH_MAX);
		strncat(path_name, "/", PATH_MAX);
		strncat(path_name, entry.d_name, PATH_MAX);

		if (lstat(path_name, &entryInfo) == 0) {
			process_elt(path_name, ptr_tree++);
			if (S_ISDIR (entryInfo.st_mode)) {
				struct s_tree_elt *ptr_tree_dir;
				ptr_tree_dir = parse_dir(path_name, ptr_tree);
				if (ptr_tree_dir != NULL)
					ptr_tree = ptr_tree_dir;
			}
		}
	}

	closedir(dir);
	return ptr_tree;
err:
	return NULL;
}


int process_tree(struct s_tree_elt *tree, char *input_dir)
{
	int i;

	if (input_dir == NULL)
		return -1;

	for (i=0;i<200;i++) {
		tree[i].name[0] = '\0';
	}
	parse_dir(input_dir, tree);

	return 0;
}



int process_files(struct s_tree_elt *tree, char *output_dir)
{
	int i;
	char path[PATH_MAX];

	if (output_dir == NULL)
		return -1;

	create_dir(output_dir);

	for (i=0;i<200;i++) {
		if (tree[i].name[0] == '\0')
			break;

		if (is_dir(tree[i].name)) {
			sprintf(path, "%s/%s", output_dir, tree[i].name+2);
			create_dir(path);
		} else if (!is_markdown_file(tree[i].name)) {
			// TODO : cp
		} else {
			create_html_page(tree+i, tree, output_dir);
		}
	}
	
	return 0;
}
