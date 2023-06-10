#include <cstring>
#include <algorithm>
#include <string>

#include "Math_Helper.h"

// Works only for single argument scanfs. I dislike Microsoft...
#define str_scanf(buffer, format, ...) sscanf_s(buffer, format, __VA_ARGS__, (unsigned)_countof(__VA_ARGS__))

void remove_spaces(char* s) {
	char* p = s;
	while (*p) // Check if not null
	{
		*p = *s; // Set current char to the actual char
		if (*p != ' ') // increment pointer if not space
			p++; 
		s++; // always increment actual string
	}
}

int count_chars(char* s, char chr)
{
	int count = 0;
	do
		if (*s == chr)
			count++;
	while (*s++);

	return count;
}

// Returns a pointer pointing on the next char, at [0]. nullptr if not found
char* get_next_operator(char* szStr)
{
	char* ptr = szStr;
	while (*ptr) {
		if (strchr("+-*/^", *ptr))
			return ptr;
		ptr++;
	}
	return nullptr;
}

bool is_operator(char c) {
	return c == 0 ? 0 : strchr("+-*/^", c);
}

bool is_brackets(char c) {
	return strchr("()[]", c);
}

char* get_next_brackets(char* szStr, bool ignoreFunctions = true)
{
	char* ptr = szStr;
	while (*ptr) {
		if (strchr("()[]", *ptr)) {
			if(ptr == szStr || (!ignoreFunctions || strchr("+-*/^", * (ptr - 1)))) // Check if is first or the character before brackets is an operator (so we dont take function brackets)
				return ptr;
		}
		ptr++;
	}
	return nullptr;
}

char count_brackets_state(char* szStr)
{
	char* ptr = szStr;
	char count = 0;
	while (*ptr) {
		if (*ptr == '(')
			count--;

		else if (*ptr == ')')
			count++;

		ptr++;
	}

	return count;
}

bool is_number(char* szStr)
{
	char* str_ptr = szStr;
	do {
		auto x = strchr("0123456789.", (*str_ptr));
		if(!strchr("0123456789.", (*str_ptr)))
			return false;

	} while (*(++str_ptr));

	return true;
}

// (Allocates memory) Pointer should be taken care of by the caller
char* clean_equation(const char* szText)
{
	size_t og_len = std::strlen(szText) + 1;
	char* szEqCopy = new char[og_len];
	std::copy(szText, szText + og_len, szEqCopy);
	remove_spaces(szEqCopy);

	return szEqCopy;
}

char* clear_leading_ops_and_brackets(char* szText)
{
	char* ptr = szText;
	while (*ptr)
	{
		if (!is_brackets(*ptr) && !is_operator(*ptr))
			return ptr;
		
		ptr++;
	}

	return szText;
}

bool is_function(char* szText, bool skip_ops_and_open_brackets = false)
{
	char* next_op = get_next_operator(szText);
	char* power_ptr = strchr(szText, '^');
	// First check for "^" (power notation)
	if (power_ptr && power_ptr < next_op)
		return true;
	
	// Now check for real functions
	char* ptr = clear_leading_ops_and_brackets(szText);
	char* next_brackets = strchr(ptr, '(');
	long size = next_brackets - ptr;
	char* _buf = new char[size+1];
	memset(_buf, 0, size + 1);
	std::copy(ptr, next_brackets, _buf);

	if ((int)getFuncFromString((const char*)_buf) >= 0)
		return true;

	return false;
}

//std::vector<char*> get_all_function_params(char* (&szText))
//{
//	std::vector<char*> out_arr;
//	char* text_ptr = szText;
//	char* comma_ptr = strchr(text_ptr, ',');
//	char* brackets_ptr = strchr(text_ptr, ')');
//	char* furthest_ptr = strchr(text_ptr, '(') + 1;
//	short brackets_state = 0;
//	while (comma_ptr || brackets_ptr || furthest_ptr)
//	{
//		char arg[24] = { 0 };
//		out_arr.push_back(furthest_ptr);
//		if (comma_ptr != nullptr && comma_ptr < brackets_ptr)
//			auto x = 0;
//			//str_scanf(furthest_ptr, ",%23s", arg);
//		else {
//			//str_scanf(furthest_ptr, "%23[^)])", arg);
//			brackets_state--;
//			break;
//		}
//
//		text_ptr = furthest_ptr;
//		comma_ptr = strchr(text_ptr, ',');
//		brackets_ptr = strchr(text_ptr, ')');
//		furthest_ptr = std::min(comma_ptr, brackets_ptr);
//	}
//
//	return out_arr;
//}

