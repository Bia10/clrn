CREATE TABLE [decisions] (
  [win] INTEGER NOT NULL ON CONFLICT ABORT, 
  [position] INTEGER NOT NULL ON CONFLICT ABORT, 
  [pot_rate] INTEGER NOT NULL ON CONFLICT ABORT, 
  [stack_rate] INTEGER NOT NULL ON CONFLICT ABORT, 
  [players] INTEGER NOT NULL ON CONFLICT ABORT, 
  [danger] INTEGER NOT NULL ON CONFLICT ABORT, 
  [bot_avg_style] INTEGER NOT NULL ON CONFLICT ABORT, 
  [bot_style] INTEGER NOT NULL ON CONFLICT ABORT, 
  [bot_stack] INTEGER NOT NULL ON CONFLICT ABORT, 
  [decision] INTEGER NOT NULL ON CONFLICT ABORT, 
  CONSTRAINT [] PRIMARY KEY ([win], [position], [pot_rate], [stack_rate], [players], [danger], [bot_avg_style], [bot_style], [bot_stack]) ON CONFLICT REPLACE);

