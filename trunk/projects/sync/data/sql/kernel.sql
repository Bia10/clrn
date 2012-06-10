insert into settings(module, name, value) values(0, 'log_level', '4');
insert into settings(module, name, value) values(1, 'log_level', '4');
insert into settings(module, name, value) values(2, 'log_level', '4');
insert into settings(module, name, value) values(3, 'log_level', '4');
insert into settings(module, name, value) values(4, 'log_level', '4');
insert into settings(module, name, value) values(5, 'log_level', '4');
insert into settings(module, name, value) values(0, 'log_source', '1');
insert into settings(module, name, value) values(1, 'log_source', '1');
insert into settings(module, name, value) values(2, 'log_source', '1');
insert into settings(module, name, value) values(3, 'log_source', '1');
insert into settings(module, name, value) values(4, 'log_source', '1');
insert into settings(module, name, value) values(5, 'log_source', '1');
insert into settings(module, name, value) values(0, 'udp_port', '5000');
insert into settings(module, name, value) values(0, 'udp_threads', '4');
insert into settings(module, name, value) values(0, 'udp_buffer_size', '4096');
insert into settings(module, name, value) values(0, 'kernel_threads', '4');
insert into settings(module, name, value) values(0, 'plugins_path', './');
insert into settings(module, name, value) values(0, 'ping_interval', '5000');

insert into plugins values(1, 'Events');
insert into plugins values(2, 'Settings');
insert into plugins values(3, 'DataBase');
insert into plugins values(4, 'Network');

insert into hosts(guid, ip, port) values('00000000-0000-0000-0000-000000000000', '192.168.1.2', 5000);
insert into hosts(guid, ip, port) values('00000000-0000-0000-0000-000000000001', '192.168.1.2', 10000);


insert into accounts values(0, 'CLRN');
update settings set value = '3' where name = 'log_level';

