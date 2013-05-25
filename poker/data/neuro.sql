CREATE TABLE [decisions] (
  [win] INTEGER NOT NULL ON CONFLICT ABORT, 
  [position] INTEGER NOT NULL ON CONFLICT ABORT, 
  [bet] INTEGER NOT NULL ON CONFLICT ABORT, 
  [players] INTEGER NOT NULL ON CONFLICT ABORT, 
  [danger] INTEGER NOT NULL ON CONFLICT ABORT, 
  [bot_avg_style] INTEGER NOT NULL ON CONFLICT ABORT, 
  [bot_style] INTEGER NOT NULL ON CONFLICT ABORT, 
  [bot_stack] INTEGER NOT NULL ON CONFLICT ABORT, 
  [decision] INTEGER NOT NULL ON CONFLICT ABORT, 
  CONSTRAINT [] PRIMARY KEY ([win], [position], [bet], [players], [danger], [bot_avg_style], [bot_style], [bot_stack]) ON CONFLICT REPLACE);

