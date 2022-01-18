create table sgdev
(
devgr int NOT NULL,
modul int NOT NULL,
devid int NOT NULL,
fromval int,
toval int,
primary key (devgr, modul, devid),
foreign key (devgr) references devicegroups(devgr),
foreign key (modul) references devices(modul),
foreign key (devid) references devices(devid)
);