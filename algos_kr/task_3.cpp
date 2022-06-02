#include <stack>
#include <string>
#include <iostream>
using namespace std;

void check_parentheses(std::string st) {
    bool ans = true;

	std::stack<char> s;

    for (int i = 0; i < st.size(); i++)
        if (st[i] == '(' || st[i] == '[' || st[i] == '{')
            s.push(st[i]);
        else
        {
            if (!s.empty())
            {
                char temp = s.top();

                s.pop();

                if (st[i] == ')' && temp != '(')
                {
                    ans = false;
                    break;
                }
                if (st[i] == ']' && temp != '[')
                {
                    ans = false;
                    break;
                }
                if (st[i] == '}' && temp != '{')
                {
                    ans = false;
                    break;
                }
            }
            else
            {
                ans = false;
                break;
            }
        }

    if (!s.empty())
        ans = false;
    if (ans)
        std::cout << "balanced" << std::endl;
    else
        std::cout << "Not balanced" << std::endl;
}

int main()
{
	std::string brackets;
	std::cout << "Enter brackets: ";

	std::cin >> brackets;

    check_parentheses(brackets);
}

