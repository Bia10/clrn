-- raise if we have nuts
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 5 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0, 1, 2, 3, 4 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1, 2 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 0, 1, 2 ) 
       AND decision IN ( 0, 1 ) 
	   
-- raise if win = vg and danger is not high
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 4, 5 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0, 1, 2, 3, 4 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 0, 1, 2 ) 
       AND decision IN ( 0, 1 ) 
	   
-- call if win = vg and danger is high
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 4 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0, 1, 2, 3, 4 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 2 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 0, 1, 2 ) 
       AND decision IN ( 0, 2 ) 	   
	   
-- raise in middle/later position with good win and danger is not high and bet is not high	   
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 3 ) 
       AND position IN ( 1, 2 ) 
       AND bet IN ( 0, 1, 2 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0, 1 ) 
	   
-- call else with good win	   
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 3 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0, 1, 2 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0 ) 
	   
-- raise in later position with normal win
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 2 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0, 1 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0, 1 ) 	   
	   
-- call if bot is aggressive or position is not later
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 2 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0, 1, 2 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0 ) 	 

-- raise with low win only in later position with low danger
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 1 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0, 1 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0 ) 
       AND bot_avg_style IN ( 0 ) 
       AND bot_style IN ( 0, 1 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0, 1 ) 	

-- call with low win only if bet is low and position is later
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 1 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0, 1 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0 ) 		   
	   
-- raise with very low only on later pos without bets and low danger at hands up
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 0 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0 ) 
       AND players IN ( 0 ) 
       AND danger IN ( 0 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 0, 1 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0, 1 ) 	
	   
-- call with normal win from later positions with small stack
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 2 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0, 1 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1 ) 
       AND bot_stack IN ( 0, 1, 2 ) 
       AND decision IN ( 0 ) 		   
	   
-- call low bet with low win from later positions with nomal or big stacks with high danger
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 1 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0 ) 
       AND players IN ( 0, 1 ) 
       AND danger IN ( 0, 1, 2 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0 ) 		   
	   
-- bluff with low and very low win and later pos without bets and without danger
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 0, 1 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0 ) 
       AND players IN ( 0, 1 ) 
       AND danger IN ( 0 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 0 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0 ) 
	   
-- call hight bets with normal rate from later position with low danger
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 2 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 3 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0 ) 
	   
-- call low bets with normal rate and small stack
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 2 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0, 1 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 0, 1, 2 ) 
       AND decision IN ( 0 ) 

-- call normal bets with good rate and high danger
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 3 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0, 1, 2 ) 
       AND players IN ( 0, 1 ) 
       AND danger IN ( 2 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 0, 1, 2 ) 
       AND decision IN ( 0 ) 	   
	   
-- call huge bets with good rate and not high danger with one player
UPDATE decisions 
SET    decision = 1 
WHERE  win IN ( 3 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 3, 4 ) 
       AND players IN ( 0 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1, 2 ) 
       AND bot_style IN ( 0, 1, 2 ) 
       AND bot_stack IN ( 1, 2 ) 
       AND decision IN ( 0 ) 	 

-- raise hude bets with small stack from later position and normal rate
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 2 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 4 ) 
       AND players IN ( 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 0 ) 
       AND bot_stack IN ( 0 ) 
       AND decision IN ( 0 ) 	   
	   
-- all in on preflop with small stack and >=normal win	   
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 2, 3, 4, 5 ) 
       AND position IN ( 2 ) 
       AND bet IN ( 0, 1, 2, 3, 4 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0, 1 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 0 ) 
       AND bot_stack IN ( 0 ) 
       AND decision IN ( 0 ) 	
	   
-- continuation bets with low danger
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 0, 1 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0 ) 
       AND players IN ( 0, 1 ) 
       AND danger IN ( 0 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 2 ) 
       AND bot_stack IN ( 2 ) 
       AND decision IN ( 0 ) 		   
	   
-- push bets with low danger and normal win rate
UPDATE decisions 
SET    decision = 2 
WHERE  win IN ( 2 ) 
       AND position IN ( 0, 1, 2 ) 
       AND bet IN ( 0 ) 
       AND players IN ( 0, 1, 2 ) 
       AND danger IN ( 0 ) 
       AND bot_avg_style IN ( 0, 1 ) 
       AND bot_style IN ( 2 ) 
       AND bot_stack IN ( 2 ) 
       AND decision IN ( 0, 1 ) 		   