#include <stdio.h>
#include <errno.h>
#include "files.h"
#include "render_html.h"

void render_navbar(FILE *fp)
{
	if (fp == NULL)
		return;

	fputs("<div class=\"navbar navbar-fixed-top\">\n", fp);
	fputs("	<div class=\"navbar-inner\">\n", fp);
	fputs("		<div class=\"container-fluid\">\n", fp);
	fputs("			<a class=\"brand\" href=\"#\">ucDoc</a>\n", fp);
	fputs("		</div>\n", fp);
	fputs("	</div>\n", fp);
	fputs("</div>\n", fp);
}

void render_sidebar(FILE *fp, struct s_tree_elt *tree)
{
	int i;
	
	if (fp == NULL)
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

		if (is_dir(tree[i].name)) {
			get_short_filename(tree[i].name, short_filename);
			sprintf(string, "<li class=\"nav-header\">%s</li>", short_filename);
		} else {
			get_short_filename_no_ext(tree[i].name, short_filename);
			get_html_url(tree[i].name, filename);
			sprintf(string, "<li><a href=\"%s\">%s</a></li>", filename, short_filename);
		}
		fputs(string, fp);
	}

	fputs("	</ul>\n", fp);
	fputs("</div>\n", fp);

}

int create_html_page(char *output, char *buffer, struct s_tree_elt *tree)
{
	FILE *fp;

	if (output == NULL || buffer == NULL)
		goto err;
	
	fp = fopen(output, "w");
	if (!fp) {
		printf("Unable to open file %s : %s\n", output, strerror(errno));
		goto err;
	}

	fputs("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n", fp);
	fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\" dir=\"ltr\" lang=\"fr-FR\">\n", fp);
	fputs("<head>\n", fp);
	fputs("	<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\">\n", fp);
	fputs("	<link href=\"http://localhost/doc/bootstrap/css/bootstrap.css\" type=\"text/css\" rel=\"stylesheet\">\n", fp);
	fputs("	<link href=\"http://localhost/doc/custom.css\" type=\"text/css\" rel=\"stylesheet\">\n", fp);
	fputs("</head>\n", fp);
	fputs("<body>\n", fp);

	render_navbar(fp);

	fputs("<div class=\"container-fluid\">\n", fp);
	fputs("<div class=\"row-fluid\">\n", fp);
	fputs("<div class=\"span3\">\n", fp);

	render_sidebar(fp, tree);

	fputs("</div>\n", fp);

	fputs("<div class=\"span9\">\n", fp);
	fputs("<div class=\"well\">\n", fp);
	fputs(buffer, fp);
	fputs("</div>\n", fp);
	fputs("</div>\n", fp);
	fputs("</body>\n", fp);
	fputs("</html>\n", fp);

end:
	fclose(fp);
	return 0;
err:
	return -1;
}
