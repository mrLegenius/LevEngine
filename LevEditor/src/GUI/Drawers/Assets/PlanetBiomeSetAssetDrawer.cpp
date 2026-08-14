#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/EditorGUI.h"
#include "Assets/PlanetBiomeSetAsset.h"

namespace LevEngine::Editor
{
	class PlanetBiomeSetAssetDrawer final : public AssetDrawer<PlanetBiomeSetAsset, PlanetBiomeSetAssetDrawer>
	{
	protected:
		[[nodiscard]] String GetLabel() const override { return "PlanetBiomeSetAsset"; }

		void DrawContent(const Ref<PlanetBiomeSetAsset> assetRef) override
		{
			ImGui::TextWrapped(
				"Ground textures on the left, biomes on the right. A biome is a window in temperature, "
				"rainfall, height and slope, and the ground takes whichever biomes it qualifies for, "
				"blended by how well it qualifies -- so nothing here says where anything goes.");

			//<--- Textures ---<<

			ImGui::SeparatorText("Ground Textures");
			ImGui::TextWrapped(
				"One slice of the texture arrays each, referenced by a biome's Texture index. They are a "
				"separate list because biomes share ground: savanna and steppe are the same dry grass. "
				"All slices are resampled to the size of the first, since a texture array has one size.");

			Vector<PlanetBiomeSetAsset::GroundTextures>& textures = assetRef->GetTextures();

			int textureToRemove = -1;

			for (size_t index = 0; index < textures.size(); ++index)
			{
				PlanetBiomeSetAsset::GroundTextures& entry = textures[index];

				ImGui::PushID(static_cast<int>(index));

				const String header = Format("[{0}] {1}", index, entry.Name);

				if (ImGui::TreeNode(header.c_str()))
				{
					char name[128]{};
					strncpy_s(name, entry.Name.c_str(), sizeof name - 1);

					if (ImGui::InputText("Name", name, sizeof name))
						entry.Name = name;

					bool changed = false;
					changed |= EditorGUI::DrawTextureAsset("Albedo", entry.Albedo);
					changed |= EditorGUI::DrawTextureAsset("Normal", entry.Normal);
					changed |= EditorGUI::DrawTextureAsset("Roughness", entry.Roughness);

					//<--- The arrays are packed from these, so a change means repacking them ---<<
					if (changed)
						assetRef->InvalidateTextures();

					if (ImGui::Button("Remove Texture"))
						textureToRemove = static_cast<int>(index);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			if (textureToRemove >= 0)
			{
				// Removing a slice shifts every slice after it, and biomes index them by number, so the
				// indices are fixed up rather than left pointing at the wrong ground.
				textures.erase(textures.begin() + textureToRemove);

				for (PlanetBiome& biome : assetRef->GetBiomes())
				{
					if (biome.TextureIndex > textureToRemove)
						--biome.TextureIndex;
					else if (biome.TextureIndex == textureToRemove)
						biome.TextureIndex = 0;
				}

				assetRef->InvalidateTextures();
			}

			if (ImGui::Button("Add Texture"))
			{
				textures.emplace_back(PlanetBiomeSetAsset::GroundTextures{});
				assetRef->InvalidateTextures();
			}

			//<--- Biomes ---<<

			ImGui::SeparatorText("Biomes");

			Vector<PlanetBiome>& biomes = assetRef->GetBiomes();

			if (biomes.size() >= k_MaxPlanetBiomes)
			{
				ImGui::TextWrapped("The table is full at %u biomes. It is uploaded to a constant buffer "
				                   "and read by every pixel of the planet, so it is a fixed size.",
				                   k_MaxPlanetBiomes);
			}

			int biomeToRemove = -1;

			for (size_t index = 0; index < biomes.size(); ++index)
			{
				PlanetBiome& biome = biomes[index];

				ImGui::PushID(static_cast<int>(1000 + index));

				if (ImGui::TreeNode(biome.Name.c_str()))
				{
					char name[128]{};
					strncpy_s(name, biome.Name.c_str(), sizeof name - 1);

					if (ImGui::InputText("Name", name, sizeof name))
						biome.Name = name;

					ImGui::SeparatorText("Where it lives");

					ImGui::DragFloatRange2("Temperature", &biome.MinTemperature, &biome.MaxTemperature,
					                       0.5f, -150.0f, 200.0f, "%.1f C", "%.1f C");
					ImGui::DragFloatRange2("Humidity", &biome.MinHumidity, &biome.MaxHumidity,
					                       0.01f, 0.0f, 1.0f);
					ImGui::DragFloatRange2("Height", &biome.MinHeight, &biome.MaxHeight,
					                       5.0f, -1000000.0f, 1000000.0f);
					ImGui::DragFloatRange2("Slope", &biome.MinSlope, &biome.MaxSlope, 0.01f, 0.0f, 1.0f);
					ImGui::SetItemTooltip("One minus the cosine of the angle from straight up: 0 is level "
					                      "ground, 1 is a vertical wall. This is what keeps grass off cliffs.");

					ImGui::DragFloat("Priority", &biome.Priority, 0.05f, 0.0f, 10.0f);
					ImGui::SetItemTooltip("Where it stands when several biomes fit equally well. A cliff "
					                      "and the meadow at its foot share a climate, so the slope has "
					                      "to win -- which it does by having a higher priority.");

					if (ImGui::TreeNode("Edge fades"))
					{
						ImGui::TextWrapped("Width of the fade at each edge of the four windows above, in "
						                   "the units of that window. Zero gives a hard line, which reads "
						                   "as a contour drawn on the ground.");

						ImGui::DragFloat("Temperature Blend", &biome.TemperatureBlend, 0.1f, 0.0f, 50.0f);
						ImGui::DragFloat("Humidity Blend", &biome.HumidityBlend, 0.005f, 0.0f, 1.0f);
						ImGui::DragFloat("Height Blend", &biome.HeightBlend, 1.0f, 0.0f, 10000.0f);
						ImGui::DragFloat("Slope Blend", &biome.SlopeBlend, 0.005f, 0.0f, 1.0f);

						ImGui::TreePop();
					}

					ImGui::SeparatorText("How it looks");

					ImGui::ColorEdit3("Tint", biome.Tint.Raw());
					ImGui::SetItemTooltip("Multiplied into the albedo texture, so one texture can serve "
					                      "several biomes. With no texture assigned this is the ground.");

					ImGui::DragFloat("Roughness", &biome.Roughness, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Metallic", &biome.Metallic, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Texture Scale", &biome.TextureScale, 0.1f, 0.01f, 10000.0f);
					ImGui::SetItemTooltip("World units one tile of the texture covers.");

					ImGui::DragFloat("Normal Strength", &biome.NormalStrength, 0.02f, 0.0f, 4.0f);

					int textureIndex = biome.TextureIndex;
					const int textureCount = Math::Max(1, static_cast<int>(textures.size()));

					if (ImGui::DragInt("Texture", &textureIndex, 1, 0, textureCount - 1))
						biome.TextureIndex = Math::Clamp(textureIndex, 0, textureCount - 1);

					if (biome.TextureIndex < static_cast<int>(textures.size()))
						ImGui::TextUnformatted(textures[biome.TextureIndex].Name.c_str());

					if (ImGui::Button("Remove Biome"))
						biomeToRemove = static_cast<int>(index);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			if (biomeToRemove >= 0)
				biomes.erase(biomes.begin() + biomeToRemove);

			if (biomes.size() < k_MaxPlanetBiomes && ImGui::Button("Add Biome"))
				biomes.emplace_back(PlanetBiome{});

			ImGui::Separator();

			if (ImGui::Button("Reset To Earthlike"))
				assetRef->ResetToEarthlike();

			ImGui::SetItemTooltip("Replaces the whole set with sixteen biomes covering an Earthlike "
			                      "planet from the sea floor to the snow line, and a texture slot for each.");
		}
	};
}
