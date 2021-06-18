#include <iostream>
#include <cstdlib> // atoi, atol, malloc
#include <fstream>
#include <cstring>
#include <string>
#include <bitset>
#include <vector>
#include <iterator>
using namespace std;

string modulo(int num, int dlen, string generator){
	string dword = "";
	int glen = generator.size();
	if(dlen == 4){
		bitset<4> b(num);
		dword = b.to_string();
	}
	else{
		bitset<8> b(num);
		dword = b.to_string();
	}
	for(int i=0;i<glen-1;i++)
		dword += "0";

	string quotient="";
	string remainder="";

	string s1=dword.substr(0, glen);
	string s2=generator;
	for(int i=0; i<dlen; i++){
		if(s1[0]=='1'){
			string tmp="";
			quotient += "1";
			for(int j=0;j<glen;j++){ // xor 
				if(s1[j]==s2[j])
					tmp += "0";
				else
					tmp += "1";
			}
			if(i==dlen-1)
				remainder = tmp;
			tmp.push_back(dword[i+glen]); // shift 1 bit left
			tmp.erase(0, 1); // glen크기로 잘라줌
			s1 = tmp;
		}
		else{
			quotient += "0";
			if(i==dlen-1)
				remainder = s1;
			s1.push_back(dword[i+glen]);
			s1.erase(0, 1);
		}
	}
	remainder.erase(0,1);
	dword.erase(dlen, glen);
	return dword+remainder;
}
int main(int argc, char* argv[]){
	if(argc != 5){
		cout << "usage: ./crc_encoder input_file output_file generator dataword_size" << endl;
	}
	ifstream fin(argv[1], ios::in|ios::binary);
	if(!fin){
		cout << "input file open error." << endl;
		exit(1);
	}
	ofstream fout(argv[2], ios::out|ios::binary);
	if(!fout){
		cout << "output file open error." << endl;
		exit(1);
	}

	string generator = argv[3];
	int dataword_size = atoi(argv[4]);
	if(dataword_size!=4 && dataword_size!=8){
		cout << "dataword size must be 4 or 8." << endl;
		exit(1);
	}
	vector<unsigned char> buf(istreambuf_iterator<char>(fin), (istreambuf_iterator<char>()));
	string content = "";

	for(int i=0;i<(int)buf.size();i++){
		int c = (int)buf[i]; // 여기에서 .jpg는 안되고 .txt는 되는 문제 발생했었음. 원래)char c = buf[i];
		bitset<8> bit(c);
		if(dataword_size==4){
			string left = modulo(c/16, 4, generator);
			string right = modulo(c%16, 4, generator);
			content += left+right;
		}
		else{ // dataword==8
			string s = modulo(c, 8, generator);
			content += s;
		}
	}

	// zero-padding
	int zero_count;
	if(content.size() % 8 == 0)
		zero_count = 0;
	else
		zero_count = 8 - content.size()%8;// 8bit 맞춰주기
	for(int i=0;i<zero_count;i++)
		content.insert(0, "0");

	bitset<8> zz(zero_count); // zero count 써주기
	content = zz.to_string() + content;

	string final_string = "";
	for(int i=0;i<int(content.size());i++){
		if(i%8==0){
			bitset<8> c(content.substr(i, 8));
			final_string += (char)c.to_ulong();
		}
	}
	fout << final_string;
	return 0;
}