char* get_brakets_equalibrium_point(char* szText)
{
	char brackets_state = 0;
	char* ptr = strchr(szText, '(');
	while (*ptr)
	{
		brackets_state += (*ptr == '(' ? -1 : *ptr == ')');
		if (!brackets_state)
			break;
		ptr++;
	}

	return ptr;
}

char* get_function_end(char* szText)
{
	char* ptr = clear_leading_ops_and_brackets(szText);
	if (!is_function(ptr))
		return nullptr;
	else
		return get_brakets_equalibrium_point(ptr);
}

// Returns then end of the number (eg. 263, will return a pointer to the character after the last digit). nullptr if arg doesnt start with a number
char* get_number_end(char* szText)
{
	if (!isdigit(*szText))
		return nullptr;

	register char* ptr = szText;
	while (*ptr && (isdigit(*ptr) || *ptr == '.'))
	{
		ptr++;
	}

	return ptr;
}

// For example changes "2x" to 2*x
char* formatEquation(char* szText)
{
	char* ptr = szText;
	std::string out_string;

	while (char c = *ptr)
	{
		char next_c = *(ptr + 1);
		if ((c == 'x' || isdigit(c)) &&
			next_c != 0 && ((next_c == '(' || next_c == 'x' || (!is_number(ptr) && !is_operator(next_c))) &&
			(next_c != ')' && !isdigit(next_c) && next_c != '.'))
			)
		{
			out_string += c;
			out_string += '*';
		}
		else
			out_string += c;

		ptr++;
	}

	char* szOut = new char[out_string.size()+1];
	memset(szOut, 0, out_string.size() + 1);
	memcpy(szOut, out_string.c_str(), out_string.size()+1);
	return szOut;
}

std::queue<RPN_Element> ParseEquationToRPN(char szEq[])
{
	char* _szEqClean = clean_equation(szEq);
	char* szEqClean = formatEquation(_szEqClean);
	delete[] _szEqClean;

	std::queue<RPN_Element> out_q; // output queue
	std::stack<RPN_Element> op_stack; // operators stack

	char* pChar = szEqClean;
	while (*pChar)
	{
		char c = *pChar;
		Enum_Operator op = getOperatorFromChar(c);

		if (isdigit(c))
		{
			double value = atof(pChar);
			pChar = get_number_end(pChar)-1;
			out_q.push(RPN_Element(RPN_ElementType_Value, value));
		}
		else if (c == 'x')
		{
			out_q.push(RPN_Element(RPN_ElementType_Variable, 'x'));
		}
		else if (op != Enum_Operator::none)
		{
			if (op == Enum_Operator::lBracket)
			{
				op_stack.push(RPN_Element(Enum_RPN_ElementType::RPN_ElementType_Operator, op));
			}
			else if (op == Enum_Operator::rBracket)
			{
				auto top_op = op_stack.size() > 0 ? op_stack.top().data.op_type : Enum_Operator::none;
				bool isFunc = op_stack.top().type == RPN_ElementType_Function;
				while (top_op != Enum_Operator::lBracket)
				{
					out_q.push(op_stack.top());
					op_stack.pop();
					//if (isFunc)
					//	break;
					top_op = op_stack.size() > 0 ? op_stack.top().data.op_type : Enum_Operator::none;
				}
				if(top_op == Enum_Operator::lBracket)
					op_stack.pop();
				if (op_stack.size() > 0 && op_stack.top().type == RPN_ElementType_Function) {
					out_q.push(op_stack.top());
					op_stack.pop();
				}
			}
			else // operator
			{
				auto top_op = op_stack.size() > 0 ? op_stack.top().data.op_type : Enum_Operator::none;

				while (
					top_op != Enum_Operator::none &&
					((op == Enum_Operator::power && getOperatorPriority(op) < getOperatorPriority(top_op)) ||
					(op != Enum_Operator::power && getOperatorPriority(op) <= getOperatorPriority(top_op)))
					)
				{
					out_q.push(RPN_Element(Enum_RPN_ElementType::RPN_ElementType_Operator, top_op));
					op_stack.pop();
					top_op = op_stack.size() > 0 ? op_stack.top().data.op_type : Enum_Operator::none;
				}

				op_stack.push(RPN_Element(Enum_RPN_ElementType::RPN_ElementType_Operator, op));
			}
		}
		else if (c == ',')
		{
			auto top_op = op_stack.size() > 0 ? op_stack.top().data.op_type : Enum_Operator::none;
			while (top_op != Enum_Operator::lBracket)
			{
				out_q.push(op_stack.top());
				op_stack.pop();
				top_op = op_stack.size() > 0 ? op_stack.top().data.op_type : Enum_Operator::none;
			}
		}
		else // Possibly a function, but could also be a missformat
		{
			char szFunc[24];
			char* func_end = get_next_brackets(pChar, false);
			strncpy_s(szFunc, pChar, func_end - pChar);
			auto func_type = getFuncFromString(szFunc);
			if ((int)func_type != -1)
			{
				op_stack.push(RPN_Element(RPN_ElementType_Function, func_type));
				op_stack.push(RPN_Element(RPN_ElementType_Operator, Enum_Operator::lBracket));
			}
			pChar = func_end;
		}

#ifdef DEBUG

		printf("char: %c\n", c);
		printf("Output Queue:\n");
		auto out_q_c = out_q;
		while(out_q_c.size())
		{
			if (out_q_c.front().type == RPN_ElementType_Value)
				printf("%.1f, ", out_q_c.front().data.value);
			else
				printf("%c, ", getOperatorNameChar(out_q_c.front().data.op_type));

			out_q_c.pop();
		}


		printf("\n\nOp Stack:\n");
		auto op_stack_c = op_stack;
		while (op_stack_c.size())
		{
			printf("%c, ", getOperatorNameChar(op_stack_c.top().data.op_type));
			op_stack_c.pop();
		}

		printf("\n\n----------------\n");

#endif // DEBUG

		pChar++;
	}

	while (op_stack.size())
	{
		out_q.push(op_stack.top());
		op_stack.pop();
	}

	delete szEqClean;

	return out_q;
}

