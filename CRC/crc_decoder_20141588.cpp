#include <iostream>
#include <cstdlib> // atoi, atol, malloc
#include <fstream>
#include <cstring>
#include <string>
#include <bitset>
#include <vector>
using namespace std;

int check_modulo(string s, int dataword_size, string generator, int generator_size){
	// s 가 generator로 나누어 떨어지면
	string s1 = s.substr(0, generator_size);
	string s2 = generator;
	string quo = "", remain = "";

	for(int i=0;i<dataword_size;i++){
		if(s1[0]=='1'){
			quo += "1";

			string tmp="";
			for(int j=0;j<generator_size;j++){
				if(s1[j]==s2[j])
					tmp += "0";
				else
					tmp += "1";
			}
			if(i==dataword_size-1)
				remain = tmp;

			//left push
			tmp += s[i+generator.size()];
			tmp.erase(0, 1);
			s1 = tmp;
		}
		else{
			quo += "0";
			if(i==dataword_size-1)
				remain = s1;
			s1 += s[i+generator.size()];
			s1.erase(0, 1);
		}
	}
	//cout << "quo: " << quo << "  remainder: " << remain << endl;
	if(atoi(remain.c_str())==0){
		//cout << "no error" << endl;
		return 1;
	}
	else{
		//cout << "yes error" << endl;
		return -1;
	}
	//return atoi(s.substr(0, dataword_size).c_str());
}
int main(int argc, char* argv[]){
	if(argc != 6){
		cout << "usage: ./crc_decoder input_file output_file result_file generator dataword_size";
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
	ofstream fres(argv[3]);
	if(!fres){
		cout << "result file open error." << endl;
		exit(1);
	}
	string generator = argv[4];
	int dataword_size = atoi(argv[5]);
	if(dataword_size!=4 && dataword_size!=8){
		cout << "dataword size must be 4 or 8." << endl;
		exit(1);
	}
	vector<unsigned char> buf(istreambuf_iterator<char>(fin), (istreambuf_iterator<char>()) );
	//for(int i=0;i<buf.size();i++)
	//	cout << buf[i] << " ";

	string content = "";
	int padding;
	for(int i=0;i<(int)buf.size();i++){
		//cout << buf[i] << endl;
		bitset <8> bb( buf[i]);
		if(i==0){
			padding = int(bb.to_ulong());
			continue;
		}
		content += bb.to_string();
	}

	content.erase(0, padding); // remove padding
	int codeword_size = dataword_size + generator.size() - 1;
	int generator_size = generator.size();

	int codeword_cnt = 0;
	int error_cnt = 0;
	string message = "";
	string ascii = "";
	for(int i=0;i<int(content.size());i++){
		if(i%codeword_size==0){
			
			codeword_cnt += 1;
			string tmp = content.substr(i, codeword_size);
			
			//get original msg
			ascii += content.substr(i, dataword_size);
			if(ascii.size()==8){
				bitset<8> bb(ascii);
				message += char(bb.to_ulong());
				ascii.clear();
			}
			//check error
			if(check_modulo(tmp, dataword_size, generator, generator_size)==-1){
				error_cnt += 1;
			}
		}
	}
	//cout << content << endl;
	//cout << "msg : " << endl << message << endl;
	fout << message;
	fres << codeword_cnt << " " << error_cnt;
	return 0;
}
