#include <physics/mesh_splitter_utils/mesh_splitter.hpp>

namespace legion::physics
{
    void MeshSplitter::MultipleSplitMesh(const std::vector<MeshSplitParams>& splittingPlanes,
        std::vector<ecs::entity_handle>& entitiesGenerated, bool keepBelow)
    {
        auto [posH, rotH, scaleH] = owner.get_component_handles<transform>();
        const math::mat4& transform = math::compose(scaleH.read(), rotH.read(), posH.read());

        //-------------------------------- copy polygons of original mesh and add it to the output list -----------------------------------------//

        std::vector< std::vector<SplittablePolygonPtr>> outputPolygonIslandsGenerated;

        std::vector<SplittablePolygonPtr> copiedPolygons;
        CopyPolygons(meshPolygons, copiedPolygons);

        outputPolygonIslandsGenerated.push_back(std::move(copiedPolygons));

        //-------------------------------- spllit mesh based on list of splitting planes -----------------------------------------//
        for (const MeshSplitParams& splitParam : splittingPlanes)
        {
            std::vector< std::vector<SplittablePolygonPtr>> inputList = std::move(outputPolygonIslandsGenerated);
            outputPolygonIslandsGenerated.clear();

            for (std::vector<SplittablePolygonPtr>& polygonIsland : inputList)
            {
                for (SplittablePolygonPtr polygon : polygonIsland)
                {
                    polygon->isVisited = false;
                }

                SplitPolygons
                (polygonIsland,
                    splitParam.planeNormal,
                    splitParam.planePostion,
                    transform,
                    outputPolygonIslandsGenerated);
            }
        }

        //-------------------------------- use each polygon list to create a new object -----------------------------------------//

        for (auto& polygonIsland : outputPolygonIslandsGenerated)
        {
            PrimitiveMesh newMesh(owner, polygonIsland, ownerMaterialH);
            auto newEnt = newMesh.InstantiateNewGameObject();

            entitiesGenerated.push_back(newEnt);
        }
    }

    void MeshSplitter::SplitPolygons(std::vector<SplittablePolygonPtr>& polygonsToSplit, const math::vec3& planeNormal, const math::vec3& planePosition,
        const math::mat4& transform, std::vector<std::vector<SplittablePolygonPtr>>& resultingIslands, bool keepBelow)
    {
        log::debug("SplitPolygons");

        //----------------------- Find out which polygons are below,above, or intersecting the splitting plane -----------------------------------//

        for (auto polygon : polygonsToSplit)
        {
            polygon->isVisited = false;
            polygon->CalculatePolygonSplit(transform, planePosition, planeNormal);

        }

        SplitState requestedState = keepBelow ? SplitState::Below : SplitState::Above;

        SplittablePolygonPtr initialFound = nullptr;

        bool foundUnvisited =
            FindFirstIntersectingOrRequestedState
            (initialFound, requestedState, polygonsToSplit);

        if (math::distance(planeNormal, math::vec3(-1, 0, 0)) < 0.01f)
        {
            DebugBreak();
        }
        //while there is an unvisited polygon that is on the requestedState or is intersecting the splitting plane
        while (foundUnvisited)
        {
            std::vector<SplittablePolygonPtr> splitMesh;
            std::vector<SplittablePolygonPtr> nonSplitMesh;

            //------------------------ BFS search polygons that are in the same island -------------------------------------------//
            //------------------------ and  divide them into a list of split and nonsplit polygons--------------------------//
            initialFound->isVisited = false;
            BFSFindRequestedAndIntersecting(
                initialFound,
                splitMesh,
                nonSplitMesh, requestedState);


            std::vector < std::vector <meshHalfEdgePtr>> holeIslands;
            std::vector<std::vector<SplittablePolygonPtr>> intersectionIslands;

            //----------------------------------- Detect multiple holes in mesh  --------------------------------------------------//

            DetectIntersectionIsland(splitMesh, intersectionIslands);

            //----------------------------------- Filter Edges in polygon in order to fit sliced mesh --------------------------------------------------//

            std::vector<IntersectionEdgeInfo> generatedIntersectionEdges;

            for (std::vector<SplittablePolygonPtr>& intersectionIsland : intersectionIslands)
            {
                for (SplittablePolygonPtr islandPolygon : intersectionIsland)
                {
                    SplitPolygon(islandPolygon, transform, planePosition, planeNormal, requestedState, generatedIntersectionEdges);
                }
            }

            math::vec3 localNormal = transform * math::vec4(planeNormal, 0);
            SplittablePolygonPtr intersectionPolygon = CreateIntersectionPolygon(generatedIntersectionEdges, math::normalize(localNormal));
            intersectionPolygon->isVisited = true;
            intersectionPolygon->ResetEdgeVisited();

            //---------------------------------- Add intersecting and nonsplit to primitive mesh ---------------------------------//

            std::vector< SplittablePolygonPtr> resultPolygons;

            resultPolygons.insert(resultPolygons.end()
                , std::make_move_iterator(splitMesh.begin()), std::make_move_iterator(splitMesh.end()));

            resultPolygons.insert(resultPolygons.end()
                , std::make_move_iterator(nonSplitMesh.begin()), std::make_move_iterator(nonSplitMesh.end()));

            resultPolygons.push_back(std::move(intersectionPolygon));

            foundUnvisited =
                FindFirstIntersectingOrRequestedState
                (initialFound, requestedState, polygonsToSplit);

            resultingIslands.push_back(std::move(resultPolygons));

        }
    }

