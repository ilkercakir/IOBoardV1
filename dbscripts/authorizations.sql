create table authorizations
(
username char(20) NOT NULL,
modul integer NOT NULL,
devid integer NOT NULL,
level char(1),
primary key (username, modul, devid),
foreign key (username) references users(username),
foreign key (modul) references devices(modul),
foreign key (devid) references devices(devid)
);