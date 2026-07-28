#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "parser/parser.h"
#include "ds/trie.h"
#include "ds/edit_distance.h"
#include "utils/utils.h"
#include "analyzer/analyzer.h"
#include "ds/graph.h"
#include "ai/ai_suggestions.h"

#include <string>
#include <vector>

using namespace std;

int main() {

    // ---------------- INIT ----------------
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1000, 750,
        "ResumeIQ - Smart Resume Analyzer", NULL, NULL);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.2f;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // ---------------- DATA ----------------
    static char filename[256] = "resume.txt";
    static char role[256] = "";

    static Candidate candidate;
    static AnalysisResult res;
    static string aiSuggestions = "";
    static vector<string> corrected;

    static float scoreValue = 0.0f;
    static bool hasResult = false;
    static bool analyzing = false;

    // ---------------- LOOP ----------------
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Full screen window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1000, 750));
        ImGui::Begin("ResumeIQ", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse);

        // -------- HEADER --------
        ImGui::SetCursorPosX(350);
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f),
            "ResumeIQ - Smart Resume Analyzer");
        ImGui::Separator();
        ImGui::Spacing();

        // -------- INPUT SECTION --------
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.5f, 1.0f),
            ">> Input Details");
        ImGui::Spacing();

        ImGui::Text("Resume File :");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##file", filename, 256);

        ImGui::Text("Target Role :");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##role", role, 256);

        ImGui::Spacing();

        // -------- ANALYZE BUTTON --------
        ImGui::SetCursorPosX(350);
        if (ImGui::Button("  Analyze Resume  ", ImVec2(250, 40))) {

            // Reset
            corrected.clear();
            aiSuggestions = "";
            hasResult = false;

            // Parse resume
            candidate = parseResume(string(filename));

            // Trie + Edit Distance filtering
            Trie trie;
            loadSkills(trie);
            vector<string> allKnown = getRoleSkills(string(role));
            vector<string> filtered;

            for (auto s : candidate.skills) {
                string norm = toLower(s);
                if (trie.search(norm)) {
                    filtered.push_back(norm);
                } else {
                    string closest = findClosestSkill(norm, allKnown);
                    if (closest != "") {
                        filtered.push_back(closest);
                        corrected.push_back(s + " -> " + closest);
                    }
                }
            }
            candidate.skills = filtered;

            // Analyze
            res = analyze(candidate, string(role));
            scoreValue = res.score / 100.0f;

            // AI Suggestions
            aiSuggestions = getAISuggestions(
                string(role),
                res.matched,
                res.missing,
                candidate.experience
            );

            // Save
            vector<string> suggestions = generateSuggestions(candidate, res);
            saveReport(candidate, res, suggestions);
            saveHistory(candidate, res.score);

            hasResult = true;
        }

        ImGui::Spacing();
        ImGui::Separator();

        // -------- OUTPUT SECTION --------
        if (hasResult) {

            ImGui::Spacing();

            // Candidate Info
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.5f, 1.0f),
                ">> Candidate");
            ImGui::Text("   Name  : %s", candidate.name.c_str());
            ImGui::Text("   Email : %s", candidate.email.c_str());
            ImGui::Text("   Role  : %s", role);
            ImGui::Spacing();

            // Auto-corrections
            if (!corrected.empty()) {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f),
                    ">> Auto-Corrections");
                for (auto& c : corrected) {
                    ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f),
                        "   %s", c.c_str());
                }
                ImGui::Spacing();
            }

            // Score
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.5f, 1.0f),
                ">> Match Score");
            ImGui::Text("   %d%%", res.score);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(400);
            ImGui::ProgressBar(scoreValue, ImVec2(400, 20));
            ImGui::Spacing();

            // Two columns — matched and missing
            ImGui::Columns(2, "skills");

            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.5f, 1.0f),
                ">> Matched Skills");
            ImGui::PushStyleColor(ImGuiCol_Text,
                IM_COL32(80, 255, 80, 255));
            for (auto& s : res.matched)
                ImGui::BulletText("%s", s.c_str());
            ImGui::PopStyleColor();

            ImGui::NextColumn();

            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.5f, 1.0f),
                ">> Missing Skills");
            ImGui::PushStyleColor(ImGuiCol_Text,
                IM_COL32(255, 80, 80, 255));
            for (auto& s : res.missing)
                ImGui::BulletText("%s", s.c_str());
            ImGui::PopStyleColor();

            ImGui::Columns(1);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Profile
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.5f, 1.0f),
                ">> Profile");
            ImGui::Text("   Experience    : %d year(s)",
                candidate.experience);
            ImGui::Text("   Projects      : %d",
                candidate.projects);
            ImGui::Text("   Certifications: %d",
                candidate.certifications);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // AI Suggestions
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.5f, 1.0f),
                ">> AI Personalized Suggestions");
            ImGui::Spacing();

            // Scrollable text box for AI output
            ImGui::PushStyleColor(ImGuiCol_ChildBg,
                ImVec4(0.1f, 0.1f, 0.15f, 1.0f));
            ImGui::BeginChild("ai_output", ImVec2(940, 150), true);
            ImGui::TextWrapped("%s", aiSuggestions.c_str());
            ImGui::EndChild();
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f),
                "   Report saved to output/reports/");
        }

        ImGui::End();

        // ---------------- RENDER ----------------
        ImGui::Render();
        glViewport(0, 0, 1000, 750);
        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // ---------------- CLEANUP ----------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}