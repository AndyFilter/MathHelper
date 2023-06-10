#include <math.h>
#include <vector>
#include <stack>
#include <queue>

#define sign(a) (a < 0 ? -1 : a == 0 ? 0 : 1)

enum class Enum_FunctionType
{
	sine,
	cosine,
	tan,
	cot,
	log,
	ln,
	pow,
	sqrt,
	root,
	custom,
};
static const char* getFuncNameCpp(Enum_FunctionType func)
{
	switch (func)
	{
	case Enum_FunctionType::sine:
		return "sin";
		break;
	case Enum_FunctionType::cosine:
		return "cos";
		break;
	case Enum_FunctionType::tan:
		return "tan";
		break;
	case Enum_FunctionType::cot:
		return "tan";
		break;
	case Enum_FunctionType::log:
		return "log10";
		break;
	case Enum_FunctionType::ln:
		return "log";
		break;
	case Enum_FunctionType::pow:
		return "pow";
		break;
	case Enum_FunctionType::sqrt:
		return "sqrt";
		break;
	case Enum_FunctionType::root:
		return "pow";
		break;
	case Enum_FunctionType::custom:
		return "";
		break;
	default:
		return "";
		break;
	}
}
static Enum_FunctionType getFuncFromString(const char* str)
{
	for (int i = 0; i < (int)Enum_FunctionType::custom; i++) {
		if (strcmp(getFuncNameCpp((Enum_FunctionType)i), str) == 0)
			return (Enum_FunctionType)i;
	}

	return (Enum_FunctionType)-1;
}

enum class Enum_Operator
{
	none,
	add,
	subtract,
	multiply,
	divide,
	power,
	lBracket,
	rBracket,
};
static const char* getOperatorName(Enum_Operator op) {
	switch (op)
	{
	case Enum_Operator::none:
		return "";
		break;
	case Enum_Operator::add:
		return "+";
		break;
	case Enum_Operator::subtract:
		return "-";
		break;
	case Enum_Operator::multiply:
		return "*";
		break;
	case Enum_Operator::divide:
		return "/";
		break;
	case Enum_Operator::power:
		return "^";
		break;
	case Enum_Operator::lBracket:
		return "(";
		break;
	case Enum_Operator::rBracket:
		return ")";
		break;
	default:
		break;
	}
}
static const char getOperatorNameChar(Enum_Operator op) {
	switch (op)
	{
	case Enum_Operator::none:
		return '\0';
		break;
	case Enum_Operator::add:
		return '+';
		break;
	case Enum_Operator::subtract:
		return '-';
		break;
	case Enum_Operator::multiply:
		return '*';
		break;
	case Enum_Operator::divide:
		return '/';
		break;
	case Enum_Operator::power:
		return '^';
		break;
	case Enum_Operator::lBracket:
		return '(';
		break;
	case Enum_Operator::rBracket:
		return ')';
		break;
	default:
		break;
	}
}
static Enum_Operator getOperatorFromChar(char c)
{
	for (int i = 0; i <= 7; i++) {
		if (getOperatorNameChar((Enum_Operator)i) == c)
			return (Enum_Operator)i;
	}

	return Enum_Operator::none;
}
static uint8_t getOperatorPriority(Enum_Operator op)
{
	switch (op)
	{
	case Enum_Operator::none:
		return 0;
		break;
	case Enum_Operator::add:
		return 1;
		break;
	case Enum_Operator::subtract:
		return 1;
		break;
	case Enum_Operator::multiply:
		return 2;
		break;
	case Enum_Operator::divide:
		return 2;
		break;
	case Enum_Operator::power:
		return 3;
		break;
	case Enum_Operator::lBracket:
		return 0;
		break;
	case Enum_Operator::rBracket:
		return 1;
		break;
	default:
		break;
	}
}


#pragma region ReversePolishNotation

enum Enum_RPN_ElementType
{
	RPN_ElementType_Value,
	RPN_ElementType_Variable,
	RPN_ElementType_Operator,
	RPN_ElementType_Function,
};

struct RPN_Element
{
	Enum_RPN_ElementType type;

	union RPN_Element_data // I found a use for union 0_0
	{
		char value_var;
		float value;
		Enum_FunctionType func_type;
		Enum_Operator op_type;
		RPN_Element_data(const double value) : value(value) { };
		RPN_Element_data(const Enum_FunctionType func_type) : func_type(func_type) { };
		RPN_Element_data(const Enum_Operator op_type) : op_type(op_type) { };
		RPN_Element_data(const char value_var) : value_var(value_var) { };
	} data;

	RPN_Element(const Enum_RPN_ElementType& type, const RPN_Element_data& data)
		: type(type), data(data)
	{ }
};

std::queue<RPN_Element> ParseEquationToRPN(char szEq[]);
float EvaluateRPN(std::queue<RPN_Element> q, float x);

#pragma endregion