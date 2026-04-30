#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Your project headers
#include "parser/parser.h"
#include "ds/trie.h"
#include "utils/utils.h"
#include "analyzer/analyzer.h"
#include "ds/graph.h"

#include <string>
#include <vector>

int main() {

    // ---------------- INIT ----------------
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1000, 700, "Resume Analyzer", NULL, NULL);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // ---------------- DATA ----------------
    static char filename[100] = "";
    static char role[100] = "";

    static AnalysisResult res;
    static std::vector<std::string> suggestions;

    static float scoreValue = 0.0f;
    static bool hasResult = false;

    // ---------------- LOOP ----------------
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ---------------- UI ----------------
        ImGui::Begin("Resume Analyzer");

        ImGui::Text("Smart Resume Analyzer");
        ImGui::Separator();

        // -------- INPUT --------
        ImGui::Text("Enter Details:");
        ImGui::InputText("Resume File", filename, 100);
        ImGui::InputText("Target Role", role, 100);

        ImGui::Spacing();

        // -------- BUTTON --------
        if (ImGui::Button("Analyze Resume", ImVec2(220, 45))) {

            Candidate c = parseResume(filename);

            Trie trie;
            loadSkills(trie);

            std::vector<std::string> filteredSkills;
            for (auto s : c.skills)
                if (trie.search(s)) filteredSkills.push_back(s);

            c.skills = filteredSkills;

            res = analyze(c, role);
            suggestions = generateSuggestions(c, res);

            scoreValue = res.score / 100.0f;
            hasResult = true;
        }

        ImGui::Spacing();
        ImGui::Separator();

        // -------- OUTPUT --------
        if (hasResult) {

            ImGui::Text("Analysis Result");
            ImGui::Spacing();

            // 🔥 Score Bar
            ImGui::Text("Overall Score");
            ImGui::ProgressBar(scoreValue, ImVec2(400, 25));
            ImGui::Spacing();

            // ✅ Matched Skills (GREEN)
            ImGui::Text("Matched Skills:");
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            for (auto &s : res.matched)
                ImGui::BulletText("%s", s.c_str());
            ImGui::PopStyleColor();

            ImGui::Spacing();

            // ❌ Missing Skills (RED)
            ImGui::Text("Missing Skills:");
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));
            for (auto &s : res.missing)
                ImGui::BulletText("%s", s.c_str());
            ImGui::PopStyleColor();

            ImGui::Spacing();

            // 💡 Suggestions
            ImGui::Text("Suggestions:");
            for (auto &s : suggestions)
                ImGui::BulletText("%s", s.c_str());
        }

        ImGui::End();

        // ---------------- RENDER ----------------
        ImGui::Render();
        glViewport(0, 0, 1000, 700);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // ---------------- CLEANUP ----------------
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}