    void MeshSplitter::CopyPolygons(std::vector<SplittablePolygonPtr>& originalSplitMesh,
        std::vector<SplittablePolygonPtr>& copySplitMesh)
    {

        //----copy all edges of all polygons and connect them to each edge that is within the same polygon--//
        for (SplittablePolygonPtr originalPolygon : originalSplitMesh)
        {
            originalPolygon->ResetEdgeVisited();

            std::vector<meshHalfEdgePtr> copyPolygonEdges;
            CopyEdgeVector(originalPolygon->GetMeshEdges(), copyPolygonEdges);

            auto copyPolygon = std::make_shared<SplittablePolygon>(copyPolygonEdges, originalPolygon->localNormal);
            copyPolygon->AssignEdgeOwnership();
            copySplitMesh.push_back(copyPolygon);
        }

        //----use shadow edge to connect boundary edges between copied polygons--//

        for (SplittablePolygonPtr originalPolygon : originalSplitMesh)
        {


            for (auto originalEdge : originalPolygon->GetMeshEdges())
            {
                if (originalEdge->isBoundary && originalEdge->pairingEdge)
                {
                    auto shadowEdge = originalEdge->shadowEdge;
                    auto shadowEdgePairing = originalEdge->pairingEdge->shadowEdge;

                    shadowEdge->setPairing(shadowEdgePairing);
                }

            }
        }

        //----nullify shadow edge for each edge in the original polygon list--//

        for (SplittablePolygonPtr originalPolygon : originalSplitMesh)
        {
            originalPolygon->ResetEdgeVisited();

            for (auto originalEdge : originalPolygon->GetMeshEdges())
            {
                originalEdge->shadowEdge = nullptr;
            }
        }
    }

    void MeshSplitter::CopyEdgeVector(std::vector<meshHalfEdgePtr>& originalHalfEdgeList, std::vector<meshHalfEdgePtr>& resultCopyList)
    {
        for (meshHalfEdgePtr originalEdge : originalHalfEdgeList)
        {
            //copy polygon and place on shadow edge
            originalEdge->cloneOnShadowEdge();
        }

        //BFS connect with clone edge
        std::queue<meshHalfEdgePtr> unvisitedOriginalHalfEdgeList;
        unvisitedOriginalHalfEdgeList.push(originalHalfEdgeList[0]);

        while (!unvisitedOriginalHalfEdgeList.empty())
        {
            auto originalEdge = unvisitedOriginalHalfEdgeList.front();
            unvisitedOriginalHalfEdgeList.pop();

            if (!originalEdge->isVisited)
            {
                originalEdge->isVisited = true;

                auto [original1, original2, original3] = originalEdge->getTriangle();
                auto [shadow1, shadow2, shadow3] = originalEdge->getShadowTriangle();

                MeshHalfEdge::connectIntoTriangle(shadow1, shadow2, shadow3);

                if (!original1->isBoundary)
                {
                    shadow1->setPairing(original1->pairingEdge->shadowEdge);
                    unvisitedOriginalHalfEdgeList.push(original1->pairingEdge);
                }

                if (!original2->isBoundary)
                {
                    shadow2->setPairing(original2->pairingEdge->shadowEdge);
                    unvisitedOriginalHalfEdgeList.push(original2->pairingEdge);
                }

                if (!original3->isBoundary)
                {
                    shadow3->setPairing(original3->pairingEdge->shadowEdge);
                    unvisitedOriginalHalfEdgeList.push(original3->pairingEdge);
                }

                shadow1->populateVectorWithTriangle(resultCopyList);

            }
        }
    }

}


