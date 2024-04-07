#pragma once

#include <Core.hpp>
#include <Core/Time.hpp>

#include "GuiWindow.hpp"

// utility structure for realtime plot
struct ScrollingBuffer {
	int MaxSize;
	int Offset;
	ImVector<ImVec2> Data;
	ScrollingBuffer(int max_size = 2000) {
		MaxSize = max_size;
		Offset = 0;
		Data.reserve(MaxSize);
	}
	void AddPoint(float x, float y) {
		if (Data.size() < MaxSize)
			Data.push_back(ImVec2(x, y));
		else {
			Data[Offset] = ImVec2(x, y);
			Offset = (Offset + 1) % MaxSize;
		}
	}
	void Erase() {
		if (Data.size() > 0) {
			Data.shrink(0);
			Offset = 0;
		}
	}
};

struct SettingsWindowOutput {

};

class SettingsWindow : public GuiWindow {
#define SETTINGS_NAME "Settings"

public:
	SettingsWindow() {}

	virtual const char* GetName() const override {
		return SETTINGS_NAME;
	};

	SettingsWindowOutput Draw(std::chrono::nanoseconds renderTimeNs, Renderer& renderer) {
		SettingsWindowOutput output{};
		
		using namespace std::chrono_literals;
		
		static ScrollingBuffer scrollingFrameData;
		static float t = 0;
		t += Time::DeltaTime();
		scrollingFrameData.AddPoint(t, Time::DeltaTime() * 1000.0f);

		static float history = 10.0f;
		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_GEAR " " SETTINGS_NAME, &m_DrawThis)) {
			if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150))) {
				ImPlot::SetupAxes("Time", "Frame Time (ms)", flags, flags);
				ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);
				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
				ImPlot::PlotShaded("Frame (ms)", &scrollingFrameData.Data[0].x, &scrollingFrameData.Data[0].y, scrollingFrameData.Data.size(), -INFINITY, 0, scrollingFrameData.Offset, 2 * sizeof(float));
				ImPlot::EndPlot();
			}

			if (ImGui::BeginTable("Settings", 2)) {
				ImGui::TableNextColumn();
				ImGui::Text("Render Time");
				ImGui::TableNextColumn();
				ImGui::Text(std::format("{:.2f} ms", renderTimeNs / 1.0ms).c_str());

				ImGui::TableNextColumn();
				ImGui::Text("Frame Time");
				ImGui::TableNextColumn();
				ImGui::Text(std::format("{:.2f} ms", Time::DeltaTime()*1000.0f).c_str());

				ImGui::TableNextColumn();
				ImGui::Separator();
				ImGui::TableNextColumn();
				ImGui::Separator();

				ImGui::TableNextColumn();
				ImGui::Text("Render Grid");
				ImGui::TableNextColumn();
				ImGui::Checkbox("##RenderGrid", &renderer.renderGrid);

				ImGui::TableNextColumn();
				ImGui::Text("Exposure");
				ImGui::TableNextColumn();
				ImGui::DragFloat("##Exposure", &renderer.exposure, 0.005f, -1.0f, 10.0f);

				ImGui::TableNextColumn();
				ImGui::Text("Gamma");
				ImGui::TableNextColumn();
				ImGui::DragFloat("##Gamma", &renderer.gamma, 0.005f, -1.0f, 10.0f);

				ImGui::TableNextColumn();
				ImGui::Text("Skybox HDR");
				ImGui::TableNextColumn();
				if (DynamicAssetField(renderer.hdrSkybox, 0)) {
					renderer.HdrToCubemaps();
				}

				ImGui::TableNextColumn();
				ImGui::Text("Show Irradiance");
				ImGui::TableNextColumn();
				ImGui::Checkbox("##ShowIrradiance", &renderer.showIrradiance);

				ImGui::TableNextColumn();
				ImGui::Text("Render Wireframe");
				ImGui::TableNextColumn();
				ImGui::Checkbox("##RenderWireframe", &renderer.wireframeOverride);

				ImGui::EndTable();

			}
		}
		ImGui::End();

		return output;
	}
};