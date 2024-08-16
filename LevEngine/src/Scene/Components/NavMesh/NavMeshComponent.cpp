#include "levpch.h"
#include "NavMeshComponent.h"

#include "NavMeshableComponent.h"

#include "NavMeshBoundingBox.h"
#include "Assets/MeshAsset.h"
#include "Renderer/3D/Mesh.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	NavMeshComponent::NavMeshComponent()
	{
		m_NavMeshQuery = dtAllocNavMeshQuery();
	}
	
	void NavMeshComponent::OnSceneLoaded(const Ref<Scene>& sceneRef)
	{
		auto& registry = sceneRef->GetRegistry();
		const auto view = registry.view<NavMeshComponent>();
		for (auto& entity : view)
		{
			NavMeshComponent& component = registry.get<NavMeshComponent>(entity);
			const auto entityWrapped = Entity(entt::handle{ registry, entity });
			component.ConstructComponent(entityWrapped);
		}
	}

	void NavMeshComponent::ConstructComponent(Entity entity)
	{
		m_self = entity;
		if(buildOnStart)
		{
			Build();
		}
	}
	
	void NavMeshComponent::Cleanup()
	{
		delete [] m_TriangleAreas;
		m_TriangleAreas = nullptr;
		rcFreeHeightField(m_Solid);
		m_Solid = 0;
		rcFreeCompactHeightfield(m_CompactHeightfield);
		m_CompactHeightfield = nullptr;
		rcFreeContourSet(m_ContourSet);
		m_ContourSet = nullptr;
		rcFreePolyMesh(m_PolyMesh);
		m_PolyMesh = nullptr;
		rcFreePolyMeshDetail(m_PolyMeshDetail);
		m_PolyMeshDetail = nullptr;
		dtFreeNavMesh(m_NavMesh);
		m_NavMesh = nullptr;

		IsBuilded = false;
	}

    void NavMeshComponent::Build()
    {
		Cleanup();
        //
        // Step 1. Initialize build config.
        //
		rcConfig m_Config = {};
		m_Config.cs = CellSize;
        m_Config.ch = CellHeight;
        m_Config.walkableSlopeAngle = AgentMaxSlope;
        m_Config.walkableHeight = static_cast<int>(ceilf(AgentHeight / m_Config.ch));
        m_Config.walkableClimb = static_cast<int>(floorf(AgentMaxClimb / m_Config.ch));
        m_Config.walkableRadius = static_cast<int>(ceilf(AgentRadius / m_Config.cs));
        m_Config.maxEdgeLen = static_cast<int>(EdgeMaxLen / CellSize);
        m_Config.maxSimplificationError = EdgeMaxError;
        m_Config.minRegionArea = static_cast<int>(rcSqr(RegionMinSize));		// Note: area = size*size
        m_Config.mergeRegionArea = static_cast<int>(rcSqr(RegionMergeSize));	// Note: area = size*size
        m_Config.maxVertsPerPoly = static_cast<int>(VertsPerPoly);
        m_Config.detailSampleDist = DetailSampleDist < 0.9f ? 0 : CellSize * DetailSampleDist;
        m_Config.detailSampleMaxError = CellHeight * DetailSampleMaxError;
		
		const auto& thisTransform = m_self.GetComponent<Transform>();
		if(!m_self.HasComponent<NavMeshBoundingBox>())
		{
			Log::CoreError("Build navigation: NavMeshBoundingBoxComponent on entity with NavMeshComponent is required");
			return;
		}
		const auto& navMeshBoundingBox = m_self.GetComponent<NavMeshBoundingBox>();
		
		const Vector3& transformedMinPoint = Vector3::Transform(navMeshBoundingBox.GetMinPoint(), thisTransform.GetModel());
		const Vector3& transformedMaxPoint = Vector3::Transform(navMeshBoundingBox.GetMaxPoint(), thisTransform.GetModel());
		
		auto& registry = SceneManager::GetActiveScene()->GetRegistry();
        const auto view = registry.view<Transform, NavMeshableComponent, MeshRendererComponent>();
  
		Vector<Vector3> verticesOfAllMeshes;
		Vector<uint32_t> indicesOfAllMeshes;
  
		uint32_t indexOffset = 0;
		
		for (auto& entity : view)
		{
			const auto& navMeshableComponent = view.get<NavMeshableComponent>(entity);
			if(!navMeshableComponent.UseInNavMesh)
			{
				continue;
			}
			
			auto& transform = view.get<Transform>(entity);
			transform.RecalculateModel();
			const auto& meshRendererComponent = view.get<MeshRendererComponent>(entity);
			const auto& mesh = meshRendererComponent.mesh->GetMesh();

			if(!mesh)
			{
				continue;
			}
		
			Vector<Vector3> meshVertices = mesh->GetVertices();
			Vector<uint32_t> meshIndices = mesh->GetIndices();
  
			for (auto& meshVertex : meshVertices)
			{
				meshVertex = Vector3::Transform(meshVertex, transform.GetModel());
				verticesOfAllMeshes.push_back(meshVertex);
			}
			
			for (auto meshIndex : meshIndices)
			{
				indicesOfAllMeshes.push_back(meshIndex + indexOffset);
			}
  
			indexOffset += mesh->GetVerticesCount();
		}
		
		float* vertices = &verticesOfAllMeshes.data()->x;
		const int verticesCount = verticesOfAllMeshes.size();
		
		int* triangles = reinterpret_cast<int*>(indicesOfAllMeshes.data());
		const int trianglesCount = indicesOfAllMeshes.size() / 3;
		
		const float bmin[3] = {transformedMinPoint.x, transformedMinPoint.y, transformedMinPoint.z};
		const float bmax[3] = {transformedMaxPoint.x, transformedMaxPoint.y, transformedMaxPoint.z};
            
        // Set the area where the navigation will be build.
        rcVcopy(m_Config.bmin, bmin);
        rcVcopy(m_Config.bmax, bmax);
        rcCalcGridSize(m_Config.bmin, m_Config.bmax, m_Config.cs, &m_Config.width, &m_Config.height);
  
        //
        // Step 2. Rasterize input polygon soup.
        //
        
        // Allocate voxel heightfield where we rasterize our input data to.
        m_Solid = rcAllocHeightfield();
        if (!m_Solid)
        {
            Log::CoreError("Build navigation: Could not allocate heightfield.");
            return;
        }
        if (!rcCreateHeightfield(m_Context, *m_Solid, m_Config.width, m_Config.height, m_Config.bmin,
            m_Config.bmax, m_Config.cs, m_Config.ch))
        {
            Log::CoreError("Build navigation: Could not create solid heightfield.");
            return;
        }
  
        // Allocate array that can hold triangle area types.
        // If you have multiple meshes you need to process, allocate
        // and array which can hold the max number of triangles you need to process.
        m_TriangleAreas = new unsigned char[trianglesCount];
        if (!m_TriangleAreas)
        {
            Log::CoreError("Build navigation: Out of memory 'm_TriAreas'", trianglesCount);
            return;
        }
  
        // Find triangles which are walkable based on their slope and rasterize them.
        // If your input data is multiple meshes, you can transform them here, calculate
        // the are type for each of the meshes and rasterize them.
        memset(m_TriangleAreas, 0, trianglesCount * sizeof(unsigned char));
        rcMarkWalkableTriangles(m_Context, m_Config.walkableSlopeAngle, vertices, verticesCount, triangles, trianglesCount, m_TriangleAreas);
        if (!rcRasterizeTriangles(m_Context, vertices, verticesCount, triangles, m_TriangleAreas, trianglesCount, *m_Solid, m_Config.walkableClimb))
        {
            Log::CoreError("Build navigation: Could not rasterize triangles.");
            return;
        }
  
        if (!KeepIntermediateResults)
        {
            delete [] m_TriangleAreas;
            m_TriangleAreas = nullptr;
        }
  
        //
        // Step 3. Filter walkable surfaces.
        //
  
        // Once all geometry is rasterized, we do initial pass of filtering to
        // remove unwanted overhangs caused by the conservative rasterization
        // as well as filter spans where the character cannot possibly stand.
        if (FilterLowHangingObstacles)
            rcFilterLowHangingWalkableObstacles(m_Context, m_Config.walkableClimb, *m_Solid);
        if (FilterLedgeSpans)
            rcFilterLedgeSpans(m_Context, m_Config.walkableHeight, m_Config.walkableClimb, *m_Solid);
        if (FilterWalkableLowHeightSpans)
            rcFilterWalkableLowHeightSpans(m_Context, m_Config.walkableHeight, *m_Solid);
  
        //
		// Step 4. Partition walkable surface to simple regions.
		//
  
		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		m_CompactHeightfield = rcAllocCompactHeightfield();
		if (!m_CompactHeightfield)
		{
			Log::CoreError("Build navigation: Out of memory 'CompactHeightField'.");
			return;
		}
		if (!rcBuildCompactHeightfield(m_Context, m_Config.walkableHeight, m_Config.walkableClimb, *m_Solid, *m_CompactHeightfield))
		{
			Log::CoreError("Build navigation: Could not build compact data.");
			return;
		}
  
		if (!KeepIntermediateResults)
		{
			rcFreeHeightField(m_Solid);
			m_Solid = nullptr;
		}
			
		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(m_Context, m_Config.walkableRadius, *m_CompactHeightfield))
		{
			Log::CoreError("Build navigation: Could not erode.");
			return;
		}
  
		// (Optional) Mark areas.
		// const ConvexVolume* vols = m_geom->getConvexVolumes();
		// for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
		// 	rcMarkConvexPolyArea(m_Context, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_CompactHeightfield);
    	
		// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
		// There are 3 partitioning methods, each with some pros and cons:
		// 1) Watershed partitioning
		//   - the classic Recast partitioning
		//   - creates the nicest tessellation
		//   - usually slowest
		//   - partitions the heightfield into nice regions without holes or overlaps
		//   - the are some corner cases where this method creates produces holes and overlaps
		//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
		//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
		//   * generally the best choice if you precompute the navmesh, use this if you have large open areas
		// 2) Monotone partitioning
		//   - fastest
		//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
		//   - creates long thin polygons, which sometimes causes paths with detours
		//   * use this if you want fast navmesh generation
		// 3) Layer partitoining
		//   - quite fast
		//   - partitions the heighfield into non-overlapping regions
		//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
		//   - produces better triangles than monotone partitioning
		//   - does not have the corner cases of watershed partitioning
		//   - can be slow and create a bit ugly tessellation (still better than monotone)
		//     if you have large open areas with small obstacles (not a problem if you use tiles)
		//   * good choice to use for tiled navmesh with medium and small sized tiles
  
        switch (PartitionType)
        {
			case SamplePartitionWatershed:
			{
				// Prepare for region partitioning, by calculating distance field along the walkable surface.
				if (!rcBuildDistanceField(m_Context, *m_CompactHeightfield))
				{
					Log::CoreError("Build navigation: Could not build distance field.");
					return;
				}
		
				// Partition the walkable surface into simple regions without holes.
				if (!rcBuildRegions(m_Context, *m_CompactHeightfield, 0, m_Config.minRegionArea, m_Config.mergeRegionArea))
				{
					Log::CoreError("Build navigation: Could not build watershed regions.");
					return;
				}
				break;
			}
			case SamplePartitionMonotone:
        	{
				// Partition the walkable surface into simple regions without holes.
				// Monotone partitioning does not need distancefield.
				if (!rcBuildRegionsMonotone(m_Context, *m_CompactHeightfield, 0, m_Config.minRegionArea, m_Config.mergeRegionArea))
				{
					Log::CoreError("Build navigation: Could not build monotone regions.");
					return;
				}
				break;
        	}
			case SamplePartitionLayers:
			{
				// Partition the walkable surface into simple regions without holes.
				if (!rcBuildLayerRegions(m_Context, *m_CompactHeightfield, 0, m_Config.minRegionArea))
				{
					Log::CoreError("Build navigation: Could not build layer regions.");
					return;
				}
				break;
			}
        }
  
    	
    	//
    	// Step 5. Trace and simplify region contours.
    	//
	 
    	// Create contours.
    	m_ContourSet = rcAllocContourSet();
    	if (!m_ContourSet)
    	{
    		Log::CoreError("Build navigation: Out of memory 'ContourSet'.");
    		return;
    	}
    	if (!rcBuildContours(m_Context, *m_CompactHeightfield, m_Config.maxSimplificationError, m_Config.maxEdgeLen, *m_ContourSet))
    	{
    		Log::CoreError("Build navigation: Could not create contours.");
    		return;
    	}
  
    	//
    	// Step 6. Build polygons mesh from contours.
    	//
  
    	// Build polygon navmesh from the contours.
    	m_PolyMesh = rcAllocPolyMesh();
    	if (!m_PolyMesh)
    	{
    		Log::CoreError("Build navigation: Out of memory 'PolyMesh'.");
    		return;
    	}
    	if (!rcBuildPolyMesh(m_Context, *m_ContourSet, m_Config.maxVertsPerPoly, *m_PolyMesh))
    	{
    		Log::CoreError("Build navigation: Could not triangulate contours.");
    		return;
    	}
  
    	//
		// Step 7. Create detail mesh which allows to access approximate height on each polygon.
		//
  
		m_PolyMeshDetail = rcAllocPolyMeshDetail();
		if (!m_PolyMeshDetail)
		{
			Log::CoreError("Build navigation: Out of memory 'PolyMeshDetail'.");
			return;
		}
  
		if (!rcBuildPolyMeshDetail(m_Context, *m_PolyMesh, *m_CompactHeightfield, m_Config.detailSampleDist,
			m_Config.detailSampleMaxError, *m_PolyMeshDetail))
		{
			Log::CoreError("Build navigation: Could not build detail mesh.");
			return;
		}
  
		if (!KeepIntermediateResults)
		{
			rcFreeCompactHeightfield(m_CompactHeightfield);
			m_CompactHeightfield = 0;
			rcFreeContourSet(m_ContourSet);
			m_ContourSet = nullptr;
		}
  
		// At this point the navigation mesh data is ready, you can access it from m_PolyMesh.
		// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.
  
		//
		// (Optional) Step 8. Create Detour data from Recast poly mesh.
		//
  
		// The GUI may allow more max points per polygon than Detour can handle.
		// Only build the detour navmesh if we do not exceed the limit.
		if (m_Config.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
		{
			unsigned char* navData = 0;
			int navDataSize = 0;
  
			// Update poly flags from areas.
			for (int i = 0; i < m_PolyMesh->npolys; ++i)
			{
				if (m_PolyMesh->areas[i] == RC_WALKABLE_AREA)
					m_PolyMesh->areas[i] = SamplePolyAreaGround;
					
				if (m_PolyMesh->areas[i] == SamplePolyAreaGround ||
					m_PolyMesh->areas[i] == SamplePolyAreaGrass ||
					m_PolyMesh->areas[i] == SamplePolyAreaRoad)
				{
					m_PolyMesh->flags[i] = SamplePolyFlagsWalk;
				}
				else if (m_PolyMesh->areas[i] == SamplePolyAreaWater)
				{
					m_PolyMesh->flags[i] = SamplePolyFlagsSwim;
				}
				else if (m_PolyMesh->areas[i] == SamplePolyFlagsDoor)
				{
					m_PolyMesh->flags[i] = SamplePolyFlagsWalk | SamplePolyFlagsDoor;
				}
			}
  
  
			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = m_PolyMesh->verts;
			params.vertCount = m_PolyMesh->nverts;
			params.polys = m_PolyMesh->polys;
			params.polyAreas = m_PolyMesh->areas;
			params.polyFlags = m_PolyMesh->flags;
			params.polyCount = m_PolyMesh->npolys;
			params.nvp = m_PolyMesh->nvp;
			params.detailMeshes = m_PolyMeshDetail->meshes;
			params.detailVerts = m_PolyMeshDetail->verts;
			params.detailVertsCount = m_PolyMeshDetail->nverts;
			params.detailTris = m_PolyMeshDetail->tris;
			params.detailTriCount = m_PolyMeshDetail->ntris;
			params.offMeshConCount = 0;
			params.walkableHeight = AgentHeight;
			params.walkableRadius = AgentRadius;
			params.walkableClimb = AgentMaxClimb;
			rcVcopy(params.bmin, m_PolyMesh->bmin);
			rcVcopy(params.bmax, m_PolyMesh->bmax);
			params.cs = m_Config.cs;
			params.ch = m_Config.ch;
			params.buildBvTree = true;
			
			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				Log::CoreError("Could not build Detour navmesh.");
				return;
			}
			
			m_NavMesh = dtAllocNavMesh();
			if (!m_NavMesh)
			{
				dtFree(navData);
				Log::CoreError("Could not create Detour navmesh");
				return;
			}
			
			dtStatus status;
			
			status = m_NavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
			if (dtStatusFailed(status))
			{
				dtFree(navData);
				Log::CoreError("Could not init Detour navmesh");
				return;
			}
			
			status = m_NavMeshQuery->init(m_NavMesh, 2048);
			if (dtStatusFailed(status))
			{
				Log::CoreError("Could not init Detour navmesh query");
				return;
			}
		}
  
		Log::CoreInfo("Navmesh builded successfully.");
  
		// // Show performance stats.
		// duLogBuildTimes(*m_Context, m_Context->getAccumulatedTime(RC_TIMER_TOTAL));
		// m_ctx->log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", m_PolyMesh->nverts, m_PolyMesh->npolys);
		//
		// m_totalBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL)/1000.0f;
		//
		// if (m_tool)
		// 	m_tool->init(this);
		// initToolStates(this);
  
		IsBuilded = true;
    }

	dtNavMesh* NavMeshComponent::GetNavMesh() const
    {
		return m_NavMesh;
    }

	dtNavMeshQuery* NavMeshComponent::GetNavMeshQuery() const
	{
		return m_NavMeshQuery;
	}

    const rcPolyMesh& NavMeshComponent::GetPolyMesh() const
	{
		return *m_PolyMesh;
	}

    class NavMeshComponentSerializer final : public ComponentSerializer<NavMeshComponent, NavMeshComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "NavMesh"; }
        
        void SerializeData(YAML::Emitter& out, const NavMeshComponent& component) override
        {
        	out << YAML::Key << "Build on start" << YAML::Value << component.buildOnStart;
            out << YAML::Key << "Cell size" << YAML::Value << component.CellSize;
            out << YAML::Key << "Cell Height" << YAML::Value << component.CellHeight;
            out << YAML::Key << "Agent Height" << YAML::Value << component.AgentHeight;
            out << YAML::Key << "Agent radius" << YAML::Value << component.AgentRadius;
            out << YAML::Key << "Agent max climb" << YAML::Value << component.AgentMaxClimb;
            out << YAML::Key << "Agent max slope" << YAML::Value << component.AgentMaxSlope;
            out << YAML::Key << "Region min size" << YAML::Value << component.RegionMinSize;
            out << YAML::Key << "Region merge size" << YAML::Value << component.RegionMergeSize;
            out << YAML::Key << "Edge max len" << YAML::Value << component.EdgeMaxLen;
            out << YAML::Key << "Edge max error" << YAML::Value << component.EdgeMaxError;
            out << YAML::Key << "Verts per poly" << YAML::Value << component.VertsPerPoly;
            out << YAML::Key << "Detail sample dist" << YAML::Value << component.DetailSampleDist;
            out << YAML::Key << "Detail sample max error" << YAML::Value << component.DetailSampleMaxError;
            out << YAML::Key << "Partition type" << YAML::Value << component.PartitionType;
        	out << YAML::Key << "Filter low hanging obstacles" << YAML::Value << component.FilterLowHangingObstacles;
        	out << YAML::Key << "Filter ledge spans" << YAML::Value << component.FilterLedgeSpans;
        	out << YAML::Key << "Filter walkable low height spans" << YAML::Value << component.FilterWalkableLowHeightSpans;
        	out << YAML::Key << "Keep intermediate results" << YAML::Value << component.KeepIntermediateResults;
        }
        
        void DeserializeData(const YAML::Node& node, NavMeshComponent& component) override
        {
        	component.buildOnStart = node["Build on start"].as<bool>();
            component.CellSize = node["Cell size"].as<float>();
            component.CellHeight = node["Cell Height"].as<float>();
            component.AgentHeight = node["Agent Height"].as<float>();
            component.AgentRadius = node["Agent radius"].as<float>();
            component.AgentMaxClimb = node["Agent max climb"].as<float>();
            component.AgentMaxSlope = node["Agent max slope"].as<float>();
            component.RegionMinSize = node["Region min size"].as<float>();
            component.RegionMergeSize = node["Region merge size"].as<float>();
            component.EdgeMaxLen = node["Edge max len"].as<float>();
            component.EdgeMaxError = node["Edge max error"].as<float>();
            component.VertsPerPoly = node["Verts per poly"].as<float>();
            component.DetailSampleDist = node["Detail sample dist"].as<float>();
            component.DetailSampleMaxError = node["Detail sample max error"].as<float>();
            component.PartitionType = static_cast<SamplePartitionType>(node["Partition type"].as<int>());
        	component.FilterLowHangingObstacles = node["Filter low hanging obstacles"].as<bool>();
        	component.FilterLedgeSpans = node["Filter ledge spans"].as<bool>();
        	component.FilterWalkableLowHeightSpans = node["Filter walkable low height spans"].as<bool>();
        	component.KeepIntermediateResults = node["Keep intermediate results"].as<bool>();
        }
    };
}
