CREATE TABLE [cards] (
  [id] INTEGER NOT NULL ON CONFLICT ABORT, 
  [value] CHAR NOT NULL ON CONFLICT ABORT);

CREATE UNIQUE INDEX [cards_index] ON [cards] ([id], [value]);


CREATE TABLE [games] (
  [id] INTEGER NOT NULL ON CONFLICT ABORT PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT, 
  [players] INTEGER NOT NULL ON CONFLICT ABORT, 
  [flop] INTEGER CONSTRAINT [games_flop_id_FK] REFERENCES [cards]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE);


CREATE TABLE [players] (
  [id] INTEGER NOT NULL ON CONFLICT ABORT PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT, 
  [name] NVARCHAR2 NOT NULL ON CONFLICT ABORT);


CREATE TABLE [actions] (
  [game] INTEGER NOT NULL ON CONFLICT ABORT CONSTRAINT [game_id_FK] REFERENCES [games]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE, 
  [player] INTEGER NOT NULL ON CONFLICT ABORT CONSTRAINT [player_id_FK] REFERENCES [players]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE, 
  [street] INTEGER NOT NULL ON CONFLICT ABORT, 
  [action] INTEGER NOT NULL ON CONFLICT ABORT, 
  [pot_amount] FLOAT NOT NULL ON CONFLICT ABORT, 
  [stack_amount] FLOAT NOT NULL ON CONFLICT ABORT, 
  [position] INTEGER NOT NULL ON CONFLICT ABORT);

CREATE INDEX [player_index] ON [actions] ([player]);

CREATE INDEX [game_index] ON [actions] ([game]);


CREATE TABLE [hands] (
  [player] INTEGER NOT NULL ON CONFLICT ABORT CONSTRAINT [player_id_FK] REFERENCES [players]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE, 
  [game] INTEGER NOT NULL ON CONFLICT ABORT CONSTRAINT [game_id_FK] REFERENCES [games]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE, 
  [cards] INTEGER CONSTRAINT [hand_id_FK] REFERENCES [cards]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE);


CREATE TABLE [percents] (
  [player] INTEGER NOT NULL ON CONFLICT ABORT CONSTRAINT [player_id_FK] REFERENCES [players]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE, 
  [game] INTEGER NOT NULL ON CONFLICT ABORT CONSTRAINT [game_id_FK] REFERENCES [games]([id]) ON DELETE CASCADE ON UPDATE CASCADE MATCH SIMPLE, 
  [street] LARGEINT NOT NULL ON CONFLICT ABORT, 
  [value] FLOAT NOT NULL ON CONFLICT ABORT);

CREATE UNIQUE INDEX [unique_player_game_street] ON [percents] ([player], [game], [street]);


