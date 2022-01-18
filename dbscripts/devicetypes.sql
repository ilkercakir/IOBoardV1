create table devicetypes
(
dtype integer NOT NULL,
categ char(1) NOT NULL,
dtext char(20),
dstat integer,
dicon char(20),
primary key (dtype),
foreign key (categ) references devcat(categ)
);