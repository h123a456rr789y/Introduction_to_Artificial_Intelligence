#include<iostream>
#include<bits/stdc++.h>

using namespace std;

vector<string>word[50];
struct word_block{
	int x;
	int y;
	int len;
	string dir;
	int cro;
	friend bool operator < (const word_block& a,const word_block& b){
		//return a.cro > b.cro;							//heuristic with cross amount
		return word[a.len].size() < word[b.len].size();	//heuristic with word with same length
	}
};

char a[50][50];  //the board
int cross[50][50];// count the cross
int max_x,max_y,flag,node_cnt,ans_cnt;
set<string>use; //check for the word used or not 
vector<word_block>w; //word content

void dfs(int n){
	if(flag)return;//answer found
	node_cnt++;//add new node
	if(n==(int)w.size()){// all words are  already assinged and print them out  
		cout<<endl;
		for(int i = 0 ; i < max_x+1 ; i++){
			cout<<' ';
			for(int j = 0 ; j < max_y+1 ; j++){
				cout<<a[j][i]<<' ';
			}
			cout<<endl;
		}
		flag = 1;//answer found
		cout<<endl;
		//ans_cnt++;
		return;
	}
	int x=w[n].x;
	int y=w[n].y;
	int len=w[n].len;
	string dir=w[n].dir;
	int xs=0,ys=0;// x shift and y shift with direction 
	if(dir == "A"){
		xs = 1;
	}
	else{
		ys = 1;
	}
	for(auto i:word[len]){// find throught all words consist current needed length
		if(use.find(i)!=use.end())continue; // find unused words
		int found=0;
		int nx=x,ny=y;
		for(int j=0;j<len;j++){
			if(a[nx][ny]!='_'&&a[nx][ny]!=i[j]){// check if there is a space for the word
				found=1;
				break;
			}
			nx += xs;
			ny += ys;
		}
		nx=x,ny=y;
		if(found==0){
			vector<pair<int,int> >sp;//assigned the letters into the node
			nx=x;
			ny=y;
			for(int j=0;j<len;j++){
				if(a[nx][ny]=='_'){
					sp.push_back({nx,ny});
					a[nx][ny]=i[j];
				}
				nx+=xs;
				ny+=ys;
			}
			use.insert(i);
			dfs(n+1);
			use.erase(i);
			for(auto j:sp){// remove assigned letter from the node
				a[j.first][j.second]='_';
			}
		}
	}
}



int main(){
	string s;
	fstream file;
	file.open("English words 3000.txt",ios::in); // read the file
	string input;
	while(file >> input){
		word[input.length()].push_back(input);
	}
	while(getline(cin,s)){
		stringstream ss(s);
		int x,y,len;
		string dir;
		max_x=0;
		max_y=0;
		w.clear();
		while(ss >> x >> y >>len >>dir){ 	// get the input x, y, length, direction required
			w.push_back({x,y,len,dir,0});
			max_x=max(max_x,x+(dir=="A")*len);
			max_y=max(max_y,y+(dir=="D")*len);
			int tmp_x=x,tmp_y=y;
			while(len--){                   //check the cross from the words
				cross[tmp_x][tmp_y]++;
				if(dir=="A"){
					tmp_x++;
				}
				else{
					tmp_y++;
				}
			}
		}
		for(auto &i:w){
			int tmp_x=i.x,tmp_y=i.y;
			for(int j=0;j<i.len ;j++){
				i.cro += cross[tmp_x][tmp_y] - 1; // count cross heuristic
				if(i.dir=="A"){
					tmp_x++;
				}
				else {
					tmp_y++;
				}
			}
		}
		sort(w.begin(),w.end());//operate heuristic
		for(int i = 0 ; i < max_x+1 ; i++){ 	//initialize the broad
			for(int j = 0 ; j < max_y + 1 ; j++){
				a[i][j]='_';
			}
		}
		flag=0;		//intialize for no ans 
		//ans_cnt=0; // count the amount of answers 
		dfs(0);
		use.clear(); //intialize the used words 
		cout <<"Node generated: "<< node_cnt << endl; // print the nodes generated 
		//cout <<"The total possible answer amount: "<< ans_cnt << endl; // print the answers amount 

	}
}
