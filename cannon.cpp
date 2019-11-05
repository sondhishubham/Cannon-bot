#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
using namespace std;

int h_blacks[4][8][8];
int h_whites[4][8][8];
int MAX_PLIES = 4;
int SOLDIER_VAL = 3;
//int NUM_MOVES = 10;

class Pawn {
public:
    Pawn(Pawn *pPawn) {
        corX=pPawn->getcorX();
        corY=pPawn->getcorY();
        side=pPawn->getside();
    }
    enum Side{
        BLACK,WHITE
    };
    Pawn(Pawn::Side sd,int x,int y){
        corX=x;
        corY=y;
        side=sd;
    };
    Pawn::Side getside(){return side;};
    int getcorX(){return corX;};
    int getcorY(){return corY;};
    int setcorX(int a){corX=a;};
    int setcorY(int b){corY=b;};
private:
    int corX;
    int corY;
    Side side;
};

class game{
public:
    int board[8][8];// positive integers represent blacks and negative integers represent whites
    game()
    {
        heuristic=0;
        for(int i(0);i<8;i++) for(int j(0);j<8;j++)board[i][j]=0;
        for(int i(0);i<4;i++) { destroyedWhites.push_back(1);destroyedBlacks.push_back(1);}
        int blackCount(1),whiteCount(-1);
        for(int i=0; i<8; i+=2) {
            for (int j = 5; j < 8; j++) {
                blacks.push_back(new Pawn(Pawn::BLACK, i, j));
                whites.push_back(new Pawn(Pawn::WHITE, i+1, 7-j));
                board[i][j]=blackCount;
                board[i+1][7-j]=whiteCount;
                blackCount++,whiteCount++;
            }
        }
    }
    vector<int> destroyedBlacks;
    vector<int> destroyedWhites;
    int heuristic;
    vector<Pawn> getBlack(){ return blacks;}
    vector<Pawn> getWhite(){ return whites;}
    void setSoldiers(vector<Pawn> p,Pawn::Side s){
        if(s==Pawn::BLACK) {
            this->blacks=p;
        }
        else {
            this->whites=p;
        }
    }
    vector<Pawn> getSoldiers(Pawn::Side p){if (p==Pawn::BLACK) return blacks; else return whites;}
    int getPawn(int curr_x, int curr_y){
        int m=board[curr_x][curr_y]
        if (m>0) return (m-1);
        return -(m+1);
        //cout<<"i am here 55"<<endl ;
        // if(b==Pawn::BLACK)
        // {
        //     for(int i(0);i<blacks.size();i++)
        //     { if((blacks[i].getcorX()==curr_x)&&(blacks[i].getcorY()==curr_y)) return i;}
        // }
        // else
        // {
        //     for(int i(0);i<whites.size();i++)
        //     { if((whites[i].getcorX()==curr_x)&&(whites[i].getcorY()==curr_y)) return i;}
        // }
        // return (-1);
    }
}

private:
    vector<Pawn> blacks;
    vector<Pawn> whites;
};

Pawn::Side box2Side(int p){
    if(p>0)
        return Pawn::BLACK;
    return Pawn::WHITE;
}

Pawn::Side getEnemySide(int p){return  (p>0) ? (Pawn::WHITE) : (Pawn::BLACK);}

void printBoard(int b[8][8]){
    for(int i(0);i<8;i++)
    {
        for(int j(0);j<8;j++)
        {
            cout<<b[j][i]<<' ';
        }
        cout<<endl;
    }
}

void addMove(int x, int y, int i,int shot, vector<vector<int> >&moves, int board[8][8],Pawn::Side s);
bool isEnemyPawnAt(int x, int y, int board[8][8],Pawn::Side s);
bool isEnemyAttacking(int x, int y, int board[8][8],Pawn::Side s);
bool isSelfPawnAt(int x, int y, int board[8][8],Pawn::Side s);
bool isSelfTownHallAt(int x, int y, Pawn::Side s);
bool isEnemyTownHallAt(int x, int y, Pawn::Side s);
void initialize_heuristic();
int MaxVal(game game, int alpha, int beta,int maxPlies, int numPlies, Pawn::Side p);

