
#include "markdown.h"
#include "html.h"
#include "buffer.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "files.h"

int main(int argc, char **argv)
{
	int ret;

	struct s_tree_elt tree[200];
	
	if ((ret = process_tree(tree)) < 0)
		goto err;

	if ((ret = process_files(tree)) < 0)
		goto err;

end:
	return ret;

err:
	fprintf(stderr, "Error in wiki generation.\n");
	return ret;
}
