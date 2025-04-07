#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_PATH_LEN 1024

enum {
	NORMAL,
	IN_TAG,
	IN_PROP_NAME,
	IN_PROP_VALUE
} STATES;

const char USAGE[] = "Usage: htmlpaint -f <path> [-o <output>]";

void graceful_exit() {
	fprintf(stderr, USAGE);
	exit(1);
}

typedef struct {
	char *start;
	uint32_t length;
} String;

String *string_new(void) {
	String *string = malloc(sizeof(String));
	string->length = 0;
	string->start = malloc(1);
	string->start[0] = 0;

	return string;
}

void string_free(String *string) {
	if (string != NULL) {
		free(string->start);
		free(string);
	}
}

void string_push(String *string, char *chars) {
	uint32_t chars_len = strlen(chars);

	uint32_t new_size =  string->length + chars_len;
	realloc(string->start, new_size);
	strncat(string->start, chars, chars_len);

	string->length += chars_len;
}


size_t paint_prop_value(char *html, String *output) {
	char *chr;

	size_t len_chars = 0;
	string_push(output, "<span class='htmlpaint-value'>\"");
	while (*(chr = html++) != '\0' && *chr != '"' && *chr != '\'') {
		++len_chars;
		char current_chr[2];
		current_chr[0] = *chr;
		current_chr[1] = '\0';
		string_push(output, current_chr);
	}
	string_push(output, "\"</span>\n");

	return len_chars;
}

size_t paint_prop(char *html, String *output) {
	char *chr;

	size_t len_chars = 0;

	string_push(output, "<span class='htmlpaint-prop'>");

	while (*(chr = html++) != '\0') {
		++len_chars;
		if (*chr == '=') {
			string_push(output, "&equals;");
			html++;
			size_t len_chars2 = paint_prop_value(html, output);
			return len_chars + len_chars2 + 2;
		}
		else {
			char current_chr[2];
			current_chr[0] = *chr;
			current_chr[1] = '\0';
			string_push(output, current_chr);
		}
	}

	string_push(output, "</span>\r\n");

	return len_chars;
}

size_t paint_tag(char *html, String *output) {
	char *chr;
	size_t len_chars = 0;

	string_push(output, "<span class='htmlpaint-tag'>");
	
	while (*(chr = html++) != '\0' && *chr != '>') {
		++len_chars;
		if (*chr == ' ') {
			string_push(output, "&nbsp;");
			size_t len_chars2 = paint_prop(html, output);
			html += len_chars2;
		}
		else {
			char current_chr[2];
			current_chr[0] = *chr;
			current_chr[1] = '\0';
			string_push(output, current_chr);
		}
	}

	string_push(output, "</span>\r\n");
	return len_chars + 1;
}

void html_paint(char *html) {
	/*
	 * paint:
	 * - attribute names
	 * - attribute values
	 * - tag names
	 */
	String *output = string_new();
	char *chr;

	string_push(
			output,
			"<!doctype html>\n"
			"<html>\n"
			"<head>\n"
			"<style>\n"
			".htmlpaint-tag {color:red;} .htmlpaint-prop {color:green;}"
			" .htmlpaint-value {color:yellow;}\n"
			"</style>\n"
			"</head>\n"
			"<body>\n"
		);

	while (*(chr = html++) != '\0') {
		if (*chr == '<') {
			string_push(output, "&lt;");
			size_t len_chars = paint_tag(html, output);
			html = html + len_chars;
			string_push(output, "&gt;");
		}
		else {
			char current_chr[2];
			current_chr[0] = *chr;
			current_chr[1] = '\0';
			string_push(output, current_chr);
		}
	}
	string_push(output, "</body>\n</html>\n");

	printf("%s\n", output->start);

	free(output);
}
		
int main(int argc, char **argv) {
	/*
	char *src_path = NULL;
	char *target_path = NULL;


	int opt;

	while ((opt = getopt(argc, argv, "f:o:")) != -1) {
		switch (opt) {
			case 'f':
				printf("%s\n", optarg);
				src_path = malloc(MAX_PATH_LEN);
				strncpy(src_path, argv[optind-1], MAX_PATH_LEN);
			case 'o':
				printf("%s\n", optarg);
				target_path = malloc(MAX_PATH_LEN);
				strncpy(target_path, argv[optind-1], MAX_PATH_LEN);
			default:
				graceful_exit();
		}
	}

	if (src_path == NULL) {
		graceful_exit();
	}
	printf("%s, %s", src_path, target_path);
	*/
	char *html = "<span class=\"prop\">test</span>";

	printf("original: %s\n", html);
 
	html_paint(html);
	return 0;
}