float EvaluateRPN(std::queue<RPN_Element> q, float x)
{
	std::stack<float> buffer_stack;

	while (q.size())
	{
		switch (q.front().type)
		{
		case RPN_ElementType_Value:
			buffer_stack.push(q.front().data.value);
			break;

		case RPN_ElementType_Variable:
			if (q.front().data.value_var == 'x')
				buffer_stack.push(x);
			break;

		case RPN_ElementType_Operator: 
		{
			float a = buffer_stack.top(); buffer_stack.pop();
			float b = buffer_stack.top(); buffer_stack.pop();
			float out_val = 0;
			switch (q.front().data.op_type)
			{
			case Enum_Operator::none:
				break;
			case Enum_Operator::add:
				out_val = b + a;
				break;
			case Enum_Operator::subtract:
				out_val = b - a;
				break;
			case Enum_Operator::multiply:
				out_val = b * a;
				break;
			case Enum_Operator::divide:
				out_val = b / a;
				break;
			case Enum_Operator::power:
				out_val = pow(b, a);
				break;
			default:
				break;
			}
			buffer_stack.push(out_val);
		};
			break;

		case RPN_ElementType_Function:
		{
			float a = 0;
			float out_val = 0;
			switch (q.front().data.func_type)
			{
			case Enum_FunctionType::sine:
				out_val = sinf(buffer_stack.top());
				buffer_stack.pop();
				break;
			case Enum_FunctionType::cosine:
				out_val = cosf(buffer_stack.top());
				buffer_stack.pop();
				break;
			case Enum_FunctionType::tan:
				out_val = tan(buffer_stack.top());
				buffer_stack.pop();
				break;
			case Enum_FunctionType::cot:
				out_val = 1.f/tan(buffer_stack.top());
				buffer_stack.pop();
				break;
			case Enum_FunctionType::log:
				out_val = log10(buffer_stack.top());
				buffer_stack.pop();
				break;
			case Enum_FunctionType::ln:
				out_val = log(buffer_stack.top());
				buffer_stack.pop();
				break;
			case Enum_FunctionType::pow:
				a = buffer_stack.top();
				buffer_stack.pop();
				out_val = pow(buffer_stack.top(), a);
				buffer_stack.pop();
				break;
			case Enum_FunctionType::sqrt:
				out_val = sqrt(buffer_stack.top());
				buffer_stack.pop();
				break;
			case Enum_FunctionType::root:
				a = buffer_stack.top();
				buffer_stack.pop();
				out_val = pow(buffer_stack.top(), 1.f/a);
				buffer_stack.pop();
				break;
			case Enum_FunctionType::custom:
				break;
			default:
				break;
			}
			buffer_stack.push(out_val);
		};
			break;

		default:
			break;
		}

		q.pop();
	}

	return buffer_stack.top();
}