#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "markup.h"

#include "markdown.h"
#include "html.h"
#include "buffer.h"

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

/*
 * Take a file path as input and give a HTML formatted string output
 */
int convert_mkd_to_html(char *input, char **output)
{
	struct buf *ib, *ob;
	int ret;
	FILE *fp_input;

	struct sd_callbacks callbacks;
	struct html_renderopt options;
	struct sd_markdown *markdown;

	if (input == NULL || output == NULL)
		return -1;

	fp_input = fopen(input, "r");
	if (!fp_input) {
		printf("Unable to open file %s : %s\n", input, strerror(errno));
		return -1;
	}

	/* reading everything */
	ib = bufnew(READ_UNIT);
	bufgrow(ib, READ_UNIT);
	while ((ret = fread(ib->data + ib->size, 1, ib->asize - ib->size, fp_input)) > 0) {
		ib->size += ret;
		bufgrow(ib, ib->size + READ_UNIT);
	}

	/* performing markdown parsing */
	ob = bufnew(OUTPUT_UNIT);

	sdhtml_renderer(&callbacks, &options, 0);
	markdown = sd_markdown_new(0, 16, &callbacks, &options);

	sd_markdown_render(ob, ib->data, ib->size, markdown);
	sd_markdown_free(markdown);

	*output = malloc(ob->size+1);
	if (*output == NULL) {
		printf("Memory allocation error\n");
		return -1;
	}

	memcpy(*output, ob->data, ob->size);
	(*output)[ob->size] = '\0';
	/* cleanup */
	bufrelease(ib);
	bufrelease(ob);
	
	return (ret < 0) ? -1 : 0;
}


