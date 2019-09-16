#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

int h_blacks[4][8][8];
int h_whites[4][8][8];
int MAX_PLIES = 3;

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
    enum BOX{
        EMPTY,BLACK,WHITE
    };
    game::BOX board[8][8];
    game()
    {
        heuristic=0;
        for(int i(0);i<8;i++) for(int j(0);j<8;j++)board[i][j]=game::EMPTY;
        for(int i(0);i<4;i++) { destroyedWhites.push_back(1);destroyedBlacks.push_back(1);}
        for(int i=0; i<8; i+=2) {
            for (int j = 5; j < 8; j++) {
                blacks.push_back(new Pawn(Pawn::BLACK, i, j));
                whites.push_back(new Pawn(Pawn::WHITE, i+1, 7-j));
                board[i][j]=BOX::BLACK;
                board[i+1][7-j]=BOX ::WHITE;
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
    static BOX getBox(Pawn p){
        if (p.getside()==Pawn::WHITE)
            return WHITE;
        else
            return BLACK;
    }
    int getPawn(int curr_x, int curr_y, Pawn::Side b){
        //cout<<"i am here 55"<<endl ;
        if(b==Pawn::BLACK)
        {
            for(int i(0);i<blacks.size();i++)
            {
                if((blacks[i].getcorX()==curr_x)&&(blacks[i].getcorY()==curr_y))
                    return i;
            }
        }
        else
        {
            for(int i(0);i<whites.size();i++)
            {
                if((whites[i].getcorX()==curr_x)&&(whites[i].getcorY()==curr_y)) {
                    return i;
                }
            }
        }
        return (-1);
    }

    void move(Pawn &s, int curr_x, int curr_y, int new_x, int new_y){
        board[curr_x][curr_y]=EMPTY;
        board[new_x][new_y]=getBox(s);
        s.setcorX(new_x);
        s.setcorY(new_y);
    }
private:
    vector<Pawn> blacks;
    vector<Pawn> whites;
};

Pawn::Side box2Side(game::BOX p){
    if(p==game::BLACK)
        return Pawn::BLACK;
    return Pawn::WHITE;
}

Pawn::Side getEnemySide(game ::BOX p){return  (box2Side(p)== Pawn::BLACK) ? (Pawn::WHITE) : (Pawn::BLACK);}

void printBoard(game::BOX b[8][8]){
    for(int i(0);i<8;i++)
    {
        for(int j(0);j<8;j++)
        {
            cout<<b[j][i]<<' ';
        }
        cout<<endl;
    }
}

void addMove(int x, int y, int i,int shot, vector<vector<int> >&moves, game::BOX board[8][8], game::BOX b);
bool isEnemyPawnAt(int x, int y, game::BOX board[8][8],game::BOX b);
bool isEnemyAttacking(int x, int y, game::BOX board[8][8],game::BOX b);
bool isSelfPawnAt(int x, int y, game::BOX board[8][8],game::BOX b);
bool isSelfTownHallAt(int x, int y, game::BOX b);
bool isEnemyTownHallAt(int x, int y, game::BOX b);
void initialize_heuristic();
int MaxVal(game game, int alpha, int beta,int maxPlies, int numPlies, Pawn::Side p);
game::BOX sideToBox(Pawn::Side side);

vector<vector<int> > getMoves(game g, Pawn::Side p) {
    vector<Pawn> soldiers = g.getSoldiers(p);
    int x, y;
    vector<int> move(0, 0);
    vector<vector<int> > moves(4, move);
    int isBlack = -1;
    if (p == Pawn::BLACK) isBlack = 1;
    for (int i = 0; i < soldiers.size(); i++) {

        x = soldiers[i].getcorX();
        y = soldiers[i].getcorY();
        game::BOX curBox = g.board[x][y];
        // NORMAL MOVES
        addMove(x - 1, y - 1 * isBlack, i, 0, moves, g.board, curBox);
        addMove(x, y - 1 * isBlack, i, 0, moves, g.board, curBox);
        addMove(x + 1, y - 1 * isBlack, i, 0, moves, g.board, curBox);
        // RETREAT MOVES
        if (isEnemyAttacking(x, y, g.board, curBox)) {
            addMove(x - 2, y + 2 * isBlack, i, 0, moves, g.board, curBox);
            addMove(x, y + 2 * isBlack, i, 0, moves, g.board, curBox);
            addMove(x + 2, y + 2 * isBlack, i, 0, moves, g.board, curBox);
        }
        // SIDE MOVES
        if (isEnemyPawnAt(x - 1, y, g.board, curBox)) {
            addMove(x - 1, y, i, 0, moves, g.board, curBox);
        }
        if (isEnemyPawnAt(x + 1, y, g.board, curBox)) {
            addMove(x + 1, y, i, 0, moves, g.board, curBox);
        }
	if(isEnemyTownHallAt(x-1,y,curBox)){
            addMove(x - 1, y, i, 0, moves, g.board, curBox);
        }
        if(isEnemyTownHallAt(x+1,y,curBox)){
            addMove(x + 1, y, i, 0, moves, g.board, curBox);
        }

        //CANNON MOVES
        if (isSelfPawnAt(x + 1, y, g.board, curBox) && isSelfPawnAt(x + 2, y, g.board, curBox)) {
            //Horizontal cannon
            if (!isEnemyPawnAt(x + 3, y, g.board, curBox)) {
                // cannon move
                addMove(x + 3, y, i, 0, moves, g.board, curBox);
                //cannon shot
                if (!isSelfPawnAt(x + 3, y, g.board, curBox)) {
                    addMove(x + 4, y, i, 1, moves, g.board, curBox);
                    addMove(x + 5, y, i, 1, moves, g.board, curBox);
                }
            }
            if (!isEnemyPawnAt(x - 1, y, g.board, curBox)) {
                int tail = g.getPawn(x + 2, y, box2Side(curBox));
                addMove(x - 1, y, tail, 0, moves, g.board, curBox);
                if (!isSelfPawnAt(x - 1, y, g.board, curBox)) {
                    addMove(x - 2, y, i, 1, moves, g.board, curBox);
                    addMove(x - 3, y, i, 1, moves, g.board, curBox);
                }
            }
        }

        if (isSelfPawnAt(x, y+1, g.board, curBox) && isSelfPawnAt(x, y+2, g.board, curBox)) {
            //vertical cannon
            if (!isEnemyPawnAt(x, y + 3, g.board, curBox)) {
                addMove(x, y + 3, i, 0, moves, g.board, curBox);
                if (!isSelfPawnAt(x, y + 3, g.board, curBox)) {
                    addMove(x, y + 4, i, 1, moves, g.board, curBox);
                    addMove(x, y + 5, i, 1, moves, g.board, curBox);
                }
            }
            if (!isEnemyPawnAt(x, y - 1, g.board, curBox)) {
                int tail = g.getPawn(x , y+2, box2Side(curBox));
                addMove(x, y - 1, tail, 0, moves, g.board, curBox);
                if (!isSelfPawnAt(x, y - 1, g.board, curBox)) {
                    addMove(x, y - 2, i, 1, moves, g.board, curBox);
                    addMove(x, y - 3, i, 1, moves, g.board, curBox);
                }
            }
        }

        if (isSelfPawnAt(x + 1, y - 1, g.board, curBox) && isSelfPawnAt(x + 2, y - 2, g.board, curBox)) {
            //top-right cannon
            if (!isEnemyPawnAt(x + 3, y - 3, g.board, curBox)) {
                addMove(x + 3, y - 3, i, 0, moves, g.board, curBox);
                if (!isSelfPawnAt(x + 3, y - 3, g.board, curBox)) {
                    addMove(x + 4, y - 4, i, 1, moves, g.board, curBox);
                    addMove(x + 5, y - 5, i, 1, moves, g.board, curBox);
                }
            }
            if (!isEnemyPawnAt(x - 1, y + 1, g.board, curBox)) {
                int tail = g.getPawn(x + 2, y-2, box2Side(curBox));
                addMove(x - 1, y + 1, tail, 0, moves, g.board, curBox);
                if (!isSelfPawnAt(x - 1, y + 1, g.board, curBox)) {
                    addMove(x - 2, y + 2, i, 1, moves, g.board, curBox);
                    addMove(x - 3, y + 3, i, 1, moves, g.board, curBox);
                }
            }
        }

        if (isSelfPawnAt(x + 1, y + 1, g.board, curBox) && isSelfPawnAt(x + 2, y + 2, g.board, curBox)) {
            //bottom right cannon
            if (!isEnemyPawnAt(x + 3, y + 3, g.board, curBox)) {
                addMove(x + 3, y + 3, i, 0, moves, g.board, curBox);
                if (!isSelfPawnAt(x + 3, y + 3, g.board, curBox)) {
                    addMove(x + 4, y + 4, i, 1, moves, g.board, curBox);
                    addMove(x + 5, y + 5, i, 1, moves, g.board, curBox);
                }
            }
            if (!isEnemyPawnAt(x - 1, y - 1, g.board, curBox)) {
                int tail = g.getPawn(x + 2, y + 2, box2Side(curBox));
                addMove(x - 1, y - 1, tail, 0, moves, g.board, curBox);
                if (!isSelfPawnAt(x - 1, y - 1, g.board, curBox)) {
                    addMove(x - 2, y - 2, i, 1, moves, g.board, curBox);
                    addMove(x - 3, y - 3, i, 1, moves, g.board, curBox);
                }
            }
        }
    }
    return moves;
}
bool isSelfPawnAt(int x, int y, game::BOX board[8][8],game::BOX b) {
    if(x>7 || x<0 || y>7 || y<0)
        return false;
    if(board[x][y]==game::EMPTY) {
        return false;
    }
    return (board[x][y]==b);
}

bool isSelfTownHallAt(int x, int y, game::BOX b) {
    if(b==game::BLACK)
        return ( y==7 && x%2==1 );
    return (y==0 && x%2==0);
}

bool isEnemyTownHallAt(int x, int y, game::BOX b){
    if(b==game::BLACK)
        return (y==0 && x%2==0);
    return (y==7 && x%2==1);
}


bool isEnemyPawnAt(int x, int y, game::BOX board[8][8],game::BOX b) {
    if(x>7 || x<0 || y>7 || y<0)
        return false;
    if(board[x][y]==game::EMPTY)
        return false;
    return (board[x][y]!=b);
}

bool isEnemyAttacking(int x, int y, game::BOX board[8][8],game::BOX b) {
    if(b==game::BLACK)
        return (isEnemyPawnAt(x+1,y,board,b) || isEnemyPawnAt(x+1,y-1,board,b) || isEnemyPawnAt(x,y-1,board,b) || isEnemyPawnAt(x-1,y-1,board,b) || isEnemyPawnAt(x-1,y,board,b));
    else
        return (isEnemyPawnAt(x+1,y,board,b) || isEnemyPawnAt(x+1,y+1,board,b) || isEnemyPawnAt(x,y+1,board,b) || isEnemyPawnAt(x-1,y+1,board,b) || isEnemyPawnAt(x-1,y,board,b));
}

void addMove(int x, int y, int i, int shot, vector<vector<int> >&v, game::BOX board[8][8], game::BOX b){
    if(x>7 || x<0 || y>7 || y<0 || isSelfPawnAt(x,y,board,b) || isSelfTownHallAt(x,y,b)) return;
//    cout<<"adding move: "<<x<<","<<y<<" "<<i<<" "<<shot<<endl;
    v.at(0).push_back(i); v.at(1).push_back(shot); v.at(2).push_back(x); v.at(3).push_back(y);
    //cout<<i<<' '<<shot<<' '<<x<<' '<<y<<endl;
}



int c_heuristic(game g,game::BOX b,int i,int shot, int x,int y,int currH)//destroyedWhite is 0 when first town hall of white 1 is destroyed
{
    Pawn::Side s = box2Side(b);
    vector<int> destroyedBlack = g.destroyedBlacks;
    vector<int> destroyedWhite = g.destroyedWhites;
//    Pawn::Side enemySide = getEnemySide(b);
    game::BOX enemy = (b == game::WHITE) ? (game::BLACK) : (game::WHITE);
    Pawn player = g.getSoldiers(s).at(i);
    int currX = player.getcorX();
    int currY = player.getcorY();
    if (enemy == game::WHITE) {

        if (isSelfTownHallAt(x, y, enemy) && destroyedWhite[x/2]==1) {
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
        else if (g.board[x][y] == enemy) {
            for (int i = 0; i < 4; i++)
                currH += (destroyedBlack[i] * h_whites[i][x][y]+3);
        }
        if(shot==0 && destroyedWhite[x/2]!=0)
        {
            for(int i(0);i<4;i++) currH+=(h_blacks[i][x][y]-h_blacks[i][currX][currY]);
        }
    } else {
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
        else if (g.board[x][y] == enemy) {
            for (int i = 0; i < 4; i++)
                currH -= (destroyedWhite[i] * h_blacks[i][x][y]+3);
        }
        if(shot==0 && destroyedBlack[x/2]!=0)
        {
            for(int i(0);i<4;i++) currH-=(h_whites[i][x][y]-h_whites[i][currX][currY]);
        }
    }
    return currH;
}


game playMove(game g,game::BOX b,int i, int shot, int x, int y) {
    game gg = g;//gg is good game;
    Pawn::Side s = box2Side(b);
    vector<Pawn> pawns = gg.getSoldiers(s);
    int j, curr_x, curr_y;
    int h=c_heuristic(g,b,i,shot,x,y,g.heuristic);
    Pawn::Side enemySide = (s == Pawn::BLACK) ? (Pawn::WHITE) : (Pawn::BLACK);
    vector<Pawn> enemies = gg.getSoldiers(enemySide);
    if (shot == 0) {
        curr_x = pawns.at(i).getcorX();
        curr_y = pawns.at(i).getcorY();
        gg.board[curr_x][curr_y] = game::EMPTY;
        pawns.at(i).setcorX(x);
        pawns.at(i).setcorY(y);
        gg.setSoldiers(pawns,s);
        gg.board[x][y] = b;
    }
    if (isEnemyPawnAt(x, y, g.board, b)) {
        j = g.getPawn(x, y, enemySide);
        enemies.erase(enemies.begin() + j);
        gg.setSoldiers(enemies, enemySide);
        if(shot==1)
            gg.board[x][y]=game::EMPTY;
    }
//    game::BOX enemy = () ? () : b;
    if(enemySide == Pawn::BLACK) {
        if (isSelfTownHallAt(x, y, game::BLACK)){
            gg.destroyedBlacks.at(x/2) = 0;
        }
    } else{
        if(isSelfTownHallAt(x,y, game::WHITE))
            gg.destroyedWhites.at(x/2) = 0;
    }
    gg.heuristic=h;
    //cout<<' '<<h<<endl;
    return gg;
}


//int MaxVal(int state[8][8], int alpha,int beta, int numPlies);


game best_first(game g, Pawn::Side side) {
    vector<vector<int > > moves=getMoves(g,side);
    int max_h = -20000;
    int index = 0;
    for(int i=0; i< moves.at(0).size(); i++)
    {
        int temp = c_heuristic(g, sideToBox(side), moves[0][i], moves[1][i], moves[2][i], moves[3][i], g.heuristic);
//        cout<<moves[1][i]<<endl;
        //cout<<temp<<' ';
        if(temp > max_h){
            max_h = temp;
            index = i;
        }
    }
    //cout<<endl;
    game gg = playMove(g,sideToBox(side), moves[0][index], moves[1][index], moves[2][index], moves[3][index]);
    Pawn pawn = g.getSoldiers(side)[moves[0][index]];
    string m = ( (moves[1][index]==0) ? (" M ") : (" B ") );
//    cout<<moves[1][index]<<endl;
    cout<<"S "<< pawn.getcorX()<<" "<<pawn.getcorY()<<m<<moves[2][index]<<" "<<moves[3][index]<<endl;
    return gg;
}

int bestchild(game g, Pawn::Side side,vector<vector<int> > moves, bool last) {
    int max_h = -20000;
    int index = 0;
    for(int i=0; i< moves.at(0).size(); i++)
    {
        int temp = c_heuristic(g, sideToBox(side), moves[0][i], moves[1][i], moves[2][i], moves[3][i], g.heuristic);
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
        int temp = c_heuristic(g, sideToBox(side), moves[0][i], moves[1][i], moves[2][i], moves[3][i], g.heuristic);
        if(temp<min_h){
            min_h = temp;
            index = i;
        }
    }
    if(last) return index;
    return min_h;
}

game::BOX sideToBox(Pawn::Side side) { return (side==Pawn::WHITE) ? (game::WHITE) : (game::BLACK);}
game::BOX enemySidetoBox(Pawn::Side side) { return (side==Pawn::WHITE) ? (game::BLACK) : (game::WHITE);}
Pawn::Side enemy(Pawn::Side side){ return (side==Pawn::WHITE) ? (Pawn::BLACK) : (Pawn::WHITE);}

int MinVal(game g, int alpha, int beta,int maxPlies, int numPlies,Pawn::Side p)
{
    int child,index(0);
    int bestChild=10000;
    vector< vector <int> > children = getMoves(g,p);
    if (numPlies==1)
        return worstchild(g, p, children,(maxPlies==1));

    for (int i(0);i<children[0].size();i++)
    {
        game gg=playMove(g,enemySidetoBox(p),children[0][i],children[1][i],children[2][i],children[3][i]);
        child = MaxVal(gg,alpha,beta,maxPlies,numPlies-1,enemy(p));
        beta = min(beta,child);
        if(bestChild>child){
            index = i;
            bestChild = child;
        }
        if (alpha>=beta){
            cout<<"pruning now"<<endl;
            if(maxPlies==numPlies)return i; else return child;
        }
    }

    if(maxPlies==numPlies)return index; else return bestChild;
}

int MaxVal(game g, int alpha, int beta,int maxPlies, int numPlies, Pawn::Side p)
{
    int child,index(0);
    int bestChild=-10000;
    vector< vector <int> > children = getMoves(g,p);
    if (numPlies==1)
        return bestchild(g,p,children,(maxPlies==1));

    for (int i(0);i<children[0].size();i++)
    {
        game gg=playMove(g,enemySidetoBox(p),children[0][i],children[1][i],children[2][i],children[3][i]);
        child = MinVal(gg,alpha,beta,maxPlies,numPlies-1,enemy(p));
//        cout<<i<<": "<<child<<endl;
        beta = max(beta,child);
        if(bestChild < child){
            index = i;
            bestChild = child;
        }
        if (alpha>=beta){
            cout<<"pruning now"<<endl;
            if(maxPlies==numPlies)return i; else return child;
        }
    }

//    cout<<index<<" best "<<bestChild<<endl;
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
    game g;
    char s='S',b;
    int x1,y1,x2,y2;
    int player,n,m,time;
    cin>>player>>n>>m>>time;

    while(true){
    	//g = best_first(g, Pawn::BLACK);
    	vector<vector<int> > moves = getMoves(g, Pawn::BLACK);
    	int index = MaxVal(g, -10000, 10000, MAX_PLIES, MAX_PLIES, Pawn::BLACK);

    	for(int i=0; i<moves[0].size();i++){
	    	string m = ( (moves[1][i]==0) ? (" M ") : (" B ") );
	    	Pawn pawn = g.getSoldiers(Pawn::BLACK)[moves[0][i]];
//    		cout<<i<<": S "<<pawn.getcorX()<<" "<<pawn.getcorY()<<m<<moves[2][i]<<" "<<moves[3][i]<<endl;
    	}
    	Pawn pawn = g.getSoldiers(Pawn::BLACK)[moves[0][index]];
    	g = playMove(g,game::BLACK,moves[0][index],moves[1][index],moves[2][index],moves[3][index]);
   	string m = ( (moves[1][index]==0) ? (" M ") : (" B ") );
	cout<<"S "<<pawn.getcorX()<<" "<<pawn.getcorY()<<m<<moves[2][index]<<" "<<moves[3][index]<<endl;
//    	cout<<index<<endl;
    	cin>>s>>x1>>y1>>b>>x2>>y2;
    	g = playMove(g, game::WHITE, g.getPawn(x1, y1, Pawn::WHITE), (int)(b=='B'), x2, y2);
    }
    return 0;
}
