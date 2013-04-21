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
	   
	   
	   