vector<vector<int> > getMoves(game g, Pawn::Side p) {
    vector<Pawn> soldiers = g.getSoldiers(p);
    int x, y;
    vector<vector<int> > moves;
    int isBlack = -1;
    if (p == Pawn::BLACK) isBlack = 1;
    for (int i = 0; i < soldiers.size(); i++) {

        x = soldiers[i].getcorX();
        y = soldiers[i].getcorY();
        //int curBox = g.board[x][y];
        // NORMAL MOVES
        addMove(x - 1, y - 1 * isBlack, i, 0, moves, g.board, p);
        addMove(x, y - 1 * isBlack, i, 0, moves, g.board, p);
        addMove(x + 1, y - 1 * isBlack, i, 0, moves, g.board, p);
        // RETREAT MOVES
        if (isEnemyAttacking(x, y, g.board, p)) {
            addMove(x - 2, y + 2 * isBlack, i, 0, moves, g.board, p);
            addMove(x, y + 2 * isBlack, i, 0, moves, g.board, p);
            addMove(x + 2, y + 2 * isBlack, i, 0, moves, g.board, p);
        }
        // SIDE MOVES
        if (isEnemyPawnAt(x - 1, y, g.board, p)) {
            addMove(x - 1, y, i, 0, moves, g.board, p);
        }
        if (isEnemyPawnAt(x + 1, y, g.board, p)) {
            addMove(x + 1, y, i, 0, moves, g.board, p);
        }
	if(isEnemyTownHallAt(x-1,y,p)){
            addMove(x - 1, y, i, 0, moves, g.board, p);
        }
        if(isEnemyTownHallAt(x+1,y,p)){
            addMove(x + 1, y, i, 0, moves, g.board, p);
        }

        //CANNON MOVES
        if (isSelfPawnAt(x + 1, y, g.board, p) && isSelfPawnAt(x + 2, y, g.board, p)) {
            //Horizontal cannon
            if (!isEnemyPawnAt(x + 3, y, g.board, p)) {
                // cannon move
                addMove(x + 3, y, i, 0, moves, g.board, p);
                //cannon shot
                if (!isSelfPawnAt(x + 3, y, g.board, p)) {
                    addMove(x + 4, y, i, 1, moves, g.board, p);
                    addMove(x + 5, y, i, 1, moves, g.board, p);
                }
            }
            if (!isEnemyPawnAt(x - 1, y, g.board, p)) {
                int tail = g.getPawn(x + 2, y);
                addMove(x - 1, y, tail, 0, moves, g.board, p);
                if (!isSelfPawnAt(x - 1, y, g.board, p)) {
                    addMove(x - 2, y, i, 1, moves, g.board, p);
                    addMove(x - 3, y, i, 1, moves, g.board, p);
                }
            }
        }

        if (isSelfPawnAt(x, y+1, g.board, p) && isSelfPawnAt(x, y+2, g.board, p)) {
            //vertical cannon
            if (!isEnemyPawnAt(x, y + 3, g.board, p)) {
                addMove(x, y + 3, i, 0, moves, g.board, p);
                if (!isSelfPawnAt(x, y + 3, g.board, p)) {
                    addMove(x, y + 4, i, 1, moves, g.board, p);
                    addMove(x, y + 5, i, 1, moves, g.board, p);
                }
            }
            if (!isEnemyPawnAt(x, y - 1, g.board, p)) {
                int tail = g.getPawn(x , y+2);
                addMove(x, y - 1, tail, 0, moves, g.board, p);
                if (!isSelfPawnAt(x, y - 1, g.board, p)) {
                    addMove(x, y - 2, i, 1, moves, g.board, p);
                    addMove(x, y - 3, i, 1, moves, g.board, p);
                }
            }
        }

        if (isSelfPawnAt(x + 1, y - 1, g.board, p) && isSelfPawnAt(x + 2, y - 2, g.board, p)) {
            //top-right cannon
            if (!isEnemyPawnAt(x + 3, y - 3, g.board, p)) {
                addMove(x + 3, y - 3, i, 0, moves, g.board, p);
                if (!isSelfPawnAt(x + 3, y - 3, g.board, p)) {
                    addMove(x + 4, y - 4, i, 1, moves, g.board, p);
                    addMove(x + 5, y - 5, i, 1, moves, g.board, p);
                }
            }
            if (!isEnemyPawnAt(x - 1, y + 1, g.board, p)) {
                int tail = g.getPawn(x + 2, y-2);
                addMove(x - 1, y + 1, tail, 0, moves, g.board, p);
                if (!isSelfPawnAt(x - 1, y + 1, g.board, p)) {
                    addMove(x - 2, y + 2, i, 1, moves, g.board, p);
                    addMove(x - 3, y + 3, i, 1, moves, g.board, p);
                }
            }
        }

        if (isSelfPawnAt(x + 1, y + 1, g.board, p) && isSelfPawnAt(x + 2, y + 2, g.board, p)) {
            //bottom right cannon
            if (!isEnemyPawnAt(x + 3, y + 3, g.board, p)) {
                addMove(x + 3, y + 3, i, 0, moves, g.board, p);
                if (!isSelfPawnAt(x + 3, y + 3, g.board, p)) {
                    addMove(x + 4, y + 4, i, 1, moves, g.board, p);
                    addMove(x + 5, y + 5, i, 1, moves, g.board, p);
                }
            }
            if (!isEnemyPawnAt(x - 1, y - 1, g.board, p)) {
                int tail = g.getPawn(x + 2, y + 2);
                addMove(x - 1, y - 1, tail, 0, moves, g.board, p);
                if (!isSelfPawnAt(x - 1, y - 1, g.board, p)) {
                    addMove(x - 2, y - 2, i, 1, moves, g.board, p);
                    addMove(x - 3, y - 3, i, 1, moves, g.board, p);
                }
            }
        }
    }
    return moves;
}
bool isSelfPawnAt(int x, int y, int board[8][8],Pawn::Side s) {
    if(x>7 || x<0 || y>7 || y<0)
        return false;
    if(board[x][y]==0) {
        return false;
    }
    if(s==Pawn::BLACK) return board[x][y]>0;
    return board[x][y]<0;
}

