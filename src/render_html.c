#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "files.h"
#include "render_html.h"

#include "markup.h"

void render_navbar(FILE *fp)
{
	if (fp == NULL)
		return;

	fputs("<div class=\"navbar navbar-fixed-top\">\n", fp);
	fputs("	<div class=\"navbar-inner\">\n", fp);
	fputs("		<div class=\"container-fluid\">\n", fp);
	fputs("			<a class=\"brand\" href=\"#\">µcWiki</a><span style=\"display: block;padding-top:10px;\"> - The tiny wiki engine</span>\n", fp);
	fputs("		</div>\n", fp);
	fputs("	</div>\n", fp);
	fputs("</div>\n", fp);
}

void render_sidebar(FILE *fp, struct s_tree_elt *tree, struct s_tree_elt *page, char *output_dir)
{
	int i;
	
	if (fp == NULL || tree == NULL || page == NULL || output_dir == NULL)
		return;

	fputs("<div class=\"well sidebar-nav\">\n", fp);
	fputs("	<ul class=\"nav nav-list\">\n", fp);
	fputs("		<li class=\"nav-header\">Sidebar</li>\n", fp);
	
	for (i=0;i<200;i++) {
		char filename[255];
		char short_filename[255];
		char string[600];

		if (tree[i].name[0] == '\0')
			break;

		if (!is_markdown_file(tree[i].name) && !is_dir(tree[i].name))
			continue;
		int j;
		char depth_str[250];
		strcpy(depth_str, "");
		// for (j=0;j<tree[i].depth;j++) {
		// 	strcat(depth_str, "../");
		// }

		if (is_dir(tree[i].name)) {
			get_short_filename(tree[i].name, short_filename);
			sprintf(string, "<li class=\"nav-header\">%s</li>", short_filename);
		} else {
			get_short_filename_no_ext(tree[i].name, short_filename);
			get_html_url(tree[i].name, output_dir, filename);
			sprintf(string, "<li><a href=\"%s%s\">%s</a></li>", depth_str, filename, short_filename);
		}
		fputs(string, fp);
	}

	fputs("	</ul>\n", fp);
	fputs("</div>\n", fp);

}

int create_html_page(struct s_tree_elt *file, struct s_tree_elt *tree, char *output_dir)
{
	char output[PATH_MAX];
	char *buffer = NULL;
	FILE *fp;
	int ret = 0;
	char html_str[500];

	if (file == NULL || tree == NULL || output_dir == NULL)
		goto early_err;

	get_html_url(file->name, output_dir, output);
	
	if ((ret = convert_mkd_to_html(file->name, &buffer)) < 0) // TODO use struct s_tree_elt *tree ?
		goto err;

	int i;
	char depth_str[250];
	strcpy(depth_str, "");
	for (i=0;i<file->depth;i++) {
		strcat(depth_str, "../");
	}

	fp = fopen(output, "w");
	if (!fp) {
		fprintf(stderr, "Unable to open file %s : %s\n", output, strerror(errno));
		goto err;
	}

	fputs("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n", fp);
	fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\" dir=\"ltr\" lang=\"fr-FR\">\n", fp);
	fputs("<head>\n", fp);
	fputs("	<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\">\n", fp);
	sprintf(html_str, " <link href=\"%s/bootstrap/css/bootstrap.css\" type=\"text/css\" rel=\"stylesheet\">\n", depth_str);
	fputs(html_str, fp);
	sprintf(html_str, " <link href=\"%s/custom.css\" type=\"text/css\" rel=\"stylesheet\">\n", depth_str);
	fputs(html_str, fp);
	fputs("</head>\n", fp);

	fputs("<body>\n", fp);

	render_navbar(fp);

	fputs("<div class=\"container-fluid\">\n", fp);
	fputs("<div class=\"row-fluid\">\n", fp);
	fputs("<div class=\"span3\">\n", fp);

	render_sidebar(fp, tree, file, output_dir);

	fputs("</div>\n", fp);

	fputs("<div class=\"span9\">\n", fp);
	fputs("<div class=\"well\">\n", fp);
	fputs(buffer, fp);
	fputs("</div>\n", fp);
	fputs("<footer class=\"well\">Powered by <a href=\"https://github.com/yoannsculo/ucwiki\">µcwiki</a> and <a href=\"http://twitter.github.com/bootstrap\">bootstrap</a>.</footer>\n", fp);
	fputs("</div>\n", fp);
	fputs("</body>\n", fp);
	fputs("</html>\n", fp);

err:
	if (buffer != NULL)
		free(buffer);
	if (fp != NULL)
		fclose(fp);
	return ret;

early_err:
	return -1;
}
