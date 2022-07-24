# PostgreSQL

Setting up tables:
```
create table repos (id serial not null unique, owner-id integer not null, name text not null, description text, path text not null);
create table users (id serial not null unique, name text not null unique, email text not null, passwd text not null, repos integer[]);
```

Create user:
```
insert into users values (default, 'Kimplul', 'kimi.h.kuparinen@gmail.com', 'thisishashed');
```
