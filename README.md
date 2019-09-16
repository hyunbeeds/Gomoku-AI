# GomokuAI
This Gomoku program is based on normal rules.(not renju rules)
(Only three three is banned.)

### How to use  
1. Input the mode (0,1,2,3)
    0-> between users
    1-> user->black, AI->white
    2-> user->white, AI->black
    3-> between AIs.(for debugging)
2. input the time limits of AI for one step (It is recommended to input over 1000ms)
3. Play the Gomoku


### Functions explanation
1. void init_pan() : initializes board
2. void print_pan() : prints board
3. boolean insert_dol() : get the input of one step, if one wins it returns true, otherwise it returns false
4. get_StoneCount() : this function returns the count of stones in one certain direction
5. win_check() : this function returns if one wins or not
6. is_Five(),is_Four... is_Two() etc.. : this functions returns whether one player achieved specific number of straight stones in one certain direction. 
7. print_status() : This function is used for debugging, and it returns the status which is composed of numbers of three straight stones, or numbers of four straight stones, numbers of closed straight three stones,.. etc...
8. evaluation() : This function returns the board status score of one color.
9. min_value_black(). max_value_black(), min_value_white(),min_value_black()
This function performs the evaluation of boards and performs the min-max algorithm and returns the score.
10. AI_minmax_Black(), AI_minmax_White() -> This function takes action based on evaluation.

###  AI explanation
This project is based on min-max algorithm and alpha-beta prunning.
I did iterative deepening search on this project, which performs search with increasing search limits until the time limit expires. 
And before doing search, if AI can win with one step, that state is stored in definite_x, definite_y and return true.

### Evaluation function explanation
In min-max algorithm, evaluation function is the most important part of program.
I evaluated board status with counting each numbers of open or closed count of stones and give each of them different scores. Below are specific scores that I gave. I think I can develop this evaluation function by changing specific scores of each status.

1. closed two - 1
2. open two - 2
3. closed three stones - 3
4. open three - 7
5. close four - 9 
6. open three three - (should be banned) -100000
7. open four - 2000
8. three four - 2000
9. five - 500000
