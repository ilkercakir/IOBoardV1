create table devices
(
modul integer NOT NULL,
devid integer NOT NULL,
dtext char(30),
dtype integer,
chnnl integer,
numstates integer,
initval integer,
dicon char(20),
primary key (modul, devid),
foreign key (modul) references modules(modul),
foreign key (dtype) references devicetypes(dtype)
);