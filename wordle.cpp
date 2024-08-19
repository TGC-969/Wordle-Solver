#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<utility>
#include<cmath>
#include<map>
#include<set>
#include<algorithm>
using namespace std;
vector<string> wordList;
vector<string> possOutputs;
map<vector<vector<string>>, vector<string>> possibleOutputsfc;
map<vector<string>, int>  isCalcDoThis;
map<vector<vector<string>>, int> isCalculated;
map<vector<string>, string> doThisdp;
void loadList(vector<string>& v){
	string word;
	ifstream inputFile("valid-wordle-words.txt");
	while(getline(inputFile, word)){
		v.push_back(word);
	}
	inputFile.close();
}
string convertToBase3(int n){
	string s;
	while(n>0){
		s+=to_string(n%3);
		n/=3;
	}
	while(s.size()<5){
		s+='0';
	}
	reverse(s.begin(),s.end());
	return s;
}
void possibleOutputs(){
	for(int i=0;i<243;i++){
		possOutputs.push_back(convertToBase3(i));
	}
	return;
}
bool isPossibleWord(string word, string input, string output){
	for(int j=0;j<5;j++){
		if(output[j]=='0' && word[j]!=input[j]){
			return false;
		}
	}
	int mark[5]={0};
	for(int j=0;j<5;j++){
		if(output[j]=='1'){
			int flag = 0;
			for(int k=0;k<5;k++){
				if(input[j]==word[k] && j!=k && output[k]!='0' && mark[k]!=1){
					//cout << j << " " << k << endl;
					flag=1;
					mark[k]=1;
					break;
				}
			}
			if(flag==0){
				return false;
			}
		}
	}
	for(int j=0;j<5;j++){
		if(output[j]=='2'){
			for(int k=0;k<5;k++){
				if(word[k]==input[j] && mark[k]!=1 && output[k]!='0'){
					return false;
				}
			}
		}
	}
	return true;
}
bool containsUnique(string s){
 set<char> st;
 for(int i=0;i<5;i++){
 	st.insert(s[i]);
 }
 return (s.size()==st.size());
}
string getOutput(string word, string input){
	string output = "*****";
	int mark[5]={0};
	for(int i=0;i<5;i++){
		if(word[i]==input[i]){
			output[i] = '0';
		}
	}
	for(int i=0;i<5;i++){
		if(output[i]=='*'){
			for(int j=0;j<5;j++){
				if(input[i]==word[j] && mark[j]!=1 && i!=j && output[j]!='0'){
					//cout << i << " " << j << endl;
					output[i] = '1';
					mark[j] = 1;
					break;
			}
		}
		}
	}
	for(int i=0;i<5;i++){
		if(output[i]=='*'){
			output[i]='2';
		}
	}
	return output;
}
string doThis(vector<string> possWordList){
	if(isCalcDoThis[possWordList]==1){
		return doThisdp[possWordList];
	}
	isCalcDoThis[possWordList] = 1;
	map<pair<string,string>, int> mp;
	map<string, int> totCnt;
	for(int i=0;i<possWordList.size();i++){
		for(int j=0;j<possOutputs.size();j++){
			int cnt = 0;
			for(int k=0;k<possWordList.size();k++){
				if(isPossibleWord(possWordList[k], possWordList[i], possOutputs[j])){
					cnt++;
				}
			}
			mp[make_pair(possWordList[i],possOutputs[j])] = cnt;
			totCnt[possWordList[i]]+=cnt;
		}
		//cout << i << endl;
	}
	//cout << "Completed finding possible outputs" << endl;
	//calculating entropy
	map<string, double> entropy;
	double maxEntropy=0;
	string ans;
	for(int i=0;i<possWordList.size();i++){
		entropy[possWordList[i]] = 0;
		//cout << totCnt[wordList[i]] << endl;
		for(int j=0;j<243;j++){
			//cout << mp[make_pair(wordList[i],possOutputs[j])] << endl;
			double prob = (double) mp[make_pair(possWordList[i],possOutputs[j])]/ (double) possWordList.size();
			if(prob!=0){
				entropy[possWordList[i]] += (-prob*log2(prob));
			}
		}
		//cout << possWordList[i]<< " " << entropy[possWordList[i]] << endl;
		if(entropy[possWordList[i]] >= maxEntropy){
			maxEntropy = entropy[possWordList[i]];
			ans = possWordList[i];
		}
	}
	return doThisdp[possWordList] = ans;
}


int main(){
	loadList(wordList);
	map<pair<string,string>, int> mp;
	map<string, int> totCnt;
	possibleOutputs();
	//cout << isPossibleWord("retro","error","11112");
//	cout << getOutput("rebut","rebut") << endl;
// 	return 0;
	vector<string> possWordList = wordList;
	string output,bestStart = "Default";
	map<string, double> averageMoves;
	averageMoves[bestStart] = 7;
	double minMoves = 7;
	//getOutput("rebut","cigar");
	for(int i=2063;i<wordList.size();i++){
		string input = wordList[i];
		//cout << "This is my Starting Word: " << input << " " << endl;
		int cnt = 0;
		for(int j=0;j<wordList.size();j++){
			string word = wordList[j];
			possWordList = wordList;
			input = wordList[i];
			//cout << "I'm going to guess this word "<< word << "(" << j << ")" << endl;
			do{
				//cout << "I think the word is this: "<< input << endl;
				output = getOutput(word, input);
				//cout << "This is the output I got "<< output << endl;
				vector<string> v;
				vector<vector<string>> p(3);
				p[0].push_back(input);
				p[1].push_back(output);
				p[2] = possWordList;
				if(isCalculated[p]==1){
					v = possibleOutputsfc[p];
				}
				else{
					for(int i=0;i<possWordList.size();i++){
						if(isPossibleWord(possWordList[i],input, output)){
							v.push_back(possWordList[i]);
						}
					}
					possibleOutputsfc[p] = v;
					isCalculated[p] = 1;
				}
				if(find(v.begin(),v.end(),word)==v.end()){
					cout << "F! we elemenated the required word" << endl;
					return 0;
				}
				possWordList = v;
				input = doThis(possWordList);
				if(possWordList.size()==0){
					cout << "Looks like we have a problem!" << endl;
					return 0;
				}
				cnt++;
			  } while(output!="00000");
			  //cout << endl;
		}
		averageMoves[wordList[i]] = (double)cnt/(double) wordList.size();
		//cout << "The average number of Moves for the word " << wordList[i] << " are " << averageMoves[wordList[i]] << endl;
		if(averageMoves[wordList[i]] < minMoves){
			minMoves = averageMoves[wordList[i]];
			bestStart = wordList[i];
		}
		//cout << endl;
		cout << minMoves << " " << bestStart<< "\n";
	}
	cout << bestStart << endl;
	
	return 0;

	
	
	
}