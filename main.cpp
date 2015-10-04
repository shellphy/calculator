#include <iostream>
#include <string>
#include <list>
#include <cstring>
#include <stack>

using namespace std;

string inputString;
list<string> buf;
list<string>::iterator buf_iter;
stack<int> stk;

struct TreeNode
{
	TreeNode *leftChild;
	TreeNode *RightChild;
	string val;
};

void initBuf();		// 初始化buf. 把表达式中的每个元素分开, 保存到list中 比如1+2*3就可以分成5个元素1, +, 2, *, 3
string getToken();	// 从buf中获取下一个元素
void ungetToken();	// 在buf中回退一个元素
bool isNum(string s);	// 判断字符串s是否为数字
bool isOperator(const string &s);	// 判断字符串s是不是操作符+ - * / 
TreeNode *parse_exp();		// 解析exp
TreeNode *parse_term();		// 解析term
TreeNode *parse_factor();	// 解析factor
void post_traverseTree(TreeNode *t);	// 遍历表达式树

int main()
{
	getline(cin, inputString);
	initBuf();
	TreeNode *root = parse_exp();
	post_traverseTree(root);
	cout << stk.top();

	return 0;
}

void post_traverseTree(TreeNode *t)
{
	if (t != nullptr)
	{
		post_traverseTree(t->leftChild);
		post_traverseTree(t->RightChild);
		if (isNum(t->val))
			stk.push(stoi(t->val));
		if (isOperator(t->val))
		{
			int val1 = stk.top(); stk.pop();
			int val2 = stk.top(); stk.pop();
			if (t->val == "+")
				stk.push(val2 + val1);
			else if (t->val == "-")
				stk.push(val2 - val1);
			else if (t->val == "*")
				stk.push(val2 * val1);
			else if (t->val == "/")
				stk.push(val2 / val1);
		}
	}
}

void initBuf()
{
	string temp;
	for (auto iter = inputString.cbegin(); iter != inputString.cend(); ++iter)
	{
		if (isdigit(*iter))
			temp.push_back(*iter);
		else if (*iter == '+' || *iter == '-' || *iter == '*' || *iter == '/' || *iter == '(' || *iter == ')')
		{
			if (!temp.empty())
				buf.push_back(temp);
			temp.clear();
			temp.push_back(*iter);
			buf.push_back(temp);
			temp.clear();
		}
		else if (isspace(*iter))
			continue;
		/*
		else
			error();
		*/
	}
	buf.push_back(temp);

	/* 把负数, 也就是表达式为(-1)*2这样的形式转换为(0-1)*2的形式 */
	for (auto iter = buf.begin(); iter != buf.end(); ++iter)
	{
		if (*iter == "(")
		{
			++iter;
			if (*iter == "-")
				buf.insert(iter, "0");
			else
				--iter;
		}
	}
	/* 如果在表达式的开头有负号, 比如 -1*2  那么就修改为(0-1)*2 */
	if (inputString[0] == '-')
	{
		auto iter = buf.begin(); ++iter; ++iter;
		buf.insert(iter, ")");
		buf.insert(buf.begin(), "0");
		buf.insert(buf.begin(), "(");
	}
	buf_iter = buf.begin();
}

string getToken()
{
	if (buf_iter != buf.end())
		return *buf_iter++;
	else
		return "end";
}

void ungetToken()
{
	if (buf_iter != buf.begin())
		--buf_iter;
	/*
	else
		error();
	*/
}

/*
 * 产生式:
 * exp -> exp + term
 *     -> exp - term
 *     -> term
 *
**/
TreeNode *parse_exp()
{
	TreeNode *t = parse_term();
	string temp = getToken();
	while (temp[0] == '+' || temp[0] == '-')
	{
		TreeNode *p = new TreeNode{t, parse_term(), temp};
		t = p;
		temp = getToken();
	}
	ungetToken();
	return t;
}

/*
 * 产生式:
 * term -> term * factor
 *      -> term / factor
 *      -> factor
**/

TreeNode *parse_term()
{
	TreeNode *t = parse_factor();
	string temp = getToken();
	while (temp[0] == '*' || temp[0] == '/')
	{
		TreeNode *p = new TreeNode{t, parse_factor(), temp};
		t = p;
		temp = getToken();
	}
	ungetToken();
	return t;
}

/*
 * 产生式:
 * factor -> NUM
 *        -> ( exp )
**/
TreeNode *parse_factor()
{
	TreeNode *t = nullptr;
	string temp = getToken();
	if (isNum(temp))
		t = new TreeNode{nullptr, nullptr, temp};
	else if (temp[0] == '(')
	{
		t = parse_exp();
		temp = getToken();
		if (temp[0] != ')')
		{
			cerr << "Syntax Error!";
			exit(-1);
		}
	}
	return t;
}

bool isNum(string s)    // 只要第一个字符为数字, 那么这个字符串就是数字
{
	return isdigit(s[0]);
}

bool isOperator(const string &s)
{
	return (s[0] == '+' || s[0] == '-' || s[0] == '*' || s[0] == '/');
}
