# PostgreSQL

Setting up tables:
```
create table repos (id serial not null unique, owner integer not null, name text not null, description text, path text not null);
create table users (id serial not null unique, name text not null unique, email text not null, passwd text not null, repos integer[], theme text);
```

Create user:
```
insert into users values (default, 'Kimplul', 'kimi.h.kuparinen@gmail.com', 'thisishashed', 'default');
```

# Testing

I really should try to make a script for this, but here's the bare minimum to
get the repo to generate a dirview of itself:

```
QUERY_STRING= GIT_PROJECT_ROOT=$(pwd)/../.. PATH_INFO=/exgt/Projects/exgt HTTP_ACCEPT=text/html ./exgt
```

In this case, we pretend whatever directory is two levels above is the root of
EXGT, with the next directory down being usernames, and the second down is the
project associated with the user. In this case Projects is our 'user' and exgt
is the project name, i.e. this project.