bool isSelfTownHallAt(int x, int y, Pawn::Side s) {
    if(s==Pawn::BLACK)
        return ( y==7 && x%2==1 );
    return (y==0 && x%2==0);
}

bool isEnemyTownHallAt(int x, int y, Pawn::Side s){
    if(s==Pawn::BLACK)
        return (y==0 && x%2==0);
    return (y==7 && x%2==1);
}


bool isEnemyPawnAt(int x, int y, int board[8][8],Pawn::Side s) {
    if(x>7 || x<0 || y>7 || y<0)
        return false;
    if(board[x][y]==0)
        return false;
    if(s!=Pawn::BLACK) return board[x][y]>0;
        return board[x][y]<0;
}

bool isEnemyAttacking(int x, int y, int board[8][8],Pawn::Side b) {
    if(b==Pawn::BLACK)
        return (isEnemyPawnAt(x+1,y,board,b) || isEnemyPawnAt(x+1,y-1,board,b) || isEnemyPawnAt(x,y-1,board,b) || isEnemyPawnAt(x-1,y-1,board,b) || isEnemyPawnAt(x-1,y,board,b));
    else
        return (isEnemyPawnAt(x+1,y,board,b) || isEnemyPawnAt(x+1,y+1,board,b) || isEnemyPawnAt(x,y+1,board,b) || isEnemyPawnAt(x-1,y+1,board,b) || isEnemyPawnAt(x-1,y,board,b));
}

void addMove(int x, int y, int i, int shot, vector<vector<int> >&v, int board[8][8], Pawn::Side b){
    if(x>7 || x<0 || y>7 || y<0 || isSelfPawnAt(x,y,board,b) || isSelfTownHallAt(x,y,b)) return;
//    cout<<"adding move: "<<x<<","<<y<<" "<<i<<" "<<shot<<endl;
    vector<int> curMove{i,shot,x,y};
    v.push_back(curMove);
    //cout<<i<<' '<<shot<<' '<<x<<' '<<y<<endl;
}


