#include <stdio.h>
#include <html/html.h>

int main()
{
	struct html_elem *p = html_create_elem("p",
	                                       "This is my text, is this better?");
	html_add_attr(p, "class", "dickbutt");
	html_print(p);
	html_destroy(p);
}
