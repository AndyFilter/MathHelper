#include "GUI/gui.h"
#include "../External/ImGui/implot.h"
#include "../External/ImGui/imgui.h"
#include "../External/ImGui/imgui_internal.h"

#include "Math_Helper.h"


// --------------------- TODO ---------------------
/*

- Add "To_Cpp_Code" function.
- Wrap current queue from ParseEquationToRPN in a class
- Make graph adapt to changes like zooming, panning, resize
- Add cool features

*/


#define GRAPH_RESOLUTION (1 << 12)

int OnGui()
{
#ifdef _DEBUG
	ImPlot::ShowDemoWindow();
#endif // DEBUG

	static char equation[128] = "4(sin(3.23x) + 6) * 5 / (x - 11) * (cos(3.14) + 55^(1/3))";
	//static char equationCpp[128] = "Test eq\0";
	static float eqData[GRAPH_RESOLUTION] = { 0 };
	static float graph_x_axis[GRAPH_RESOLUTION] = { 0 };
	static std::queue<RPN_Element> userRPN_equation = {};

	ImGui::SetNextItemWidth(-1);
	ImGui::InputTextWithHint("##EquationInput", "Input your equation here", equation, 128);
	if (((ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) && ImGui::IsItemFocused()) || ImGui::Button("Parse", {-1, 0}))
	{
		userRPN_equation = ParseEquationToRPN(equation);
#ifdef _DEBUG
		auto out = userRPN_equation; // make a copy
		int i = 0;
		while(out.size())
		{
			if(out.front().type == RPN_ElementType_Value)
				printf("elem[%i] = %f\n", i, out.front().data.value);
			else if(out.front().type == RPN_ElementType_Variable)
				printf("elem[%i] = %c\n", i, out.front().data.value_var);
			else if(out.front().type == RPN_ElementType_Function)
				printf("elem[%i] = %s\n", i, getFuncNameCpp(out.front().data.func_type));
			else
				printf("elem[%i] = %c\n", i, getOperatorNameChar(out.front().data.op_type));
			out.pop();
			i++;
		}
#endif // DEBUG

		//out = userRPN_equation;
		for (int i = 0; i < GRAPH_RESOLUTION; i++)
		{
			graph_x_axis[i] = (i - GRAPH_RESOLUTION/2) * 0.1f;
			eqData[i] = EvaluateRPN(userRPN_equation, graph_x_axis[i]);
		}
	}

	//if (ImGui::Button("To C++"))
	//{
	//	//eq2 = eq.FromMathEquation("4^(sin(3.23x) + 6) * 5 / (x + t) * (- cos(M_PI) - 5^(1/3))");
	//	//eq2 = eq.FromMathEquation("4*(sin(3.23x) + 6) * 5 / (x + 55) * (- cos(3.14) - 5^(1/3))");
	//	//eq2 = eq.FromMathEquation("(- cos(3.14) - (5^(1/3)))");
	//}

	//ImGui::Text(equationCpp);

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	if ((userRPN_equation.size() > 0) && ImPlot::BeginPlot("Functions", {-1, -1})) {
		ImPlot::SetupAxes("x", "y");
		ImPlot::SetupAxesLimits(2, 100, 2, 100);
		ImPlot::PlotLine("F(x)", graph_x_axis, eqData, GRAPH_RESOLUTION, 0);
		ImPlot::EndPlot();
	}

	return 0;
}

int main()
{
	GUI::Setup(OnGui);

	while (true)
	{
		if (GUI::DrawGui())
			break;
	}

	return 0;
}