int c_heuristic(game g,Pawn::Side b,vector<int> move,int currH)//destroyedWhite is 0 when first town hall of white 1 is destroyed
{
    Pawn::Side s = box2Side(b);
    vector<int> destroyedBlack = g.destroyedBlacks;
    vector<int> destroyedWhite = g.destroyedWhites;
    int x = move[2], y = move[3], i = move[0], shot = move[1];
//    Pawn::Side enemySide = getEnemySide(b);
    Pawn::Side enemy = (b == Pawn::WHITE) ? (Pawn::BLACK) : (Pawn::WHITE);
    Pawn player = g.getSoldiers(b).at(i);
    int currX = player.getcorX();
    int currY = player.getcorY();
    if (enemy == Pawn::WHITE) {
      //destroy townhall of enemy
        if (isSelfTownHallAt(x, y, enemy) && destroyedWhite[x/2]==1) {//if the enemy townHall is present at the next move
            vector<Pawn> blacks = g.getBlack();
            int sum = 0, xx, yy, townHall = x / 2;
            //cout<<townHall;
            for (int i(0); i < blacks.size(); i++) {
                xx = blacks.at(i).getcorX();
                yy = blacks.at(i).getcorY();
                sum += h_blacks[townHall][xx][yy];
                destroyedWhite[townHall] = 0;
            }
            currH += (128 * 12 - sum);
        }
        else if (g.board[x][y] <0) {// if the enemy is present at the next move
            for (int i = 0; i < 4; i++)
                currH += (destroyedBlack[i] * h_whites[i][x][y] + SOLDIER_VAL);
        }
        //if shot was not made and white townhall was not destroyed by the current move
        if(shot==0 && destroyedWhite[x/2]!=0)
        {
            for(int i(0);i<4;i++) currH += destroyedWhite[i]*(h_blacks[i][x][y]-h_blacks[i][currX][currY]);
            //remove destroyedWhites is something weird happens with the code
        }
    }
    else {
        if (isSelfTownHallAt(x, y, enemy) && destroyedBlack[x/2]==1) {
            vector<Pawn> whites = g.getWhite();
            int sum = 0, xx, yy, townHall = x / 2;
            for (int i(0); i < whites.size(); i++) {
                xx = whites.at(i).getcorX();
                yy = whites.at(i).getcorY();
                sum += h_whites[townHall][xx][yy];
                destroyedBlack[townHall] = 0;
            }
            currH -= (128 * 12 - sum);
        }
        else if (g.board[x][y] > 0) {
            for (int i = 0; i < 4; i++)
                currH -= (destroyedWhite[i] * h_blacks[i][x][y] + SOLDIER_VAL);
        }
        if(shot==0 && destroyedBlack[x/2]!=0)
        {
            for(int i(0);i<4;i++) currH -= destroyedBlack[i]*(h_whites[i][x][y]-h_whites[i][currX][currY]);
        }
    }
    return currH;
}


game playMove(game gg,Pawn::Side s,vector<int> move) {
    //game g = gg;

    Pawn::Side enemySide = (s == Pawn::BLACK) ? (Pawn::WHITE) : (Pawn::BLACK);
    int x = move[2], y = move[3], i = move[0], shot = move[1];
    vector<Pawn> pawns = gg.getSoldiers(s);
    int j, curr_x, curr_y;
    int h = c_heuristic(gg,s,move,gg.heuristic);

    //kill the opponent at new position
    if (isEnemyPawnAt(x, y, gg.board, s)) {
    	vector<Pawn> enemies = gg.getSoldiers(enemySide);
        j = gg.getPawn(x, y); //NOT double checking!!
        enemies.erase(enemies.begin() + j);
        gg.setSoldiers(enemies, enemySide);
        if(shot==1)
            gg.board[x][y]=0;
    }
    //update position of pawn and board
    if (shot == 0) {
        curr_x = pawns.at(i).getcorX();
        curr_y = pawns.at(i).getcorY();
        gg.board[curr_x][curr_y] = 0;
        pawns.at(i).setcorX(x);
        pawns.at(i).setcorY(y);
        gg.setSoldiers(pawns,s);
        gg.board[x][y] = ( (s==Pawn::WHITE) ? (-i-1) : (i+1) );
    }
    //destroy the enemy townhall
    if(enemySide == Pawn::BLACK) {
        if (isSelfTownHallAt(x, y, enemySide)){
            gg.destroyedBlacks.at(x/2) = 0;
        }
    } else{
        if(isSelfTownHallAt(x,y, enemySide))
            gg.destroyedWhites.at(x/2) = 0;
    }
    gg.heuristic=h;
    //cout<<' '<<h<<endl;
    return gg;
}

