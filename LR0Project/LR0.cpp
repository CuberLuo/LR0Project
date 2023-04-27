#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<iomanip>
using namespace std;

int i_cnt = 0;
vector<vector<string>> lr_vector;
vector<int> lr_vector_indices;
vector<vector<string>> table_vector;
vector<string> origin_lr0 = { "S->E", "E->aA", "E->bB", "A->cA", "A->d", "B->cB", "B->d" };
vector<char> symbol_vector = { 'a','b','c','d','#','E','A','B' };

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
void addRow();
void handleLR0(vector<string>, vector<string>, int);
int getColIndex(char);
int getCharType(char);
bool isEqual(vector<string>, vector<string>);
int findIndex(vector<vector<string>>, vector<string>);
void printTable();
int findIndexInLR0(vector<string>, string);
int getNonterminalNum();

int main() {
	vector<string> lr0 = origin_lr0;
	for (int i = 0; i < lr0.size(); i++) {
		size_t arrow_pos = lr0[i].find("->");
		lr0[i].insert(arrow_pos + 2, ".");
	}
	bool isOver = false;
	
	//二维数组表格初始化
	
	vector<string> tmp_vector = { lr0[0] };
	tmp_vector = tackleisNonterminalVector(tmp_vector, lr0);
	addRow();
	lr_vector.push_back(tmp_vector);
	lr_vector_indices.push_back(0);
	i_cnt++;

	handleLR0(lr0, tmp_vector,0);

	stringstream result_stream;
	result_stream <<"|" << setw(10) << "|"
		<< setw(16) << "ACTION" << setw(9) << "|"
		<< setw(9) << "GOTO" << setw(7) << "|\n";

	result_stream <<"|" << setw(7)<<"Status"<<setw(3) << "|" << setw(25) << setfill('-') << "|" << setw(16) << "|\n";
	result_stream.fill(' ');

	result_stream <<"|" << setw(10) << "|";
	for (char symbol : symbol_vector) {
		result_stream <<setw(4) << symbol << "|";
	}
	result_stream << endl;

	result_stream << "|" << setw(10) <<setfill('-') << "|" << setw(25)  << "|" << setw(16) << "|\n";
	result_stream.fill(' ');

	for (int i = 0; i < table_vector.size(); i++) {
		result_stream <<"|"<< setw(5) << i << setw(5) << "|";
		for (int j = 0; j < table_vector[i].size(); j++) {
			result_stream << setw(4) << table_vector[i][j] << "|";
		}
		result_stream << endl;
	}

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
	if (vec.size() == 0) return false;
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
		cout << outfilename << ": " << endl;
		cout << content << endl;
	}
	outfile.close();//关闭 ofstream
}
//打印当前的状态索引
void printIndex() {
	cout << "I" << i_cnt << ":" << endl;
}
void addRow() {
	vector<string> row_vector(symbol_vector.size(), " ");
	table_vector.push_back(row_vector);
}

void handleLR0(vector<string> lr0, vector<string> v,int v_index) {
	v = tackleisNonterminalVector(v, lr0);
	vector<char> char_vector = getAllCharAfterDot(v);
	vector<vector<string>> notOverVectors;
	vector<int> notOverVectorIndices;
	for (char c : char_vector) {
		vector<string> movedVector = getMovedVector(v, c);
		movedVector = tackleisNonterminalVector(movedVector, lr0);
		if (isEqual(v, movedVector)) {
			int colIndex = getColIndex(c);
			if (getCharType(c) == 0)
				table_vector[v_index][colIndex] = to_string(v_index);
			else
				table_vector[v_index][colIndex] = "S" + to_string(v_index);
			continue;
		}
			
		int mIndex = findIndex(lr_vector, movedVector);
		if (mIndex != -1) {
			int colIndex = getColIndex(c);
			if (getCharType(c) == 0)
				table_vector[v_index][colIndex] = to_string(mIndex);
			else
				table_vector[v_index][colIndex] = "S" + to_string(mIndex);
			continue;
		}
		addRow();

		int colIndex = getColIndex(c);
		if (getCharType(c) == 0)
			table_vector[v_index][colIndex] = to_string(i_cnt);
		else
			table_vector[v_index][colIndex] = "S" + to_string(i_cnt);
		
		lr_vector.push_back(movedVector);
		lr_vector_indices.push_back(v_index);
		bool vector_over = check_vector_over(movedVector);
		if (vector_over) {
			movedVector[0].pop_back();
			if (movedVector[0] == origin_lr0[0]) {
				int overSymbolIndex = getColIndex('#');
				table_vector[i_cnt][overSymbolIndex] = "acc";
			}
			else {
				int pos_index = findIndexInLR0(origin_lr0, movedVector[0]);
				for (int i = 0; i < symbol_vector.size()-getNonterminalNum(); i++) {
					table_vector[i_cnt][i] = "r" + to_string(pos_index);
				}
			}
			
		}
		else{
			notOverVectors.push_back(movedVector);
			notOverVectorIndices.push_back(i_cnt);
		}
		i_cnt++;
		
	}
	//递归结束条件:notOverVectors.size()为0
	for (size_t i = 0; i < notOverVectors.size(); i++) {
		handleLR0(lr0, notOverVectors[i], notOverVectorIndices[i]);
	}
}
//根据字符找到二维表中列的索引
int getColIndex(char c) {
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
//判断两个vector<string>是否完全相同
bool isEqual(vector<string> vec1, vector<string> vec2) {
	if (vec1.size() != vec2.size()) {  // 先判断大小是否相等
		return false;
	}
	for (int i = 0; i < vec1.size(); i++) {  // 逐个比较每个元素是否相等
		if (vec1[i] != vec2[i]) {
			return false;
		}
	}
	return true;
}
//判断vector<string>是否在vector<vector<string>>中,返回下标
int findIndex(vector<vector<string>> vec, vector<string> target) {
	for (int i = 0; i < vec.size(); ++i) {
		if (isEqual(vec[i], target)) {
			return i;
		}
	}
	return -1;
}
//打印表格
void printTable() {
	for (int i = 0; i < table_vector.size(); i++) {
		for (int j = 0; j < table_vector[i].size(); j++) {
			cout << table_vector[i][j] << "\t";
		}
		cout << endl;
	}
}
//判断string是否在vector<string>中,返回下标
int findIndexInLR0(vector<string> lr0, string s) {
	for (int i = 0; i < lr0.size(); i++) {
		if (lr0[i] == s) {
			return i;
		}
	}
	return -1;
}
//得到非终结符的个数
int getNonterminalNum() {
	int cnt = 0;
	for (char c : symbol_vector) {
		if (getCharType(c) == 0) {
			cnt++;
		}
	}
	return cnt;
}