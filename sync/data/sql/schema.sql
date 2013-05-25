pragma auto_vacuum=0;
pragma default_cache_size=2000;
pragma encoding='UTF-8';
pragma page_size=1024;

CREATE TABLE [file_chunks] (
  [id] int, 
  [file] int CONSTRAINT [files_FK] REFERENCES [files]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE, 
  [index] int, 
  CONSTRAINT [sqlite_autoindex_file_chunks_1] PRIMARY KEY ([id] ASC));
  
CREATE TABLE [files] (
  [id] int, 
  [name] char(1024), 
  [size] int, 
  [crc] char(50), 
  [time] datetime, 
  [chunk_size] int, 
  [complete] BOOL, 
  CONSTRAINT [sqlite_autoindex_files_1] PRIMARY KEY ([id] ASC));

CREATE TABLE [host_map] (
  [src] INT, 
  [dst] INT, 
  [status] INT, 
  [ping] INT, 
  [ip] CHAR(15), 
  [port] INT, 
  CONSTRAINT [host_map_src_dst] UNIQUE([src], [dst]) ON CONFLICT REPLACE);

CREATE TABLE [hosts] (
  [id] INTEGER PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT, 
  [guid] char(36), 
  [ip] char(1024), 
  [port] int, 
  CONSTRAINT [hosts_id] UNIQUE([guid], [ip], [port]) ON CONFLICT IGNORE);
  
CREATE TABLE [plugins] (
  [id] INT NOT NULL ON CONFLICT REPLACE CONSTRAINT [id] UNIQUE ON CONFLICT REPLACE, 
  [name] VARCHAR2(1024) NOT NULL ON CONFLICT ABORT, 
  CONSTRAINT [sqlite_autoindex_plugins_1] PRIMARY KEY ([id]) ON CONFLICT REPLACE);
  
CREATE TABLE [settings] (
  [id] INTEGER PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT, 
  [module] int, 
  [name] char(50), 
  [value] char(1024), 
  CONSTRAINT [settings_data] UNIQUE([module], [name], [value]) ON CONFLICT IGNORE);
  
CREATE TABLE [accounts] (
  [id] INTEGER PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT, 
  [name] CHAR(1024) CONSTRAINT [account_name] UNIQUE ON CONFLICT IGNORE);