game best_first(game g, Pawn::Side side) {
    vector<vector<int > > moves=getMoves(g,side);
    int max_h = -20000;
    int index = 0;
    for(int i=0; i< moves.at(0).size(); i++)
    {
        int temp = c_heuristic(g, side, moves[i], g.heuristic);
//        cout<<moves[1][i]<<endl;
        //cout<<temp<<' ';
        if(temp > max_h){
            max_h = temp;
            index = i;
        }
    }
    //cout<<endl;
    game gg = playMove(g, side, moves[index]);
    Pawn pawn = g.getSoldiers(side)[moves[index][0]];
    string m = ( (moves[index][1]==0) ? (" M ") : (" B ") );
//    cout<<moves[1][index]<<endl;
    cout<<"S "<< pawn.getcorX()<<" "<<pawn.getcorY()<<m<<moves[index][2]<<" "<<moves[index][3]<<endl;
    return gg;
}

int bestchild(game g, Pawn::Side side,vector<vector<int> > moves, bool last) {
    int max_h = -20000;
    int index = 0;
    for(int i=0; i< moves.at(0).size(); i++)
    {
        int temp = c_heuristic(g, side, moves[i], g.heuristic);
        if(temp>max_h){
            max_h = temp;
            index = i;
        }
    }
    if(last) return index;
    return max_h;
}


int worstchild(game g, Pawn::Side side,vector<vector<int> > moves, bool last) {
    int min_h = 20000;
    int index = 0;
    for(int i=0; i< moves.at(0).size(); i++)
    {
        int temp = c_heuristic(g, side, moves[i], g.heuristic);
        if(temp<min_h){
            min_h = temp;
            index = i;
        }
    }
    if(last) return index;
    return min_h;
}
Pawn::Side enemy(Pawn::Side side){ return (side==Pawn::WHITE) ? (Pawn::BLACK) : (Pawn::WHITE);}

int MinVal(game g, int alpha, int beta, int maxPlies, int numPlies, Pawn::Side p)
{
    int child,index(-1);
    int bestChild=10000;
    vector< vector <int> > children = getMoves(g,p);
    //check if no move is possible
    if(children.size()==0) return g.heuristic;
    if (numPlies==1)
        return worstchild(g, p, children,(maxPlies==1));

    for (int i(0);i<children.size();i++)
//    for (int i(0);i<NUM_MOVES;i++)
    {
        game gg = playMove(g,p,children[i]);
        child = MaxVal(gg,alpha,beta,maxPlies,numPlies-1,enemy(p));
        beta = min(beta,child);
        if(bestChild > child){
            index = i;
            bestChild = child;
        }
        if (alpha>=beta){
            //cout<<"pruning now "<<i<<endl;
            //if(maxPlies==numPlies)return index; else return bestChild;
            break;
        }
    }

    if(maxPlies==numPlies)return index; else return bestChild;
}

int MaxVal(game g, int alpha, int beta,int maxPlies, int numPlies, Pawn::Side p)
{
    int child,index(-1);
    int bestChild = -10000;
    vector< vector<int> > children = getMoves(g,p);
    if(children[0].size()==0) return g.heuristic;
    if (numPlies==1)
        return bestchild(g,p,children,(maxPlies==1));

    for (int i(0);i<children[0].size();i++)
//    for (int i(0);i<NUM_MOVES;i++)
    {
        game gg=playMove(g,p,children[i]);
        child = MinVal(gg,alpha,beta,maxPlies,numPlies-1,enemy(p));
        alpha = max(alpha,child);
        if(bestChild < child){
            index = i;
            bestChild = child;
        }
        if (alpha>=beta){
            //cout<<"pruning now "<<i<<endl;
            //if(maxPlies==numPlies)return index; else return bestChild;
            break;
        }
    }

    if(maxPlies==numPlies)return index; else return bestChild;
}

void print_heuristic()
{
    for(int i(0);i<8;i++) {
        for (int j(0); j < 8; j++)
            cout << h_whites[3][j][i] << '\t';
        cout << endl;
    }
}

