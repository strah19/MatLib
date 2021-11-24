#include "Application.h"
#include "Renderer.h"
#include "RendererCommands.h"
#include "OrthoCameraController.h"
#include "Geometry.h"
#include "FrameBuffer.h"
#include "Lexer.h"
#include "Parser.h"
#include "FunctionSolver.h"

#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

class Sandbox : public Ember::Application {
public:
	void OnCreate() {
		Ember::RendererCommand::Init();
		Ember::RendererCommand::SetViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		Ember::RendererCommand::SetPrimType(Ember::LINE);

		camera = Ember::OrthoCameraController(glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT));

		q = Ember::Quad::GetQuad();
		fb = new Ember::FrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

		imgui = new Ember::ImGuiLayer(window, event_handler, Ember::GuiFlags::GUI_DOCKER);
		PushLayer(imgui);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		float fontSize = 18.0f;
		io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", fontSize);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 1.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		window->SetResizeable(true);


		memset(in, 0, 512);
	}

	virtual ~Sandbox() {
		delete fb;
	}

	void OnUserUpdate(float delta) {
		fb->Bind();
		Ember::RendererCommand::Clear();
		Ember::RendererCommand::SetClearColor(background[0], background[1], background[2], 1.0f);
		camera.Update();
		renderer->BeginScene(&camera.GetCamera());

		q->Update(renderer);

		renderer->EndScene();
		fb->UnBind();
	}

	void UserDefEvent(Ember::Event& event) {
		Ember::EventDispatcher dispatch(&event);
		camera.OnEvent(event);
	}

	void OnGuiUpdate() {
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", NULL, window_flags);

		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		ImGui::End();

		ImGui::Begin("Lexer");

		ImGui::InputTextMultiline("Lexer Input", in, 512);
		if (ImGui::Button("Compile")) {
			lexer.Clear();
			lexer.Input(in);
			lexer.Run();
			MatLib::Parser p(&lexer);
			p.Run();
			p.Visualize();
			MatLib::FunctionSolver fs(&p);
			fs.Solve();
			p.Destroy();
		}
		ImGui::Text("Lexer Output:");
		auto& tokens = lexer.Tokens();

		for (size_t i = 0; i < tokens.size(); i++) {
			std::string s = lexer.DecodeToken(&tokens[i]);
			if (!s.empty())
				ImGui::Text("[%d] : %d : '%s'", i, tokens[i].type, s.c_str());
			else
				ImGui::Text("[%d] : %d : '%c'", i, tokens[i].type, tokens[i].type);
		}

		ImGui::End();

		ImGui::Begin("Display");
		{
			ImGui::BeginChild("GameRender");
			ImVec2 wsize = ImGui::GetWindowSize();
			uint32_t tex = fb->GetColorAttachment();
			ImGui::Image((void*)tex, wsize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}

		ImGui::End();
	}
private:
	Ember::OrthoCameraController camera;
	Ember::FrameBuffer* fb;
	Ember::Quad* q;
	float background[3] = { 0.129f, 0.309f, 0.431f };
	MatLib::Lexer lexer;
	char in[512];
};

int main(int argc, char** argv) {
	Sandbox sandbox;
	sandbox.Initialize("MatLib", WINDOW_WIDTH, WINDOW_HEIGHT);

	sandbox.Run();

	return 0;
}
