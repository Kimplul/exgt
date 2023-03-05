/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file index.c
 * Index page generator.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#include <utils/error.h>
#include <utils/chain.h>
#include <utils/http.h>
#include <utils/file.h>
#include <utils/path.h>
#include <utils/git.h>
#include <utils/res.h>

#include "pages.h"

/** Index generator resource manager. */
static struct res *r;

/**
 * Generate page content, in the context of the index page this means
 * instance header and description.
 *
 * @todo Should \c generate_* only actually generate the content, and adding as
 * a child or whatever be done in the calling function? Might be cleaner, but
 * sightly less compact.
 *
 * @param index_main Main element, which the content will be added to as a child.
 * @return Content element.
 */
static struct html_elem *generate_content(struct html_elem *index_main)
{
	struct html_elem *content = html_add_child(index_main, "div", NULL);
	html_add_attr(content, "class", "content text");

	/** @todo use instance text instead of dummy values */
	struct html_elem *h1 =
		html_add_child(content, "h1", "Example header\n");
	html_add_elem(h1, "p", "This is example text.\n");

	return content;
}

/**
 * Generate project header, that is name and date as well as href link.
 *
 * @param project Project header is added to as a child.
 * @param name Name of project.
 * @param path Path of project.
 * @param date Last update of project.
 * @return Project header element.
 */
static struct html_elem *generate_project_header(struct html_elem *project,
                                                 const char *name,
                                                 const char *path,
                                                 const char *date)
{
	struct html_elem *project_header = html_add_child(project, "div", NULL);
	html_add_attr(project_header, "class", "project header");

	struct html_elem *project_title =
		html_add_child(project_header, "a", name);
	html_add_attr(project_title, "class", "project title bold");
	html_add_attr(project_title, "href", path);

	struct html_elem *project_date =
		html_add_elem(project_title, "time", date);
	html_add_attr(project_date, "class", "date");

	return project_header;
}

/**
 * Generate project content, namely project description.
 *
 * @param project_header Project header, after which the content is added.
 * @param description Description of project.
 * @return Project content element.
 */
static struct html_elem *generate_project_content(
	struct html_elem *project_header, const char *description)
{
	struct html_elem *project_content =
		html_add_elem(project_header, "div", NULL);
	html_add_attr(project_content, "class", "project content");

	struct html_elem *project_description =
		html_add_child(project_content, "p", description);
	html_add_attr(project_description, "class", "project description");

	return project_content;
}

/**
 * Generate one project entry in project list from known values.
 *
 * @param name Name of project.
 * @param path Path of project.
 * @param date Last update of project.
 * @param description Description of project.
 * @return Project element.
 */
static struct html_elem *generate_known_project(const char *name,
                                                const char *path,
                                                const char *date,
                                                const char *description)
{
	struct html_elem *project = html_create_elem("div", NULL);
	html_add_attr(project, "class", "project border");

	struct html_elem *project_header;
	if (!(project_header =
		      generate_project_header(project, name, path, date))) {
		fprintf(stderr, "couldn't generate project header\n");
		return NULL;
	}

	if (!generate_project_content(project_header, description)) {
		fprintf(stderr, "couldn't generate project content\n");
		return NULL;
	}

	return project;
}

/**
 * Generate one project entry in project list with database connection info.
 *
 * @param entry Directory entry to generate project from.
 * @return Corresponding html element.
 */
static struct html_elem *generate_project(struct dirent *entry)
{
	char *name = strdup(entry->d_name);
	res_add(r, name);

	char *description = repo_description(name);
	res_add(r, description);

	char *ref_path = build_web_path(name);
	if (!ref_path)
		return NULL;

	res_add(r, ref_path);

	char *real_path = repo_real_file(name);
	if (!real_path)
		return NULL;

	res_add(r, real_path);

	char *date = repo_last_commit(real_path);
	if (!date)
		return NULL;

	res_add(r, date);

	return generate_known_project(name, ref_path, date, description);
}

/**
 * Take some sampling of existing projects and insert them into the project
 * list.
 *
 * @todo Figure out which projects to choose, just newest or should I choose
 * try to implement some kind of `star` mechanism?
 *
 * @param project_list Project list into which the projects will be inserted.
 * @return Last project inserted.
 */
static struct html_elem *generate_projects(struct html_elem *project_list)
{
	struct html_elem *project = NULL;
	char *root = git_real_root();
	if (!root)
		return NULL;

	res_add(r, root);

	DIR *dir = opendir(root);
	if (!dir) {
		fprintf(stderr, "couldn't open exgt root %s\n", root);
		return NULL;
	}

	struct dirent *dirent = NULL;
	while ((dirent = readdir(dir))) {
		if (dirent->d_name[0] == '.')
			continue;

		struct html_elem *new_project = generate_project(dirent);
		if (!new_project)
			continue;

		if (project)
			html_append_elem(project, new_project);
		else
			html_append_child(project_list, new_project);

		project = new_project;
	}

	if (!project) {
		project = html_add_child(project_list, "p",
		                         "Sorry, looks like there aren't any projects.");
		html_add_attr(project, "class", "project");
	}

	return project;
}

/**
 * Generate a list of projects. Something of a wrapper for \ref
 * generate_projects().
 *
 * @param content Content element after which the project list is added.
 * @return Project list element.
 */
static struct html_elem *generate_project_list(struct html_elem *content)
{
	struct html_elem *project_list = html_add_elem(content, "div", NULL);
	html_add_attr(project_list, "class", "project-list");
	if (!generate_projects(project_list))
		return NULL;

	return project_list;
}

/**
 * Generate index page main element. Consists of a content element and a project
 * list element.
 *
 * @param index_main Main element, which the main element will be added to as a child.
 * @return Main element.
 */
static struct html_elem *generate_main(struct html_elem *index_main)
{
	struct html_elem *content;
	if (!(content = generate_content(index_main)))
		return NULL;

	struct html_elem *project_list;
	if (!(project_list = generate_project_list(content)))
		return NULL;

	return project_list;
}

void index_serve(FILE *file)
{
	r = res_create();

	http_header(file, 200, "text/html");
	struct html_elem *html, *index_main;
	if (!(html = pages_generate_common(r, "Index\n",
	                                   &index_main, NULL))) {
		error_serve(file, 500, "error serving index\n");
		goto out;
	}

	if (!generate_main(index_main)) {
		error_serve(file, 500, "couldn't generate index main\n");
		goto out;
	}

	html_print(file, html);
out:
	html_destroy(html);
	res_destroy(r);
}
