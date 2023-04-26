#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<iomanip>
using namespace std;

int i_cnt = 0;
vector<vector<string>> lr_vector;

bool check_over(string);
bool check_vector_over(vector<string>);
bool check_lrvector_over(vector<vector<string>>);
string move_dot(string);
bool isNonterminal_dot(string);
char getCharAfterDot(string);
vector<string> getRuleVectorWithStartLetter(char, vector<string>);
vector<char> getAllCharAfterDot(vector<string>);
vector<string> getMovedVector(vector<string>, char);
void printStringVector(vector<string>);
vector<string> tackleisNonterminalVector(vector<string>, vector<string>);
void writeFile(string, string);
void printIndex();
bool handleVector(vector<string>);
int getColIndex(vector<char>, char);
int getCharType(char);


int main() {
	vector<string> lr0 = { "S->E", "E->aA", "E->bB", "A->cA", "A->d", "B->cB", "B->d" };
	stringstream result_stream;
	result_stream << setw(8) << "Status" << setw(2) << "|"
		<< setw(16) << "ACTION" << setw(9) << "|"
		<< setw(9) << "GOTO" << setw(6) << "|\n";
	result_stream << setw(10) << "|"
		<< setw(5) << "a|"
		<< setw(5) << "b|"
		<< setw(5) << "c|"
		<< setw(5) << "d|"
		<< setw(5) << "#|";
	result_stream << setw(5) << "A|"
		<< setw(5) << "B|"
		<< setw(5) << "E|\n";


	for (int i = 0; i < lr0.size(); i++) {
		size_t arrow_pos = lr0[i].find("->");
		lr0[i].insert(arrow_pos + 2, ".");
	}
	bool isOver = false;
	vector<char> symbol_vector = { 'a','b','c','d','#','A','B','E' };
	//二维数组表格初始化
	vector<vector<string>> table_vector;
	for (int i = 0; i < symbol_vector.size(); i++) {
		vector<string> col_vector;
		table_vector.push_back(col_vector);
	}

	
	
	vector<string> tmp_vector = { lr0[0] };
	tmp_vector = tackleisNonterminalVector(tmp_vector,lr0);
	
	
	printIndex();
	printStringVector(tmp_vector);
	cout << "-----------------------------\n";

	lr_vector.push_back(tmp_vector);
	handleVector(lr0);

	vector<char> char_vector = getAllCharAfterDot(lr_vector[0]);
	for (char c : char_vector) {
		printIndex();
		vector<string> movedVector = getMovedVector(tmp_vector, c);
		cout <<">>>>"<< c << endl;

		movedVector = tackleisNonterminalVector(movedVector, lr0);
		
		
		printStringVector(movedVector);
		lr_vector.push_back(movedVector);
		bool vector_over = check_vector_over(movedVector);
		if (vector_over) {
			cout << "\nvector over!!!\n";
		}
		else {
			cout << "\nnot over!\n";
		}
		cout << "-----------------------------\n";
	}




	/*for (string rule : lr0) {
		cout << rule << endl;
	}
	cout << endl;*/




	
	writeFile("output.txt", result_stream.str());
	return 0;
}

//检查字符串最后一个位置是不是"."
bool check_over(string str) {
	if (str.back() == '.')
		return true;
	else
		return false;
}
//检查vector中是不是所有字符串最后一个位置都是"."
bool check_vector_over(vector<string> vec) {
	bool isOver = true;
	for (string rule : vec) {
		if (!check_over(rule)) {
			isOver = false;
			break;
		}
	}
	return isOver;
}
//检查是不是vec中的每个vector都处于结束状态
bool check_lrvector_over(vector<vector<string>> vec) {
	bool isOver = true;
	for (vector<string> v : vec) {
		if (!check_vector_over(v)) {
			isOver = false;
			break;
		}
	}
	return isOver;
}
//找到一个"."并与它后面的那个字符进行交换
string move_dot(string str) {
	size_t pos = str.find('.');
	if (pos != string::npos && pos < str.size() - 1) {//字符串有"."并且不在末尾位置
		char temp = str[pos + 1];
		str[pos + 1] = str[pos];
		str[pos] = temp;
	}
	return str;
}
//检查.后面是不是非终结符
bool isNonterminal_dot(string str) {
	size_t pos = str.find('.');
	if (pos != string::npos && pos < str.size() - 1) {
		char letter = str[pos + 1];
		if (getCharType(letter) == 0) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}
//返回"."后面那个字符
char getCharAfterDot(string str) {
	size_t pos = str.find('.');
	if (pos != string::npos && pos < str.size() - 1)
		return str[pos + 1];
	else
		return ' ';//空字符表示找不到"."后面那个字符
}
//找到以letter开头的lr0中的产生式
vector<string> getRuleVectorWithStartLetter(char letter, vector<string> lr0) {
	vector<string> result_vector;
	for (string rule : lr0) {
		if (rule[0] == letter) {
			result_vector.push_back(rule);
		}
	}
	return result_vector;
}
//得到"."后面所有不同类型的字符
vector<char> getAllCharAfterDot(vector<string> vec) {
	vector<char> result_vector;
	for (string rule : vec) {
		char after_char = getCharAfterDot(rule);
		//"."后面有字符并且该字符未在result_vector中出现过
		if (after_char != ' ' && find(result_vector.begin(), result_vector.end(), after_char) == result_vector.end())
			result_vector.push_back(after_char);
	}
	return result_vector;
}
//在vec中找到"."后面紧挨着是字符c的产生式并移动"."构成的vector
vector<string> getMovedVector(vector<string> vec, char c) {
	vector<string> result_vector;
	for (string rule : vec) {
		char afterChar = getCharAfterDot(rule);
		if (afterChar != ' ' && afterChar == c) {
			rule = move_dot(rule);
			result_vector.push_back(rule);
		}
	}
	return result_vector;
}
//打印string类型的vector
void printStringVector(vector<string> v) {
	for (string element : v) {
		cout << element << endl;
	}
}
//处理vector中含有"."后面是非终结符的产生式
vector<string> tackleisNonterminalVector(vector<string> v, vector<string> lr0) {
	for (string rule : v) {
		if (isNonterminal_dot(rule)) {//若"."后面是非终结符
			char t_letter = getCharAfterDot(rule);
			vector<string> extend_vector = getRuleVectorWithStartLetter(t_letter, lr0);
			for (string rule : extend_vector) {
				//判断产生式是否已经在vector中
				if (find(v.begin(), v.end(), rule) == v.end()) {
					v.push_back(rule);
				}
			}
		}
	}
	return v;
}
//文件写入函数
void writeFile(string outfilename, string content) {
	ofstream outfile(outfilename);
	if (!outfile) {
		cout << "The file can't be opened!" << endl;
	}
	else {
		outfile << content;
		cout << endl;
		cout << outfilename << ": " << endl;
		cout << content << endl;
	}
	outfile.close();//关闭 ofstream
}
//打印当前的状态索引
void printIndex() {
	cout << "I" << i_cnt++ << ":" << endl;
}

bool handleVector(vector<string> vec) {
	return true;
}
//根据字符找到二维表中列的索引
int getColIndex(vector<char> symbol_vector, char c) {
	int index = 0;
	for (char symbol : symbol_vector) {
		if (symbol == c) {
			return index;
		}
		index++;
	}
	return -1;
}
//判断字符是否为终结符
int getCharType(char c) {
	//0表示非终结符
	//1表示终结符
	if (c >= 'A' && c <= 'Z')
		return 0;
	else
		return 1;
}