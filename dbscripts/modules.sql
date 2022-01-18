create table modules
(
modul int NOT NULL,
modtx char(30),
categ char(1) NOT NULL,
primary key (modul),
foreign key (categ) references devcat(categ)
);