void initialize_heuristic()
{
    for(int x(0); x<8; x++){
        for(int y(0); y<8; y++){
            int val_black = pow(2, 7 - y);
            h_blacks[0][x][y] = (x>y) ? (0) : (val_black) ;
            h_blacks[1][x][y] = (x>y+2 || x+y <2) ? (0) : (val_black) ;
            h_blacks[2][x][y] = (x>y+4 || x+y <4) ? (0) : (val_black) ;
            h_blacks[3][x][y] = (x>y+6 || x+y <6) ? (0) : (val_black) ;
            int val_white = pow(2, y);
            h_whites[0][x][y] = (x+6<y || x+y >8 ) ? (0) : (val_white) ;
            h_whites[1][x][y] = (x+4<y || x+y >10) ? (0) : (val_white) ;
            h_whites[2][x][y] = (x+2<y || x+y >12) ? (0) : (val_white) ;
            h_whites[3][x][y] = (x<y) ? (0) : (val_white) ;
        }
    }
}

int main()
{

    initialize_heuristic();
    char s='S',b;
    int x1,y1,x2,y2;
    int player,n,m,time;
    cin>>player>>n>>m>>time;

//    auto start = chrono::high_resolution_clock::now();
    game g;
//    for(int i=0; i<10000; i++){
//    	game gg = playMove(g,Pawn::BLACK,0,1,0,3);
//    }
//    auto stop = chrono::high_resolution_clock::now();
//    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
//    cout<<"time taken by playMove: "<<duration.count()<<endl;

    if(player==2){
    	while(true){
    		cin>>s>>x1>>y1>>b>>x2>>y2;
        vector<int> move{g.getPawn(x1, y1), (int)(b=='B'), x2, y2};
    		g = playMove(g, Pawn::BLACK, move);

    		vector<vector<int> > moves = getMoves(g, Pawn::WHITE);
	    	vector<Pawn> pawns = g.getSoldiers(Pawn::WHITE);
//    		if(pawns.size()<8)
	    	{
			MAX_PLIES = 5;
    			vector<Pawn> enemies = g.getSoldiers(Pawn::BLACK);
			if(enemies.size()<8){
				MAX_PLIES = 5;
			}
    		}
    		int index = MinVal(g, -10000, 10000, MAX_PLIES, MAX_PLIES, Pawn::WHITE);

    		Pawn pawn = pawns[moves[0][index]];
    		g = playMove(g,Pawn::WHITE,moves[index]);
   		string m = ( (moves[1][index]==0) ? (" M ") : (" B ") );
		cout<<"S "<<pawn.getcorX()<<" "<<pawn.getcorY()<<m<<moves[index][2]<<" "<<moves[index][3]<<endl;

    	}
    	return 0;
    }
    vector<int> temp_move{5,0,1,6};
    g = playMove(g,Pawn::BLACK,temp_move);
    cout<<"S 2 7 M 1 6"<<endl;
    while(true){
    	cin>>s>>x1>>y1>>b>>x2>>y2;
      vector<int> move{g.getPawn(x1, y1), (int)(b=='B'), x2, y2};
    	g = playMove(g, Pawn::WHITE, move);

    	vector<vector<int> > moves = getMoves(g, Pawn::BLACK);
    	vector<Pawn> pawns = g.getSoldiers(Pawn::BLACK);
    	if(pawns.size()<=8)
    	{
		MAX_PLIES = 5;
    		vector<Pawn> enemies = g.getSoldiers(Pawn::WHITE);
		if(enemies.size()<8){
			MAX_PLIES = 6;
		}
	}
    	int index = MaxVal(g, -10000, 10000, MAX_PLIES, MAX_PLIES, Pawn::BLACK);
//    	cout<<index<<endl;

    	Pawn pawn = pawns[moves[0][index]];
    	g = playMove(g,Pawn::BLACK,moves[index]);
   	string m = ( (moves[index][1]==0) ? (" M ") : (" B ") );
	cout<<"S "<<pawn.getcorX()<<" "<<pawn.getcorY()<<m<<moves[index][2]<<" "<<moves[index][3]<<endl;

    }
    return 0;
}
